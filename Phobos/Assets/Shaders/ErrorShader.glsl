#FLAGS{ SKINNED }
#type vertex
#version 430
void main()
{
	gl_Position = ares_MVPMatrix * vec4(ares_ObjectPos, 1.0);
}
#type fragment
#version 430
void main()
{
	out_Color = vec4(1,0,1,1);
}