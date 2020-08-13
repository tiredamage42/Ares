#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_UV;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_UV;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
	v_UV = a_UV;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_UV;

uniform sampler2D u_Texture;

void main()
{

	// just until we make blend part of shader...
	color = vec4(texture(u_Texture, v_UV).rgb, 1); 
}
