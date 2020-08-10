#pragma once

#include <random>
namespace Ares
{
	class Random
	{
	public:
		/*
		* Returns a Random Real between min and max
		*/
		template<typename T>
		static T Real(T min, T max)
		{
			ARES_CORE_ASSERT(min < max, "min is >= max");
			std::uniform_real_distribution<T> dist(min, max);
			auto& mt = GetEngine();
			return dist(mt);
		}

		/*
		* Returns a Random Integer between min and max
		*/
		template<typename T>
		static T Int(T min, T max)
		{
			ARES_CORE_ASSERT(min < max, "min is >= max");
			std::uniform_int_distribution<T> dist(min, max);
			auto& mt = GetEngine();
			return dist(mt);
		}

		/*
		* Returns a Random bool value, either 'true' or 'false'
		*/
		static bool Bool(double trueChance = 0.5)
		{
			//fifty fifty chance
			std::bernoulli_distribution dist(trueChance);
			auto& mt = GetEngine();
			return dist(mt);
		}

	private:
		static std::mt19937& GetEngine()
		{
			static std::random_device seed_gen;
			static std::mt19937 engine(seed_gen());
			return engine;
		}

	};

}