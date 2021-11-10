#pragma once
#define MAX_SOUND 8

class CSound
{
public:
	enum
	{
		TITLE_BACKGROUND_SOUND,
		GAME_BACKGROUND_SOUND,

		BUTTON_SOUND,
		TANK_ATTACK_SOUND,
		TANK_TURRET_ROTATION_SOUND,
		TANK_COLLISION_SOUND,
		TANK_EXPLOSION_SOUND,
		BULLET_EXPLOSION_SOUND
	};

private:
	static CSound*		m_Instance;

	System*				m_System{};
	Channel*			m_Channel[MAX_SOUND]{};
	Sound*				m_Sound[MAX_SOUND]{};
	FMOD_RESULT			m_Result{};

private:
	CSound();
	~CSound();

public:
	static CSound* GetInstance();

	void Init();
	void Play(int Index, float Volume);
	void Stop(int Index);
	void Pause(int Index);
	void Resume(int nIndex);
};
