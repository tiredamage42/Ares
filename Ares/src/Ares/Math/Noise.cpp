#include "AresPCH.h"
#include "Noise.h"

#include "FastNoise.h"

namespace Ares {

	static FastNoise s_FastNoise;

	// This returns a value between -1 and 1
	float Noise::PerlinNoise(float x, float y)
	{
		s_FastNoise.SetNoiseType(FastNoise::Perlin);
		float result = s_FastNoise.GetNoise(x, y); 
		return result;
	}
	float Noise::PerlinNoise(float x, float y, float z)
	{
		s_FastNoise.SetNoiseType(FastNoise::Perlin);
		float result = s_FastNoise.GetNoise(x, y, z);
		return result;
	}
}