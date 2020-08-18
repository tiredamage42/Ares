#flags { STANDARD_VARS }

#shared
{
	#define GRID_RESOLUTION 1000.0
	#define GRID_WIDTH .05
	varying vec2 Texcoord;
}

#type vertex
#version 430

void main()
{
	Texcoord = ares_ObjectPos.xy + .5;
	gl_Position = ares_MVPMatrix * vec4(vec3(ares_ObjectPos.xy * (GRID_RESOLUTION + GRID_WIDTH), 0), 1.0);
}

#type fragment
#version 430

float grid(vec2 st)
{
	vec2 grid = fract(st);
	return step(GRID_WIDTH, grid.x) * step(GRID_WIDTH, grid.y);
}

void main()
{
	float x = grid(Texcoord * (GRID_RESOLUTION + GRID_WIDTH));
	out_Color = vec4(vec3(0.2), 0.5) * (1.0 - x);
}