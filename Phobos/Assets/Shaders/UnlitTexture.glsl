#properties
{
    u_Color | [COLOR : DEF(1,1,1,1)];
    u_Texture;
}
#flags
{
    STANDARD_VARS, SKINNED
}

#type vertex
#version 330 core

out vec2 v_UV;

void main()
{
    gl_Position = ares_MVPMatrix * vec4(ares_ObjectPos, 1.0);
    v_UV = ares_ObjectUVs;
}

#type fragment
#version 330 core

in vec2 v_UV;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

void main()
{
    out_Color = texture(u_Texture, v_UV) * u_Color;
}
