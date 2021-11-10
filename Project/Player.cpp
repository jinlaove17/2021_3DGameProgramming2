#include "stdafx.h"
#include "Player.h"

// =================================================== CBulletObject ===================================================

void CBulletObject::Animate(float ElapsedTime)
{
	if (m_IsActive)
	{
		if (m_IsExploded && m_ExplosionMesh->GetFrameTime() < 0.0f)
		{
			// 폭발 애니메이션이 모두 출력되었다면 렌더링을 수행하지 않도록 변수를 설정한다.
			m_IsActive = false;
			m_IsExploded = false;
		}
		else if (!m_IsExploded)
		{
			Move(m_MovingDirection, 15.0f * ElapsedTime, nullptr);
			UpdateBoundingBox();
		}
	}
}

void CBulletObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (m_IsActive && !m_IsExploded)
	{
		UpdateTransform(Matrix4x4::Identity());

		if (m_Child)
		{
			m_Child->Render(D3D12GraphicsCommandList, Camera);
		}
	}
}

bool CBulletObject::IsExploded() const
{
	return m_IsExploded;
}

void CBulletObject::PrepareExplosion()
{
	XMFLOAT3 BiasDistance{ Vector3::ScalarProduct(m_MovingDirection, -0.5f, false) };
	XMFLOAT3 Position{ Vector3::Add(GetPosition(), BiasDistance) };

	m_IsExploded = true;
	m_ExplosionMesh->SetPosition(Position);
	m_ExplosionMesh->SetFrameTime(0.0f);
}

void CBulletObject::SetExplosionMesh(CSpriteBilboardMesh* ExplosionMesh)
{
	m_ExplosionMesh = ExplosionMesh;
}

// ================================================= CExplodedBulletObject =================================================

CExplodedBulletObject::CExplodedBulletObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT ObjectCount) :
	m_ObjectCount{ ObjectCount }
{
	m_D3D12VertexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(CSpriteBilboardMesh) * ObjectCount,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	ThrowIfFailed(m_D3D12VertexBuffer->Map(0, nullptr, (void**)&m_MappedMeshes));

	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(CSpriteBilboardMesh);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(CSpriteBilboardMesh) * ObjectCount;

	for (UINT i = 0; i < ObjectCount; ++i)
	{
		CSpriteBilboardMesh* MappedMesh{ m_MappedMeshes + i };
		XMFLOAT2 Size{ 4.0f, 4.0f };

		MappedMesh->SetSize(Size);
		MappedMesh->SetSpriteRow(4);
		MappedMesh->SetSpriteColumn(5);
		MappedMesh->SetFrameTime(-1.0f);
	}
}

void CExplodedBulletObject::Animate(float ElapsedTime)
{
	const float FrameFPS{ 15.0f };

	for (UINT i = 0; i < m_ObjectCount; ++i)
	{
		CSpriteBilboardMesh* MappedMesh{ m_MappedMeshes + i };

		if (MappedMesh->GetFrameTime() >= 0.0f)
		{
			MappedMesh->IncreaseFrameTime(FrameFPS * ElapsedTime);
		}
	}
}

void CExplodedBulletObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (m_Material)
	{
		if (m_Material->m_Texture)
		{
			m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 3, 1);
		}
	}

	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12VertexBufferView };

	D3D12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	D3D12GraphicsCommandList->IASetVertexBuffers(0, 1, VertexBufferViews);

	for (UINT i = 0; i < m_ObjectCount; ++i)
	{
		CSpriteBilboardMesh* MappedMesh{ m_MappedMeshes + i };
		
		if (MappedMesh->GetFrameTime() >= 0.0f)
		{
			D3D12GraphicsCommandList->DrawInstanced(1, 1, i, 0);
		}
	}
}

// =================================================== CPlayer===================================================

