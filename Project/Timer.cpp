#include "stdafx.h"
#include "Timer.h"

CTimer::CTimer()
{
	QueryPerformanceFrequency(&m_PerformanceFrequency);
	QueryPerformanceCounter(&m_BasePerformanceCounter);
	m_LastPerformanceCounter = m_BasePerformanceCounter;
}

void CTimer::Start()
{

}

void CTimer::Stop()
{

}

void CTimer::Reset()
{
	LARGE_INTEGER PerformanceCounter{};

	::QueryPerformanceCounter(&PerformanceCounter);
	
	m_Stopped = false;
	m_BasePerformanceCounter = PerformanceCounter;
	m_LastPerformanceCounter = PerformanceCounter;
}

void CTimer::Tick(float LockFPS)
{
	if (m_Stopped)
	{
		m_ElapsedTime = 0.0f;
		return;
	}

	QueryPerformanceCounter(&m_CurrentPerformanceCounter);

	float ElapsedTime{ (float)(m_CurrentPerformanceCounter.QuadPart - m_LastPerformanceCounter.QuadPart) / m_PerformanceFrequency.QuadPart };

	if (LockFPS > 0.0f)
	{
		while (ElapsedTime < 1.0f / LockFPS)
		{
			QueryPerformanceCounter(&m_CurrentPerformanceCounter);
			ElapsedTime = (float)(m_CurrentPerformanceCounter.QuadPart - m_LastPerformanceCounter.QuadPart) / m_PerformanceFrequency.QuadPart;
		}
	}

	m_LastPerformanceCounter = m_CurrentPerformanceCounter;

	if (fabsf(ElapsedTime - m_ElapsedTime) < 1.0f)
	{
		memmove(&m_FrameTime[1], m_FrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_FrameTime[0] = ElapsedTime;

		if (m_SampleCount < MAX_SAMPLE_COUNT)
		{
			++m_SampleCount;
		}
	}

	++m_FrameCount;
	m_FPSElapsedTime += ElapsedTime;

	if (m_FPSElapsedTime > 1.0f)
	{
		m_FrameRate = m_FrameCount;
		m_FrameCount = 0;
		m_FPSElapsedTime = 0.0f;
	}

	m_ElapsedTime = 0.0f;

	if (m_SampleCount > 0)
	{
		for (UINT i = 0; i < m_SampleCount; ++i)
		{
			m_ElapsedTime += m_FrameTime[i];
		}

		m_ElapsedTime /= m_SampleCount;
	}
}

float CTimer::GetElapsedTime() const
{
	return m_ElapsedTime;
}

UINT CTimer::GetFrameRate(LPTSTR Title, UINT Characters)
{
	if (Title)
	{
		_itow_s(m_FrameRate, Title, Characters, 10);
		wcscat_s(Title, Characters, _T(" FPS)"));
	}

	return m_FrameRate;
}
