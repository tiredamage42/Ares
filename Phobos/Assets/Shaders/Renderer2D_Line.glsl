#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_CamRange;
uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_CamRange;
out vec3 v_WorldPos;

void main()
{
	v_Color = a_Color;
	v_CamRange = a_CamRange;
	v_WorldPos = a_Position;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 430 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_CamRange;
in vec3 v_WorldPos;
uniform vec3 ares_CamPosition;

void main()
{
	color = v_Color;

	float dist = distance(ares_CamPosition, v_WorldPos);
	// fade out based on camera distance
	float alphaMultiplier = min(max((dist - v_CamRange.x) / v_CamRange.y, 0), 1);

	color.a *= 1.0 - alphaMultiplier;

	//color = vec4(1, 0, 1, 1);
}