CPlayer::CPlayer(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// 총알 객체와 빌보드(폭발한 총알) 객체를 생성한다.
	const UINT BulletCount{ 5 };

	m_ExplodedBulletObjects = make_shared<CExplodedBulletObject>(D3D12Device, D3D12GraphicsCommandList, BulletCount);

	for (UINT i = 0; i < BulletCount; ++i)
	{
		shared_ptr<CBulletObject> BulletObject{ make_shared<CBulletObject>() };
		CSpriteBilboardMesh* ExplosionMesh{ m_ExplodedBulletObjects->m_MappedMeshes + i };

		BulletObject->SetExplosionMesh(ExplosionMesh);
		m_Bullets.push_back(BulletObject);
	}
}

void CPlayer::OnInitialize()
{
	m_СombatTowerFrame = FindFrame("CombatTower");
}

void CPlayer::Move(const XMFLOAT3& Direction, float Distance, void* TerrainContext)
{
	CObject::Move(Direction, Distance, TerrainContext);

	m_Camera->Move(Vector3::ScalarProduct(Direction, Distance, false));
}

void CPlayer::Animate(float ElapsedTime)
{
	if (m_IsExploded)
	{
		const float FrameFPS{ 10.0f };

		m_ExplosionMesh->IncreaseFrameTime(FrameFPS * ElapsedTime);

		if (m_ExplosionMesh->GetFrameTime() < 0.0f)
		{
			// 폭발 애니메이션이 모두 출력되었다면 렌더링을 수행하지 않도록 변수를 설정한다.
			m_IsActive = false;
		}
	}
	else
	{
		XMFLOAT3 Direction{ 0.0f, -1.0f, 0.0f };
		float Gravity{ 20.0f * ElapsedTime };

		Move(Direction, Gravity, nullptr);

		// 객체가 움직일 때, 체력바도 같이 머리 위에서 움직이도록 설정한다.
		if (m_HpBarMesh)
		{
			XMFLOAT3 Position{ GetPosition() };
			float Height{ m_BoundingBox.Extents.y };

			Position.y += (Height + 2.5f);
			m_HpBarMesh->SetPosition(Position);
		}
	}

	// 빌보드 객체(폭발한 총체)의 애니메이션
	if (m_ExplodedBulletObjects)
	{
		m_ExplodedBulletObjects->Animate(ElapsedTime);
	}

	// 총알 객체의 애니메이션
	for (UINT i = 0; i < m_Bullets.size(); ++i)
	{
		m_Bullets[i]->Animate(ElapsedTime);
	}
}

void CPlayer::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (m_IsActive && !m_IsExploded)
	{
		UpdateTransform(Matrix4x4::Identity());

		if (m_Material)
		{
			if (m_Material->m_Texture)
			{
				m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 3, 0);
			}
		}

		if (m_Child)
		{
			m_Child->Render(D3D12GraphicsCommandList, Camera);
		}
	}
}

bool CPlayer::IsExploded() const
{
	return m_IsExploded;
}

void CPlayer::PrepareExplosion()
{
	XMFLOAT2 HpBar{ m_HpBarMesh->GetSize() };

	HpBar.x -= 0.5f;

	if (HpBar.x <= 0.0f)
	{
		HpBar.x = 0.0f;
		m_IsExploded = true;
		m_ExplosionMesh->SetPosition(GetPosition());
		m_ExplosionMesh->SetFrameTime(0.0f);

		CSound::GetInstance()->Play(CSound::TANK_EXPLOSION_SOUND, 0.7f);
	}

	m_HpBarMesh->SetSize(HpBar);
}

void CPlayer::SetHpBarMesh(CBilboardMesh* HpBarMesh)
{
	m_HpBarMesh = HpBarMesh;
}

void CPlayer::SetExplosionMesh(CSpriteBilboardMesh* ExplosionMesh)
{
	m_ExplosionMesh = ExplosionMesh;
}

void CPlayer::SetSmokeMesh(CBilboardMesh* SmokeMesh)
{
	m_SmokeMesh = SmokeMesh;
}

