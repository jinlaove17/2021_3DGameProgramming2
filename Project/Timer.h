#pragma once

const ULONG MAX_SAMPLE_COUNT = 60;

class CTimer
{
private:
	bool				m_Stopped{};

	LARGE_INTEGER		m_PerformanceFrequency{};
	LARGE_INTEGER		m_BasePerformanceCounter{};
	LARGE_INTEGER		m_LastPerformanceCounter{};
	LARGE_INTEGER		m_CurrentPerformanceCounter{};

	float				m_ElapsedTime{};
	float				m_FPSElapsedTime{};

	float				m_FrameTime[MAX_SAMPLE_COUNT]{};
	UINT				m_SampleCount{};

	UINT				m_FrameRate{};
	UINT				m_FrameCount{};

public:
	CTimer();
	~CTimer() = default;

	void Start();
	void Stop();
	void Reset();
	void Tick(float LockFPS);

	float GetElapsedTime() const;
	UINT GetFrameRate(LPTSTR Title, UINT Characters);
};
