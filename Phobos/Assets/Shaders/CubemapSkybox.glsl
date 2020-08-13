#type vertex
#version 430

layout(location = 0) in vec2 a_Position;
//layout(location = 0) in vec3 a_Position;
//layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_InverseVP;

out vec3 v_Position;

void main()
{
	//vec4 position = vec4(a_Position.xy, 1.0, 1.0);
	vec4 position = vec4(a_Position, 1.0, 1.0); // depth always at 1

	gl_Position = position;

	v_Position = (u_InverseVP * position).xyz;
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

uniform samplerCube u_Texture;
uniform float u_TextureLod;

in vec3 v_Position;

void main()
{


	finalColor = textureLod(u_Texture, v_Position, u_TextureLod);
	//finalColor = vec4(0.0, 1.0, 0.0, 1.0);

	// color correction
	/*finalColor = finalColor / (finalColor + vec3(1.0));
	finalColor = pow(finalColor, vec3(1.0 / 2.2));*/

}