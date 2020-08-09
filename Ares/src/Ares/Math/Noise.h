#pragma once

namespace Ares {

	class Noise
	{
	public:
		static float PerlinNoise(float x, float y);
		static float PerlinNoise(float x, float y, float z);
	};

}