#pragma once
#include "Object.h"

#define DIRECTION_FORWARD			0x0001
#define DIRECTION_BACKWARD			0x0002
#define DIRECTION_LEFTUP_CROSS		0x0003
#define DIRECTION_RIGHTUP_CROSS		0x0004
#define DIRECTION_LEFTDOWN_CROSS	0x0005
#define DIRECTION_RIGHTDOWN_CROSS	0x0006
#define ROTATION_LEFT				0x0010
#define ROTATION_RIGHT				0x0020

// ================================================= CBulletObject =================================================

class CBulletObject : public CObject
{
private:
	bool								m_IsExploded{};

	CSpriteBilboardMesh*				m_ExplosionMesh{};

public:
	CBulletObject() = default;
	virtual ~CBulletObject() = default;

	virtual void Animate(float ElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	bool IsExploded() const;
	void PrepareExplosion();

	void SetExplosionMesh(CSpriteBilboardMesh* ExplosionMesh);
};

// ================================================= CExplodedBulletObject =================================================

class CExplodedBulletObject : public CBilboardObject
{
	friend class CPlayer;

private:
	UINT								m_ObjectCount{};
	CSpriteBilboardMesh*				m_MappedMeshes{};

public:
	CExplodedBulletObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT ObjectCount);
	virtual ~CExplodedBulletObject() = default;

	virtual void Animate(float ElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ================================================= CPlayer =================================================

class CPlayer : public CObject
{
private:
	bool								m_IsExploded{};

	float								m_Pitch{};
	float								m_Yaw{};
	float								m_Roll{};

	CBilboardMesh*						m_HpBarMesh{};
	CSpriteBilboardMesh*				m_ExplosionMesh{};
	CSpriteBilboardMesh*				m_SmokeMesh{};

	shared_ptr<CCamera>					m_Camera{};

	vector<shared_ptr<CBulletObject>>	m_Bullets{};
	UINT								m_BulletIndex{};

	shared_ptr<CExplodedBulletObject>	m_ExplodedBulletObjects{};

	// 특정 프레임 회전
	shared_ptr<CObject>					m_СombatTowerFrame{};
	float								m_CombatTowerAngle{};

public:
	CPlayer(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual ~CPlayer() = default;

	virtual void OnInitialize();

	virtual void Move(const XMFLOAT3& Direction, float Distance, void* TerrainContext);

	virtual void Animate(float ElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	bool IsExploded() const;
	void PrepareExplosion();

	void SetHpBarMesh(CBilboardMesh* HpBarMesh);
	void SetExplosionMesh(CSpriteBilboardMesh* ExplosionMesh);
	void SetSmokeMesh(CSpriteBilboardMesh* SmokeMesh);

	void SetCamera(const shared_ptr<CCamera>& Camera);
	CCamera* GetCamera() const;

	vector<shared_ptr<CBulletObject>>& GetBullets();
	shared_ptr<CExplodedBulletObject> GetExplodedBullets();

	void KeepDistanceToCamera(float ElapsedTime, void* TerrainContext, float DistanceToCamera);

	void Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime);
	void RotateCombatTower(UINT RotationDirection, float Angle);

	void FireBullet();
	void RenderBullets(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	void CheckBulletByEnemyCollision(CEnemyObject* EnemyObject);
	void CheckBulletByTerrainCollision(void* TerrainContext);
};