void CPlayer::SetCamera(const shared_ptr<CCamera>& Camera)
{
	m_Camera = Camera;
}

CCamera* CPlayer::GetCamera() const
{
	return m_Camera.get();
}

vector<shared_ptr<CBulletObject>>& CPlayer::GetBullets()
{
	return m_Bullets;
}

shared_ptr<CExplodedBulletObject> CPlayer::GetExplodedBullets()
{
	return m_ExplodedBulletObjects;
}

void CPlayer::KeepDistanceToCamera(float ElapsedTime, void* TerrainContext, float DistanceToCamera)
{
	CTerrainObject* TerrainObject{ (CTerrainObject*)TerrainContext };
	XMFLOAT3 Scale{ TerrainObject->GetScale() };
	XMFLOAT3 CameraPosition{ m_Camera->GetPosition() };

	int ZPos{ (int)(CameraPosition.z / Scale.z) };
	float Height{ TerrainObject->GetHeight(CameraPosition.x, CameraPosition.z) + DistanceToCamera };

	if (CameraPosition.y <= Height)
	{
		CameraPosition.y = Height;
		
		m_Camera->SetPosition(CameraPosition);
	}
}

void CPlayer::Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime)
{
	if (Pitch)
	{
		m_Pitch += Pitch;

		if (m_Pitch > 5.0f)
		{
			Pitch -= (m_Pitch - 5.0f);
			m_Pitch = 5.0f;
		}

		if (m_Pitch < -5.0f)
		{
			Pitch -= (m_Pitch + 5.0f);
			m_Pitch = -5.0f;
		}
	}

	if (Yaw)
	{
		m_Yaw += Yaw;

		if (m_Yaw > 360.0f)
		{
			m_Yaw -= 360.0f;
		}

		if (m_Yaw < 0.0f)
		{
			m_Yaw += 360.0f;
		}
	}

	if (m_Camera->GetMode() == CCamera::FIRST_PERSON)
	{
		m_Camera->Rotate(Pitch, Yaw, Roll, GetRight(), GetUp(), GetLook(), GetPosition());
	}
	else if (m_Camera->GetMode() == CCamera::THIRD_PERSON)
	{
		m_Camera->Rotate(GetRight(), GetUp(), GetLook(), GetPosition(), ElapsedTime);
	}

	if (Pitch)
	{
		XMFLOAT4X4 RotationMatrix = Matrix4x4::RotationAxis(GetRight(), Pitch);

		SetUp(Vector3::TransformNormal(GetUp(), RotationMatrix));
		SetLook(Vector3::TransformNormal(GetLook(), RotationMatrix));
	}

	if (Yaw)
	{
		XMFLOAT4X4 RotationMatrix = Matrix4x4::RotationAxis(GetUp(), Yaw);

		SetRight(Vector3::TransformNormal(GetRight(), RotationMatrix));
		SetLook(Vector3::TransformNormal(GetLook(), RotationMatrix));
	}

	if (Roll)
	{
		XMFLOAT4X4 RotationMatrix = Matrix4x4::RotationAxis(GetLook(), Roll);

		SetRight(Vector3::TransformNormal(GetRight(), RotationMatrix));
		SetUp(Vector3::TransformNormal(GetUp(), RotationMatrix));
	}

	XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	SetLook(Vector3::Normalize(GetLook()));
	SetRight(Vector3::CrossProduct(WorldUp, GetLook(), false));
	SetUp(Vector3::CrossProduct(GetLook(), GetRight(), false));
}

