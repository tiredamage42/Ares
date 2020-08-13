#type vertex
#version 430
/*
The radiance equation also depends on a position p, 
which we've assumed to be at the center of the irradiance map. 
This does mean all diffuse indirect light must come from a single environment map 
which may break the illusion of reality (especially indoors). 

Render engines solve this by placing reflection probes all over the scene 
where each reflection probes calculates its own irradiance map of its surroundings. 
This way, the irradiance (and radiance) at position p is the interpolated 
irradiance between its closest reflection probes. 
For now, we assume we always sample the environment map from its center.
*/


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out mat3 TBN;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_Transform;

void main()
{
    TexCoords = aTexCoords;
    WorldPos = vec3(u_Transform * vec4(aPos, 1.0));

    mat3 transform3 = mat3(u_Transform);

    Normal = transform3 * aNormal;
    //Normal = transpose(inverse(u_Transform)) * aNormal;


    vec3 B = cross(aNormal, aTangent);
    TBN = transform3 * mat3(aTangent, B, aNormal);

    /* maybe:
        vec3 T = normalize(transform3 * aTangent);
        vec3 N = normalize(transform3 * aNormal);
        vec3 B = cross(N, T);
        TBN = mat3(T, B, N);
    */

    gl_Position = u_ViewProjectionMatrix * vec4(WorldPos, 1.0);
}

#type fragment
#version 430
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in mat3 TBN;

// material parameters
//uniform vec3 albedo;
//uniform float metallic;
//uniform float roughness;
//uniform float ao;


uniform vec3 u_AlbedoColor;
uniform float u_Metalness;
uniform float u_Roughness;

// PBR texture inputs
uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_MetalnessTexture;
uniform sampler2D u_RoughnessTexture;

uniform float u_AlbedoTexToggle;
uniform float u_NormalTexToggle;
uniform float u_MetalnessTexToggle;
uniform float u_RoughnessTexToggle;

// BRDF LUT
uniform sampler2D u_BRDFLUTTexture;


// Environment maps
uniform samplerCube u_EnvRadianceTex;
uniform samplerCube u_EnvIrradianceTex;


// lights
//uniform vec3 lightPositions[4];
//uniform vec3 lightColors[4];
struct Light {
    vec3 Direction;
    vec3 Radiance;
    float Multiplier;
};


uniform Light lights;
uniform vec3 u_CameraPosition;

//uniform vec3 camPos; 

const float PI = 3.14159265359;


// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
vec3 getNormalFromMap()
{
    /*vec3 Q1 = dFdx(WorldPos);
    vec3 Q2 = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);*/

    vec3 tangentNormal = texture(u_NormalTexture, TexCoords).xyz * 2.0 - 1.0;
    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 worldNorm, vec3 halfwayVector, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(worldNorm, halfwayVector), 0.0);
    float NdotH2 = NdotH * NdotH;

    //float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    //return a2 / max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
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

/*
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
//float GeometrySmith(vec3 worldNorm, vec3 viewDir, vec3 lightDir, float roughness)
float GeometrySmith(float NdotV, float NdotL, float roughness)

{
    //float NdotV = max(dot(worldNorm, viewDir), 0.0);
    //float NdotL = max(dot(worldNorm, lightDir), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
*/
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------

void main()
{
    vec3 albedo = u_AlbedoTexToggle > .5 ? pow(texture(u_AlbedoTexture, TexCoords).rgb, vec3(2.2)) : u_AlbedoColor;
    
    float metallic = u_MetalnessTexToggle > .5 ? texture(u_MetalnessTexture, TexCoords).r : u_Metalness;
    float roughness = u_RoughnessTexToggle > .5 ? texture(u_RoughnessTexture, TexCoords).r : u_Roughness;
    //float ao = texture(aoMap, TexCoords).r;

    vec3 worldNorm = u_NormalTexToggle > .5 ? getNormalFromMap() : normalize(Normal);
    
    vec3 viewDir = normalize(u_CameraPosition - WorldPos);
    //vec3 viewDir = normalize(-WorldPos);

    
    float NdotV = max(dot(worldNorm, viewDir), 0.0);
    
    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    
    //for (int i = 0; i < 4; ++i)
    {
        // calculate per-light radiance

        //vec3 lightDir = normalize(lightPositions[i] - WorldPos);
        vec3 lightDir = -normalize(lights.Direction);

        vec3 halfwayVector = normalize(viewDir + lightDir);

        //float distance = length(lightPositions[i] - WorldPos);
        
        float attenuation = 1.0;// / (distance * distance);
        
        //vec3 radiance = lightColors[i] * attenuation;
        vec3 radiance = lights.Radiance * lights.Multiplier * attenuation;

        
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
        
        //float G = GeometrySmith(worldNorm, viewDir, lightDir, roughness);
        //float G = GeometrySmith(NdotV, NdotL, roughness);
        float G = gaSchlickGGX(NdotV, NdotL, roughness);
        

        vec3 numerator = D * G * F;
        float denominator = 4 * NdotV * NdotL + 0.001;
        
        // prevent divide by zero for NdotV=0.0 or NdotL=0.0
        vec3 specularBRDF = numerator / denominator;// max(denominator, 0.001);

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

        Lo += (kD * albedo / PI + specularBRDF) * radiance * NdotL;  
        //Lo += (kD * albedo + specularBRDF) * radiance * NdotL;
    }

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    //vec3 ambient = vec3(0.03) * albedo;// *ao;
    //vec3 ambient = texture(u_EnvIrradianceTex, worldNormal).rgb;
    
    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(u_EnvIrradianceTex, worldNorm).rgb;
    //vec3 irradiance = texture(u_EnvRadianceTex, worldNorm).rgb;

    
    vec3 diffuse = irradiance * albedo;

    vec3 R = reflect(-viewDir, worldNorm);
    int u_EnvRadianceTexLevels = textureQueryLevels(u_EnvRadianceTex);
    vec3 prefilteredColor = textureLod(u_EnvRadianceTex, R, roughness * u_EnvRadianceTexLevels).rgb;

    vec2 envBRDF = texture(u_BRDFLUTTexture, vec2(NdotV, roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);


    vec3 ambient = (kD * diffuse + specular);// *ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    //color = color / (color + vec3(1.0));
    // gamma correct
    //color = pow(color, vec3(1.0 / 2.2));


    

    FragColor = vec4(color, 1.0);

    //FragColor = vec4(irradiance, 1.0);
    //FragColor = vec4(TexCoords, 0, 1);
}