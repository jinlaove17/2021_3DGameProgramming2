#include "stdafx.h"
#include "Sound.h"

CSound* CSound::m_Instance;

CSound::CSound()
{

}

CSound::~CSound()
{
	if (m_Instance)
	{
		m_System->release();

		delete m_Instance;
	}
}

CSound* CSound::GetInstance()
{
	if (!m_Instance)
	{
		m_Instance = new CSound{};
	}

	return m_Instance;
}

void CSound::Init()
{
	m_Result = System_Create(&m_System);

	if (m_Result != FMOD_OK)
	{
		return;
	}

	m_Result = m_System->init(MAX_SOUND, FMOD_INIT_NORMAL, nullptr);

	if (m_Result != FMOD_OK)
	{
		return;
	}

	// 추가할 사운드 파일
	m_Result = m_System->createSound("Sound/Title_BGM.mp3", FMOD_LOOP_NORMAL, 0, &m_Sound[TITLE_BACKGROUND_SOUND]);
	m_Result = m_System->createSound("Sound/Game_BGM.mp3", FMOD_LOOP_NORMAL, 0, &m_Sound[GAME_BACKGROUND_SOUND]);
	m_Result = m_System->createSound("Sound/Button.wav", FMOD_LOOP_OFF, 0, &m_Sound[BUTTON_SOUND]);
	m_Result = m_System->createSound("Sound/Tank_Attack.wav", FMOD_LOOP_OFF, 0, &m_Sound[TANK_ATTACK_SOUND]);
	m_Result = m_System->createSound("Sound/Tank_Turret Rotation.wav", FMOD_LOOP_OFF, 0, &m_Sound[TANK_TURRET_ROTATION_SOUND]);
	m_Result = m_System->createSound("Sound/Tank_Collision.wav", FMOD_LOOP_OFF, 0, &m_Sound[TANK_COLLISION_SOUND]); 
	m_Result = m_System->createSound("Sound/Tank_Explosion.wav", FMOD_LOOP_OFF, 0, &m_Sound[TANK_EXPLOSION_SOUND]);
	m_Result = m_System->createSound("Sound/Bullet_Explosion.wav", FMOD_LOOP_OFF, 0, &m_Sound[BULLET_EXPLOSION_SOUND]);

	if (m_Result != FMOD_OK)
	{
		return;
	}
}

void CSound::Play(int Index, float Volume)
{
	m_System->update();
	m_Result = m_System->playSound(FMOD_CHANNEL_FREE, m_Sound[Index], false, &m_Channel[Index]);
	m_Channel[Index]->setVolume(Volume);
}

void CSound::Stop(int Index)
{
	m_Channel[Index]->stop();
}

void CSound::Pause(int Index)
{
	m_Channel[Index]->setPaused(true);
}

void CSound::Resume(int Index)
{
	m_Channel[Index]->setPaused(false);
}
