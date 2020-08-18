
#properties
{
    u_Color | [COLOR : DEF(1, .5, 0, 1)];
}

#flags { STANDARD_VARS, SKINNED }

#type vertex
#version 430

void main()
{
	gl_Position = ares_MVPMatrix * vec4(ares_ObjectPos, 1.0);
}
	
#type fragment
#version 430

uniform vec4 u_Color;

void main()
{
	out_Color = u_Color;
}