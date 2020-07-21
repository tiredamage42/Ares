#pragma once

namespace Ares
{
	class Time
	{
	public:
		static void Tick(double currentTime)
		{
			m_DeltaTime = currentTime - m_Time;
			m_Time = currentTime;
			m_Frame++;
		}
		const static double GetTime() { return m_Time; }
		const static double GetDeltaTime() { return m_DeltaTime; }
		const static long long unsigned int GetFrame() { return m_Frame; }
		const static uint32_t GetFPS() { return (uint32_t)(1.0f / m_DeltaTime); }
	private:
		static inline double m_Time = 0.0;
		static inline double m_DeltaTime = 0.0;
		static inline long long unsigned int m_Frame = 0;
	};
}