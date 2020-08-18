/*
    specifying uniform names in the "properties" block
    will make them show up in the material editor.
    
    Syntax:

        uniformName | [ comma seperated attributes ];

    Available Attributes:
        
        COLOR               -   the vec3 or vec4 shows up in the editor as a color picker
        
        RANGE(<min>, <max>) -   the value is clamped between min and max
        
        DEF(<value>)        -   gives the value a default value 
                                (example: DEF(1,0,1) is default vec3(1,0,1)

        BUMP                -   if the sampler is not set to any texture, a default tangent space bump 
                                map will be set (recommended for all normal map samplers)
*/

#properties
{
    u_AlbedoColor           | [COLOR        : DEF(1, 1, 1)];
    u_Metalness             | [RANGE(0, 1)  : DEF(1)];
    u_Roughness             | [RANGE(0, 1)  : DEF(1)];
    
    u_AlbedoTex;
    u_NormalTex             | [BUMP];
    u_MetalnessTex;
    u_RoughnessTex;
    u_AmbientOcclusionTex;
}

/*
    "STANDARD_VARS" flag gives access to the following uniforms and attributes:
        vertex shader only:
            mat4 ares_VMatrix
            mat4 ares_VPMatrix
            mat4 ares_Object2World;
            mat4 ares_MVPMatrix;

            vec3 ares_ObjectPos;
            vec2 ares_ObjectUVs;
            vec3 ares_ObjectNormals;
            vec3 ares_ObjectTangents;

    "SKINNED" flag compiles the shader with variant that can be used with an animated mesh.
        - using this wihtout "STANDARD_VARS" flag could lead to undefined behavior
*/
#flags 
{
    STANDARD_VARS, SKINNED
}

/*
    this shared block will be accessible betwen vertex and fragment shaders
    the "varying" keyword is safe to use here as it will be replaced
    with "in" and "out" when sent to either vertex or fragment shaders
*/
#shared
{
    varying vec2 TexCoords;
    varying vec3 WorldPos;
    varying vec3 Normal;
    varying mat3 TBN;
    varying vec3 CameraPos;
}

#type vertex
#version 430

void main()
{
    gl_Position = ares_MVPMatrix * vec4(ares_ObjectPos, 1.0);

    TexCoords = ares_ObjectUVs;
    CameraPos = inverse(ares_VMatrix)[3].xyz;
    WorldPos = vec3(ares_Object2World * vec4(ares_ObjectPos, 1.0));

    mat3 obj2World3 = mat3(ares_Object2World);
    Normal = normalize(obj2World3 * ares_ObjectNormals);
    vec3 T = normalize(obj2World3 * ares_ObjectTangents);
    T = normalize(T - (dot(T, Normal) * Normal));
    vec3 B = normalize(cross(Normal, T));
    TBN = mat3(T, B, Normal);
}

#type fragment
#version 430

uniform vec3 u_AlbedoColor;
uniform float u_Metalness;
uniform float u_Roughness;

// PBR texture inputs
uniform sampler2D u_AlbedoTex;
uniform sampler2D u_NormalTex;
uniform sampler2D u_MetalnessTex;
uniform sampler2D u_RoughnessTex;
uniform sampler2D u_AmbientOcclusionTex;

// BRDF LUT
uniform sampler2D u_BRDFLUTTexture;

// Environment maps
uniform samplerCube u_EnvRadianceTex;
uniform samplerCube u_EnvIrradianceTex;

// lights
struct Light 
{
    vec3 Direction;
    vec3 Color;
};

uniform Light ares_Light;

const float PI = 3.14159265359;

