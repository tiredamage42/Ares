#type vertex
#version 430

layout(location = 0) in vec2 a_Position;

uniform mat4 u_InverseVP;

out vec3 Texcoord;

void main()
{
	vec4 position = vec4(a_Position, 1.0, 1.0); // depth always at 1
	gl_Position = position;
	Texcoord = (u_InverseVP * position).xyz;
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

uniform samplerCube u_Texture;

in vec3 Texcoord;

void main()
{
	finalColor = textureLod(u_Texture, Texcoord, 0);
}