void CPlayer::RotateCombatTower(UINT RotationDirection, float Angle)
{
	switch (RotationDirection)
	{
	case ROTATION_LEFT:
		m_CombatTowerAngle += Angle;

		if (m_CombatTowerAngle < -20.0f)
		{
			m_CombatTowerAngle = -20.0f;
			return;
		}
		break;
	case ROTATION_RIGHT:
		m_CombatTowerAngle += Angle;

		if (m_CombatTowerAngle > +20.0f)
		{
			m_CombatTowerAngle = +20.0f;
			return;
		}
		break;
	}

	XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(GetUp(), Angle) };

	m_СombatTowerFrame->m_TransformMatrix = Matrix4x4::Multiply(RotationMatrix, m_СombatTowerFrame->m_TransformMatrix);

	UpdateTransform(Matrix4x4::Identity());
	CSound::GetInstance()->Play(CSound::TANK_TURRET_ROTATION_SOUND, 0.2f);
}

void CPlayer::FireBullet()
{
	if (m_IsActive && !m_IsExploded)
	{
		if (m_СombatTowerFrame)
		{
			XMFLOAT3 CombatTowerPosition{ m_СombatTowerFrame->GetPosition() };
			XMFLOAT3 Look{ Vector3::TransformNormal(m_СombatTowerFrame->GetLook(), Matrix4x4::RotationAxis(GetUp(), -90.0f)) };
			XMFLOAT3 Position{ Vector3::Add(CombatTowerPosition, Vector3::ScalarProduct(Look, 2.0f, false)) };

			if (!m_Bullets[m_BulletIndex]->IsExploded())
			{
				m_Bullets[m_BulletIndex]->SetActive(true);
				m_Bullets[m_BulletIndex]->SetMovingDirection(Look);
				m_Bullets[m_BulletIndex]->SetPosition(Position);

				for (int i = 0; i < 100; ++i)
				{
					CBilboardMesh* MappedMesh{ m_SmokeMesh + i };

					Position.x += GetRandomNumber(1.0f, 3.0f) * cosf((float)i);
					Position.y += GetRandomNumber(0.02f, 0.05f);
					Position.z += GetRandomNumber(1.0f, 3.0f) * sinf((float)i);

					MappedMesh->SetPosition(Position);
				}

				CSound::GetInstance()->Play(CSound::TANK_ATTACK_SOUND, 0.6f);
			}

			m_BulletIndex = (m_BulletIndex + 1) % (UINT)m_Bullets.size();
		}
	}
}

void CPlayer::RenderBullets(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	for (UINT i = 0; i < m_Bullets.size(); ++i)
	{
		m_Bullets[i]->Render(D3D12GraphicsCommandList, Camera);
	}
}

void CPlayer::CheckBulletByEnemyCollision(CEnemyObject* EnemyObject)
{
	for (UINT i = 0; i < m_Bullets.size(); ++i)
	{
		if (m_Bullets[i]->IsActive() && !m_Bullets[i]->IsExploded())
		{
			if (m_Bullets[i]->GetBoundingBox().Intersects(EnemyObject->GetBoundingBox()))
			{
				if (!EnemyObject->IsExploded())
				{
					EnemyObject->PrepareExplosion();
					m_Bullets[i]->PrepareExplosion();
					CSound::GetInstance()->Play(CSound::BULLET_EXPLOSION_SOUND, 0.35f);
				}
			}
		}
	}
}

void CPlayer::CheckBulletByTerrainCollision(void* TerrainContext)
{
	CTerrainObject* TerrainObject{ (CTerrainObject*)TerrainContext };

	for (UINT i = 0; i < m_Bullets.size(); ++i)
	{
		if (m_Bullets[i]->IsActive() && !m_Bullets[i]->IsExploded())
		{
			XMFLOAT3 BulletPosition{ m_Bullets[i]->GetPosition() };
			float TerrainHeight{ TerrainObject->GetHeight(BulletPosition.x, BulletPosition.z) };

			if (BulletPosition.y <= TerrainHeight)
			{
				m_Bullets[i]->PrepareExplosion();
				CSound::GetInstance()->Play(CSound::BULLET_EXPLOSION_SOUND, 0.35f);
			}
		}
	}
}