// PBR FUNCTIONS ----------------------------------------------------------------------------
float DistributionGGX(vec3 worldNorm, vec3 halfwayVector, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(worldNorm, halfwayVector), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / denom;
}
// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}
// Schlick-GGX approximation of geometric attenuation (GeometrySmith) function using Smith's method.
float gaSchlickGGX(float NdotV, float NdotL, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(NdotL, k) * gaSchlickG1(NdotV, k);
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------

vec3 GetWorldNormal()
{
    vec3 tangentSpaceNormal = texture(u_NormalTex, TexCoords).xyz * 2.0 - 1.0;
    // convert to world space
    return normalize(TBN * tangentSpaceNormal);
}


void main()
{
    vec3 albedo = texture(u_AlbedoTex, TexCoords).rgb * u_AlbedoColor;
    float metallic = texture(u_MetalnessTex, TexCoords).r * u_Metalness;
    float roughness = texture(u_RoughnessTex, TexCoords).r * u_Roughness;
    float ao = texture(u_AmbientOcclusionTex, TexCoords).r;

    vec3 worldNorm = GetWorldNormal();

    vec3 viewDir = normalize(CameraPos - WorldPos);
    
    float NdotV = max(dot(worldNorm, viewDir), 0.0);
    
    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 lightContribution = vec3(0.0);
    
    //for (int i = 0; i < 4; ++i)
    {
        // calculate per-light radiance

        vec3 lightDir = -normalize(ares_Light.Direction);

        vec3 halfwayVector = normalize(viewDir + lightDir);

        float attenuation = 1.0;
        
        vec3 radiance = ares_Light.Color * attenuation;

        
        /*
        Then, for each light we want to calculate the full Cook-Torrance specular BRDF term:

                    DFG
                ------------
            4 * (w0 * n) * (w1 * n)
            
        The first thing we want to do is calculate the ratio between 
        specular and diffuse reflection, or how much the surface reflects 
        light versus how much it refracts light. 
        The Fresnel equation calculates just that:

        The Fresnel-Schlick approximation expects a F0 parameter 
        which is known as the surface reflection at zero incidence or 
        how much the surface reflects if looking directly at the surface. 
        The F0 varies per material and is tinted on metals as we find in 
        large material databases. In the PBR metallic workflow we make 
        the simplifying assumption that most dielectric surfaces look visually 
        correct with a constant F0 of 0.04, while we do specify F0 for metallic 
        surfaces as then given by the albedo value.

        from above: 
            vec3 F0 = vec3(0.04);
            F0 = mix(F0, u_AlbedoColor, u_Metalness);
        */

        float NdotL = max(dot(worldNorm, lightDir), 0.0);
        float HdotV = max(dot(halfwayVector, viewDir), 0.0);

        vec3 F = fresnelSchlick(HdotV, F0);

        /*
        Given F, the remaining terms to calculate are the 
        normal distribution function D 
        and the geometry function G.
        */
        
        // Cook-Torrance BRDF
        float D = DistributionGGX(worldNorm, halfwayVector, roughness);
        float G = gaSchlickGGX(NdotV, NdotL, roughness);
        
        // prevent divide by zero for NdotV=0.0 or NdotL=0.0
        vec3 specularBRDF = (D * G * F) / (4 * NdotV * NdotL + 0.001);

        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - Fresnel
        
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
        
        // add to outgoing radiance Lo
        // note that we already multiplied the BRDF by the Fresnel (kS) 
        // so we won't multiply by kS again

        lightContribution += (kD * albedo / PI + specularBRDF) * radiance * NdotL;
        //lightContribution += (kD * albedo + specularBRDF) * radiance * NdotL;
    }

    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(u_EnvIrradianceTex, worldNorm).rgb;
    
    vec3 diffuse = irradiance * albedo;

    vec3 R = reflect(-viewDir, worldNorm);
    int u_EnvRadianceTexLevels = textureQueryLevels(u_EnvRadianceTex);
    vec3 prefilteredColor = textureLod(u_EnvRadianceTex, R, roughness * u_EnvRadianceTexLevels).rgb;

    vec2 envBRDF = texture(u_BRDFLUTTexture, vec2(NdotV, roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + lightContribution;

    // HDR tonemapping
    //color = color / (color + vec3(1.0));
    // gamma correct
    //color = pow(color, vec3(1.0 / 2.2));

    out_Color = vec4(color, 1.0);
}