#include "stdafx.h"
#include "Scene.h"
#include "Shader.h"

queue<unique_ptr<CScene>> CScene::m_Scenes;

// =============================================== CTitleScene ===============================================

void CTitleScene::OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	CreateRootSignature(D3D12Device);
	BuildObjects(D3D12Device, D3D12GraphicsCommandList);
}

void CTitleScene::OnDestroy()
{

}

void CTitleScene::BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// ��� ��ü�� �����Ѵ�.
	m_Background = make_shared<CBackgroundObject>(D3D12Device, D3D12GraphicsCommandList);

	// ��ư ��ü�� �����Ѵ�.
	m_Buttons = make_shared<CButtonObject>(D3D12Device, D3D12GraphicsCommandList, BUTTON_COUNT);

	vector<shared_ptr<CBilboardObject>> Objects{};

	// Ÿ��Ʋ ȭ�鿡�� ���� ��ư 2���� �ִ�.
	Objects.reserve(BUTTON_COUNT + 1);
	Objects.push_back(m_Background);
	Objects.push_back(m_Buttons);

	// ���̴��� �����Ѵ�.
	m_Shader = make_shared<CTitleShader>(Objects);
	m_Shader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
}

void CTitleScene::ReleaseObjects()
{

}

void CTitleScene::CreateRootSignature(ID3D12Device* D3D12Device)
{
	D3D12_DESCRIPTOR_RANGE D3D12DescriptorRanges[1]{};

	D3D12DescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	D3D12DescriptorRanges[0].NumDescriptors = 1;
	D3D12DescriptorRanges[0].BaseShaderRegister = 0; // �ؽ�ó(t0)
	D3D12DescriptorRanges[0].RegisterSpace = 0;
	D3D12DescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	CD3DX12_ROOT_PARAMETER D3D12RootParameters[1]{};

	D3D12RootParameters[0].InitAsDescriptorTable(1, &D3D12DescriptorRanges[0]); // �ؽ�ó ����

	D3D12_ROOT_SIGNATURE_FLAGS D3D12RootSignatureFlags{ D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT };	// IA�ܰ踦 ���

	CD3DX12_STATIC_SAMPLER_DESC D3D12SamplerDesc{
		0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.0f, 1, D3D12_COMPARISON_FUNC_ALWAYS, D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK, 0.0f, D3D12_FLOAT32_MAX, D3D12_SHADER_VISIBILITY_PIXEL, 0 };
	CD3DX12_ROOT_SIGNATURE_DESC D3D12RootSignatureDesc{};

	D3D12RootSignatureDesc.Init(_countof(D3D12RootParameters), D3D12RootParameters, 1, &D3D12SamplerDesc, D3D12RootSignatureFlags);

	ID3DBlob* D3D12SignatureBlob{}, * D3D12ErrorBlob{};

	ThrowIfFailed(D3D12SerializeRootSignature(&D3D12RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &D3D12SignatureBlob, &D3D12ErrorBlob));
	ThrowIfFailed(D3D12Device->CreateRootSignature(0, D3D12SignatureBlob->GetBufferPointer(), D3D12SignatureBlob->GetBufferSize(),
		__uuidof(ID3D12RootSignature), (void**)m_D3D12RootSignature.GetAddressOf()));
}

void CTitleScene::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CTitleScene::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CTitleScene::ReleaseShaderVariables()
{

}

void CTitleScene::ReleaseUploadBuffers()
{
	// Release() ������ ���� ������ �𸣰���
	//if (m_Background)
	//{
	//	m_Background->ReleaseUploadBuffers();
	//}
}

void CTitleScene::ProcessInput(HWND hWnd, float ElapsedTime)
{

}

void CTitleScene::ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	int ScreenX{ LOWORD(lParam) };
	int ScreenY{ HIWORD(lParam) };
	int IsOnButton{ m_Buttons->ActiveButton(ScreenX, ScreenY) };

	switch (Message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		if (IsOnButton == CButtonObject::GAME_START)
		{
			RECT Rect{};

			GetWindowRect(hWnd, &Rect);

			POINT OldCursorPos{ Rect.left + FRAME_BUFFER_WIDTH / 2, Rect.top + FRAME_BUFFER_HEIGHT / 2 };

			SetCursorPos(OldCursorPos.x, OldCursorPos.y);
			ShowCursor(FALSE);

			IsCursorActive = false;

			CSound::GetInstance()->Stop(CSound::TITLE_BACKGROUND_SOUND);
			CSound::GetInstance()->Play(CSound::GAME_BACKGROUND_SOUND, 0.7f);
			CScene::m_Scenes.pop();
		}
		else if (IsOnButton == CButtonObject::EXIT)
		{
			PostQuitMessage(0);
		}
		break;
	}
}

void CTitleScene::ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CTitleScene::Animate(float ElapsedTime)
{

}

void CTitleScene::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12GraphicsCommandList->SetGraphicsRootSignature(m_D3D12RootSignature.Get());

	if (m_Shader)
	{
		m_Shader->Render(D3D12GraphicsCommandList, nullptr);
	}
}

// ================================================= CGameScene =================================================

void CGameScene::OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	CreateRootSignature(D3D12Device);
	BuildObjects(D3D12Device, D3D12GraphicsCommandList);
}

void CGameScene::OnDestroy()
{

}

void CGameScene::BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// ī�޶� ��ü�� �����Ѵ�.
	shared_ptr<CCamera> Camera{ make_shared<CCamera>() };

	Camera->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	Camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
	Camera->GenerateProjectionMatrix(90.0f, (float)FRAME_BUFFER_WIDTH / (float)FRAME_BUFFER_HEIGHT, 1.0f, 1000.0f);

	// �÷��̾� ��ü�� �����Ѵ�.
	m_Player = make_shared<CPlayer>(D3D12Device, D3D12GraphicsCommandList);
	m_Player->SetActive(true);
	m_Player->SetCamera(Camera);
	m_Player->SetPosition(XMFLOAT3(20.0f, 0.0f, 10.0f));

	// ��ī�̹ڽ� ��ü�� �����Ѵ�.
	m_SkyBox = make_shared<CSkyBoxObject>(D3D12Device, D3D12GraphicsCommandList);

	// ���� ��ü�� �����Ѵ�.
#ifdef TERRAIN_TESSELLATION
	m_Terrain = make_shared<CTerrainObject>(D3D12Device, D3D12GraphicsCommandList, L"Image/HeightMap.raw", 257, 257, 13, 13, XMFLOAT3(1.0f, 0.6f, 1.0f));
#else
	m_Terrain = make_shared<CTerrainObject>(D3D12Device, D3D12GraphicsCommandList, L"Image/HeightMap.raw", 257, 257, 257, 257, XMFLOAT3(1.0f, 0.6f, 1.0f));
#endif

	// ������ ��ü(����)�� �����Ѵ�.
	m_Trees = make_shared<CTreeObject>(D3D12Device, D3D12GraphicsCommandList, (void*)m_Terrain.get());

	// ������ ��ü(ü�¹�, ����)�� �÷��̾�+�� ��ü�� ����ŭ �����Ѵ�.
	m_HpBars = make_shared<CHpBarObject>(D3D12Device, D3D12GraphicsCommandList, ENEMY_COUNT + 1);
	m_ExplodedEnemies = make_shared<CExplodedEnemyObject>(D3D12Device, D3D12GraphicsCommandList, ENEMY_COUNT + 1);

	// ������ ��ü(����)�� �����Ѵ�.
	m_Smoke = make_shared<CSmokeObject>(D3D12Device, D3D12GraphicsCommandList, SMOKE_COUNT);

	// ü�¹� ��ü�� ���� ��ü�κ��� ���ҽ��� ���ε� �ּҸ� ������ �� �÷��̾�� �� ��ü�� �������ش�. 
	CBilboardMesh* HpBarMesh{ m_HpBars->GetMappedMesh() };
	CSpriteBilboardMesh* ExplosionMesh{ m_ExplodedEnemies->GetMappedMesh() };

	m_Player->SetHpBarMesh(HpBarMesh++);
	m_Player->SetExplosionMesh(ExplosionMesh++);
	m_Player->SetSmokeMesh(m_Smoke->GetMappedMesh());

	for (UINT i = 0; i < ENEMY_COUNT; ++i)
	{
		// �� ��ü�� �����Ѵ�.
		shared_ptr<CEnemyObject> EnemyObject{ make_shared<CEnemyObject>() };

		EnemyObject->OnInitialize();
		EnemyObject->SetActive(true);
		EnemyObject->SetHpBarMesh(HpBarMesh++);
		EnemyObject->SetExplosionMesh(ExplosionMesh++);
		EnemyObject->Rotate(EnemyObject->GetUp(), 180.0f);
		EnemyObject->SetPosition(XMFLOAT3(20.0f + 30.0f * i, 0.0f, m_Terrain->GetLength() - 40.0f - 30.0f * i));

		m_Objects.push_back(EnemyObject);
	}

	// �ǳ��� �ִ� ��ü(��)�� �����Ѵ�.
	shared_ptr<CObject> InsideObject{};
	XMFLOAT3 Axis{ 0.0f, 1.0f, 0.0f };

	// ���� �޸���, �ſ��� �޸��� �Ǿ� �ϹǷ� PSO ������ �޶����� ������ CMirrorShader���� CRectMesh�� �����Ѵ�
	for (int i = 0; i < WALL_COUNT; ++i)
	{
		InsideObject = make_shared<CWallObject>();
		InsideObject->SetActive(true);

		switch (i)
		{
		case CWallObject::LOC_FRONT:
			InsideObject->Scale(22.0f, 20.0f, 1.0f);
			InsideObject->SetPosition(Vector3::Add(WALL_CENTER, XMFLOAT3(0.0f, 0.0f, -WALL_HALF_LENGTH)));
			break;
		case CWallObject::LOC_LEFT:
			InsideObject->Rotate(Axis, 90.0f);
			InsideObject->Scale(20.0f, 20.0f, 1.0f);
			InsideObject->SetPosition(Vector3::Add(WALL_CENTER, XMFLOAT3(-WALL_HALF_LENGTH, 0.0f, 0.0f)));
			break;
		case CWallObject::LOC_RIGHT:
			InsideObject->Rotate(Axis, -90.0f);
			InsideObject->Scale(20.0f, 20.0f, 1.0f);
			InsideObject->SetPosition(Vector3::Add(WALL_CENTER, XMFLOAT3(WALL_HALF_LENGTH, 0.0f, 0.0f)));
			break;
		case CWallObject::LOC_TOP:
			Axis = { 1.0f, 0.0f, 0.0f };

			InsideObject->Rotate(Axis, 90.0f);
			InsideObject->Scale(22.0f, 20.0f, 1.0f);
			InsideObject->SetPosition(Vector3::Add(WALL_CENTER, XMFLOAT3(0.0f, WALL_HALF_LENGTH, 0.0f)));
			break;
		case CWallObject::LOC_BOTTOM:
			InsideObject->Rotate(Axis, 90.0f);
			InsideObject->Scale(22.0f, 20.0f, 1.0f);
			InsideObject->SetPosition(Vector3::Add(WALL_CENTER, XMFLOAT3(0.0f, -WALL_HALF_LENGTH, 0.0f)));
			break;
		}

		m_InsideObjects.push_back(InsideObject);
	}

	// �ǳ��� �ִ� ��ü(����)�� �����Ѵ�.
	Axis = { 0.0f, 1.0f, 0.0f };

	for (int i = 0; i < BOX_COUNT; ++i)
	{
		InsideObject = make_shared<CBoxObject>();
		InsideObject->SetActive(true);
		InsideObject->Scale(2.0f, 2.0f, 2.0f);

		switch (i)
		{
		case 0:
			InsideObject->SetPosition(XMFLOAT3(-35.0f, 3.0f, 24.0f));
			break;
		case 1:
			InsideObject->Rotate(Axis, 15.0f);
			InsideObject->SetPosition(XMFLOAT3(-30.0f, 3.0f, 20.0f));
			break;
		case 2:
			InsideObject->Rotate(Axis, 60.0f);
			InsideObject->SetPosition(XMFLOAT3(-32.5f, 7.0f, 23.0f));
			break;
		}

		m_InsideObjects.push_back(InsideObject);
	}

	// ��ī�̹ڽ� ��ü�� ���̴��� �����ϰ� ���̴� ���Ϳ� �߰��Ѵ�.
	shared_ptr<CShader> SkyBoxShader{ make_shared<CSkyBoxShader>(m_SkyBox) };

	SkyBoxShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	SkyBoxShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(SkyBoxShader);

	// ���� ��ü�� ���̴��� �����ϰ� ���̴� ���Ϳ� �߰��Ѵ�.
#ifdef TERRAIN_TESSELLATION
	shared_ptr<CShader> TerrainShader{ make_shared<CTessellationTerrainShader>(m_Terrain) };
#else
	shared_ptr<CShader> TerrainShader{ make_shared<CTerrainShader>(m_Terrain) };
#endif
	TerrainShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	TerrainShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(TerrainShader);

	// �� ��ü�� ���̴��� �����ϰ� ���̴� ���Ϳ� �߰��Ѵ�.
	shared_ptr<CShader> ObjectShader{ make_shared<CObjectShader>(m_Objects) };

	ObjectShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	ObjectShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(ObjectShader);

	// ü�¹� ��ü�� ���̴��� �����ϰ� ���̴� ���Ϳ� �߰��Ѵ�.
	shared_ptr<CShader> HpBarShader{ make_shared<CHpBarShader>(m_HpBars) };

	HpBarShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	HpBarShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(HpBarShader);

	// ������ ��ü���� ��� ���͸� �����Ѵ�.
	vector<shared_ptr<CBilboardObject>> BilboardObjects{};

	BilboardObjects.push_back(m_Trees);

	// ������ ��ü�� ���̴��� �����Ѵ�.
	shared_ptr<CShader> BilboardShader{ make_shared<CBilboardShader>(BilboardObjects) };

	BilboardShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	BilboardShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(BilboardShader);

	// ���͸� �ʱ�ȭ �� ���� ��������Ʈ ������ ��ü���� �߰��Ѵ�.
	BilboardObjects.clear();
	BilboardObjects.reserve(3);
	BilboardObjects.push_back(m_ExplodedEnemies);
	BilboardObjects.push_back(m_Player->GetExplodedBullets());
	BilboardObjects.push_back(m_Smoke);

	// ��������Ʈ ������ ��ü�� ���̴��� �����Ѵ�.
	shared_ptr<CShader> SpriteBilboardShader{ make_shared<CSpriteBilboardShader>(BilboardObjects) };

	SpriteBilboardShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	SpriteBilboardShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(SpriteBilboardShader);

	// (�ſ� �ݻ縦 ���� CMirrorShader�� ����Ѵ�.)
	// �÷��̾� ���̴��� �����ϰ� ���̴� ���Ϳ� �߰��Ѵ�.
	//shared_ptr<CShader> PlayerShader{ make_shared<CPlayerShader>(m_Player) };

	//PlayerShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	//PlayerShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	//m_Shaders.push_back(PlayerShader);

	// �÷��̾� �Ѿ� ��ü�� ���̴��� �����ϰ� ���̴� ���Ϳ� �߰��Ѵ�.
	shared_ptr<CShader> BulletShader{ make_shared<CBulletShader>(m_Player->GetBullets()) };

	BulletShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	BulletShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(BulletShader);

	shared_ptr<CShader> ParticleShader{ make_shared<CParticleShader>() };

	ParticleShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	ParticleShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(ParticleShader);

	// �ſ� ��ü�� ���̴��� �����Ѵ�.
	shared_ptr<CShader> MirrorShader{ make_shared<CMirrorShader>(m_Player, m_InsideObjects) };

	MirrorShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	MirrorShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(MirrorShader);

	// ���� �� ������� ���ҽ��� �����Ѵ�.
	BuildLights();
	CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
}

void CGameScene::ReleaseObjects()
{

}

void CGameScene::CreateRootSignature(ID3D12Device* D3D12Device)
{
	D3D12_DESCRIPTOR_RANGE D3D12DescriptorRanges[2]{};

	D3D12DescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	D3D12DescriptorRanges[0].NumDescriptors = 1;
	D3D12DescriptorRanges[0].BaseShaderRegister = 0; // �ؽ�ó(t0)
	D3D12DescriptorRanges[0].RegisterSpace = 0;
	D3D12DescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12DescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	D3D12DescriptorRanges[1].NumDescriptors = 6;
	D3D12DescriptorRanges[1].BaseShaderRegister = 1; // ���� �ؽ�ó(t1 ~ t6)
	D3D12DescriptorRanges[1].RegisterSpace = 0;
	D3D12DescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	CD3DX12_ROOT_PARAMETER D3D12RootParameters[6]{};

	D3D12RootParameters[0].InitAsConstants(32, 0);								// ������Ʈ ����
	D3D12RootParameters[1].InitAsConstantBufferView(1);							// ī�޶� ����
	D3D12RootParameters[2].InitAsConstantBufferView(2);							// ���� ����
	D3D12RootParameters[3].InitAsDescriptorTable(1, &D3D12DescriptorRanges[0]); // �ؽ�ó ����
	D3D12RootParameters[4].InitAsDescriptorTable(1, &D3D12DescriptorRanges[1]); // ���� �ؽ�ó ����
	D3D12RootParameters[5].InitAsConstantBufferView(3);							// ���Ӿ� ����

	// IA�ܰ踦 ���, ��Ʈ�� ��� �ܰ踦 ���
	D3D12_ROOT_SIGNATURE_FLAGS D3D12RootSignatureFlags{ D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT };

	CD3DX12_STATIC_SAMPLER_DESC D3D12SamplerDesc{
		0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.0f, 1, D3D12_COMPARISON_FUNC_ALWAYS, D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK, 0.0f, D3D12_FLOAT32_MAX, D3D12_SHADER_VISIBILITY_PIXEL, 0 };
	CD3DX12_ROOT_SIGNATURE_DESC D3D12RootSignatureDesc{};

	D3D12RootSignatureDesc.Init(_countof(D3D12RootParameters), D3D12RootParameters, 1, &D3D12SamplerDesc, D3D12RootSignatureFlags);

	ID3DBlob* D3D12SignatureBlob{}, * D3D12ErrorBlob{};

	ThrowIfFailed(D3D12SerializeRootSignature(&D3D12RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &D3D12SignatureBlob, &D3D12ErrorBlob));
	ThrowIfFailed(D3D12Device->CreateRootSignature(0, D3D12SignatureBlob->GetBufferPointer(), D3D12SignatureBlob->GetBufferSize(),
		__uuidof(ID3D12RootSignature), (void**)m_D3D12RootSignature.GetAddressOf()));
}

void CGameScene::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// ���Ӿ� ���� ���ҽ� ����
	UINT Bytes = ((sizeof(CB_GAMESCENE_INFO) + 255) & ~255);

	m_D3D12GameSceneInfoConstantBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	ThrowIfFailed(m_D3D12GameSceneInfoConstantBuffer->Map(0, nullptr, (void**)&m_MappedGameSceneInfo));

	// ���� ���ҽ� ����
	Bytes = ((sizeof(CB_LIGHT) + 255) & ~255);

	m_D3D12LightsConstantBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	ThrowIfFailed(m_D3D12LightsConstantBuffer->Map(0, nullptr, (void**)&m_MappedLights));
}

void CGameScene::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// ���Ӿ� ���� ����
	D3D12GraphicsCommandList->SetGraphicsRootConstantBufferView(5, m_D3D12GameSceneInfoConstantBuffer->GetGPUVirtualAddress());

	// ���� ����
	memcpy(m_MappedLights->m_Lights, m_Lights.data(), sizeof(CB_LIGHT) * (UINT)m_Lights.size());
	m_MappedLights->m_GlobalAmbient = m_GlobalAmbient;

	D3D12GraphicsCommandList->SetGraphicsRootConstantBufferView(2, m_D3D12LightsConstantBuffer->GetGPUVirtualAddress());
}

void CGameScene::ReleaseShaderVariables()
{
	m_D3D12LightsConstantBuffer->Unmap(0, nullptr);
}

void CGameScene::ReleaseUploadBuffers()
{
	for (UINT i = 0; i < m_Objects.size(); ++i)
	{
		if (m_Objects[i])
		{
			m_Objects[i]->ReleaseUploadBuffers();
		}
	}

	if (m_Terrain)
	{
		m_Terrain->ReleaseUploadBuffers();
	}

	// Release() ������ ���� ������ �𸣰���
	//if (m_SkyBox)
	//{
	//	m_SkyBox->ReleaseUploadBuffers();
	//}

	//if (m_Trees)
	//{
	//	m_Trees->ReleaseUploadBuffers();
	//}
}

void CGameScene::ProcessInput(HWND hWnd, float ElapsedTime)
{
	float DeltaX{};
	float DeltaY{};

	if (!IsCursorActive)
	{
		RECT Rect{};

		GetWindowRect(hWnd, &Rect);

		POINT CurrentCursorPos{};

		GetCursorPos(&CurrentCursorPos);

		POINT OldCursorPos{ Rect.left + FRAME_BUFFER_WIDTH / 2, Rect.top + FRAME_BUFFER_HEIGHT / 2 };

		SetCursorPos(OldCursorPos.x, OldCursorPos.y);

		DeltaX = (CurrentCursorPos.x - OldCursorPos.x) * 10.0f * ElapsedTime;
		DeltaY = (CurrentCursorPos.y - OldCursorPos.y) * 10.0f * ElapsedTime;
	}

	if (m_Player->IsActive() && !m_Player->IsExploded() && !IsFreeCamera)
	{
		m_Player->Rotate(DeltaY, DeltaX, 0.0f, ElapsedTime);

		if ((GetAsyncKeyState('W') & 0x8000) && (GetAsyncKeyState('A') & 0x8000))
		{
			m_Player->Rotate(0.0f, -0.5f, 0.0f, ElapsedTime);
			(IsInside) ? m_Player->Move(m_Player->GetLook(), +10.0f * ElapsedTime, nullptr) : m_Player->Move(m_Player->GetLook(), +10.0f * ElapsedTime, (void*)m_Terrain.get());
		}
		else if ((GetAsyncKeyState('W') & 0x8000) && (GetAsyncKeyState('D') & 0x8000))
		{
			m_Player->Rotate(0.0f, +0.5f, 0.0f, ElapsedTime);
			(IsInside) ? m_Player->Move(m_Player->GetLook(), +10.0f * ElapsedTime, nullptr) : m_Player->Move(m_Player->GetLook(), +10.0f * ElapsedTime, (void*)m_Terrain.get());
		}
		else if ((GetAsyncKeyState('S') & 0x8000) && (GetAsyncKeyState('A') & 0x8000))
		{
			m_Player->Rotate(0.0f, +0.5f, 0.0f, ElapsedTime);
			(IsInside) ? m_Player->Move(m_Player->GetLook(), -10.0f * ElapsedTime, nullptr) : m_Player->Move(m_Player->GetLook(), -10.0f * ElapsedTime, (void*)m_Terrain.get());
		}
		else if ((GetAsyncKeyState('S') & 0x8000) && (GetAsyncKeyState('D') & 0x8000))
		{
			m_Player->Rotate(0.0f, -0.5f, 0.0f, ElapsedTime);
			(IsInside) ? m_Player->Move(m_Player->GetLook(), -10.0f * ElapsedTime, nullptr) : m_Player->Move(m_Player->GetLook(), -10.0f * ElapsedTime, (void*)m_Terrain.get());
		}
		else if (GetAsyncKeyState('W') & 0x8000)
		{
			(IsInside) ? m_Player->Move(m_Player->GetLook(), +10.0f * ElapsedTime, nullptr) : m_Player->Move(m_Player->GetLook(), +10.0f * ElapsedTime, (void*)m_Terrain.get());
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			(IsInside) ? m_Player->Move(m_Player->GetLook(), -10.0f * ElapsedTime, nullptr) : m_Player->Move(m_Player->GetLook(), -10.0f * ElapsedTime, (void*)m_Terrain.get());
		}

		if (GetAsyncKeyState('Q') & 0x8000)
		{
			m_Player->RotateCombatTower(ROTATION_LEFT, -0.5f);
		}
		else if (GetAsyncKeyState('E') & 0x8000)
		{
			m_Player->RotateCombatTower(ROTATION_RIGHT, 0.5f);
		}
	}
	else if (!m_Player->IsActive() && m_Player->IsExploded() || IsFreeCamera)
	{
		CCamera* Camera{ m_Player->GetCamera() };

		Camera->Rotate(DeltaY, DeltaX, 0.0f);

		if (GetAsyncKeyState('W') & 0x8000)
		{
			Camera->Move(Vector3::ScalarProduct(Camera->GetLook(), +30.0f * ElapsedTime, false));
		}

		if (GetAsyncKeyState('S') & 0x8000)
		{
			Camera->Move(Vector3::ScalarProduct(Camera->GetLook(), -30.0f * ElapsedTime, false));
		}

		if (GetAsyncKeyState('A') & 0x8000)
		{
			Camera->Move(Vector3::ScalarProduct(Camera->GetRight(), -30.0f * ElapsedTime, false));
		}

		if (GetAsyncKeyState('D') & 0x8000)
		{
			Camera->Move(Vector3::ScalarProduct(Camera->GetRight(), +30.0f * ElapsedTime, false));
		}
	}
}

void CGameScene::ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	switch (Message)
	{
	case WM_LBUTTONDOWN:
		if (!IsCursorActive)
		{
			m_Player->FireBullet();
		}
		break;
	case WM_RBUTTONDOWN:
		//PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		break;
	}
}

void CGameScene::ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	switch (Message)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_TAB:
			if (IsCursorActive)
			{
				RECT Rect{};

				GetWindowRect(hWnd, &Rect);

				POINT OldCursorPos{ Rect.left + FRAME_BUFFER_WIDTH / 2, Rect.top + FRAME_BUFFER_HEIGHT / 2 };

				SetCursorPos(OldCursorPos.x, OldCursorPos.y);
				ShowCursor(FALSE);

				IsCursorActive = false;
			}
			else
			{
				ShowCursor(TRUE);

				IsCursorActive = true;
			}
			break;
		case VK_F1:
			m_Player->GetCamera()->SetMode(CCamera::FIRST_PERSON);
			break;
		case VK_F3:
			m_Player->GetCamera()->SetMode(CCamera::THIRD_PERSON);
			break;
		case '1': // �÷��̾ �ǳ� �Ǵ� �ǿܷ� �̵���Ų��.
			if (IsInside)
			{
				IsInside = false;
				m_Player->SetPosition(XMFLOAT3(20.0f, 0.0f, 10.0f));
			}
			else
			{
				IsInside = true;
				m_Player->SetPosition(XMFLOAT3(-40.0f, 10.0f, 15.0f));
			}
			break;
		case '2': // ������ FILL_MODE�� SOLID �Ǵ� WIRE_FRAME���� �����ϵ��� ON/OFF�Ѵ�.
			IsSolidTerrain ? IsSolidTerrain = false : IsSolidTerrain = true;
			break;
		case '3': // ������ ���� �׽췹�̼��� �����ϵ��� ON/OFF�Ѵ�.
			m_MappedGameSceneInfo->m_IsTessellationActive ? m_MappedGameSceneInfo->m_IsTessellationActive = false : m_MappedGameSceneInfo->m_IsTessellationActive = true;
			break;
		case 'c':
		case 'C': // ī�޶� �����Ӱ� �̵��ϵ��� ON/OFF�Ѵ�.
			IsFreeCamera ? IsFreeCamera = false : IsFreeCamera = true;
			break;
		default:
			break;
		}
	}
}

void CGameScene::Animate(float ElapsedTime)
{
	m_MappedGameSceneInfo->m_TotalTime += ElapsedTime;
	m_MappedGameSceneInfo->m_ElapsedTime = ElapsedTime;

	for (int i = 0 ; i < ENEMY_COUNT; ++i)
	{
		if (!IsInside)
		{
			m_Objects[i]->Animate(ElapsedTime, m_Player->GetPosition());
			m_Objects[i]->Move(m_Objects[i]->GetMovingDirection(), 8.0f * ElapsedTime, (void*)m_Terrain.get());
		}
		
		m_Objects[i]->KeepDistanceToTerrain(ElapsedTime, (void*)m_Terrain.get(), m_Objects[i]->GetBoundingBox().Extents.y);
	}

	if (!IsFreeCamera)
	{
		m_Player->Animate(ElapsedTime);
		m_Player->KeepDistanceToTerrain(ElapsedTime, (void*)m_Terrain.get(), m_Player->GetBoundingBox().Extents.y);
		m_Player->KeepDistanceToCamera(ElapsedTime, (void*)m_Terrain.get(), 8.0f);
	}
	
	if (!IsInside)
	{
		if (m_Player->IsActive())
		{
			CheckPlayerByEnemyCollision(ElapsedTime);
		}

		CheckEnemyByEnemyCollision(ElapsedTime);
		CheckBulletByEnemyCollision();
		CheckBulletByTerrainCollision();
	}
	else
	{
		CheckPlayerByBoxCollision(ElapsedTime);
	}
}

void CGameScene::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12GraphicsCommandList->SetGraphicsRootSignature(m_D3D12RootSignature.Get());

	UpdateShaderVariables(D3D12GraphicsCommandList);
	m_Player->GetCamera()->UpdateShaderVariables(D3D12GraphicsCommandList);

	for (const auto& Shader : m_Shaders)
	{
		Shader->Render(D3D12GraphicsCommandList, m_Player->GetCamera());
	}
}

void CGameScene::BuildLights()
{
	Light SceneLight[1]{};

	SceneLight[0].m_IsActive = true;
	SceneLight[0].m_Type = DIRECTIONAL_LIGHT;
	SceneLight[0].m_Direction = XMFLOAT3(0.7f, -0.4f, 1.0f);
	SceneLight[0].m_Diffuse = XMFLOAT4(0.25f, 0.2f, 0.2f, 1.0f);
	SceneLight[0].m_Specular = XMFLOAT4(0.4f, 0.3f, 0.3f, 1.0f);
	SceneLight[0].m_Ambient = XMFLOAT4(0.4f, 0.3f, 0.3f, 1.0f);

	m_Lights.push_back(SceneLight[0]);

	m_GlobalAmbient = XMFLOAT4(0.2f, 0.0f, 0.0f, 1.0f);
}

void CGameScene::CheckPlayerByEnemyCollision(float ElapsedTime)
{
	if (!m_Player->IsExploded())
	{
		const auto& PlayerBoundingBox{ m_Player->GetBoundingBox() };

		for (UINT i = 0; i < ENEMY_COUNT; ++i)
		{
			CEnemyObject* EnemyObject{ (CEnemyObject*)m_Objects[i].get() };

			if (EnemyObject->IsActive() && !EnemyObject->IsExploded())
			{
				const auto& EnemyBoundingBox{ m_Objects[i]->GetBoundingBox() };

				if (PlayerBoundingBox.Intersects(EnemyBoundingBox))
				{
					XMFLOAT3 ToPlayer{ Vector3::Subtract(PlayerBoundingBox.Center, EnemyBoundingBox.Center) };
					XMFLOAT3 ToEnemy{ Vector3::Subtract(EnemyBoundingBox.Center, PlayerBoundingBox.Center) };

					ToPlayer = Vector3::Normalize(ToPlayer);
					ToEnemy = Vector3::Normalize(ToEnemy);

					m_Player->PrepareExplosion();
					m_Player->Move(ToPlayer, 100.0f * ElapsedTime, (void*)m_Terrain.get());
					EnemyObject->Move(ToEnemy, 100.0f * ElapsedTime, (void*)m_Terrain.get());

					CSound::GetInstance()->Play(CSound::TANK_COLLISION_SOUND, 0.6f);
				}
			}
		}
	}
}

void CGameScene::CheckPlayerByBoxCollision(float ElapsedTime)
{
	if (!m_Player->IsExploded())
	{
		const auto& PlayerBoundingBox{ m_Player->GetBoundingBox() };

		for (UINT i = WALL_COUNT; i < m_InsideObjects.size(); ++i)
		{
			if (m_InsideObjects[i]->IsActive())
			{
				m_InsideObjects[i]->UpdateBoundingBox();

				const auto& BoxBoundingBox{ m_InsideObjects[i]->GetBoundingBox() };

				if (PlayerBoundingBox.Intersects(BoxBoundingBox))
				{
					XMFLOAT3 ToPlayer{ Vector3::Subtract(PlayerBoundingBox.Center, BoxBoundingBox.Center) };

					ToPlayer = Vector3::Normalize(ToPlayer);
					m_Player->Move(ToPlayer, 8.0f * ElapsedTime, (void*)m_Terrain.get());
				}
			}
		}
	}
}

void CGameScene::CheckEnemyByEnemyCollision(float ElapsedTime)
{
	for (UINT i = 0; i < ENEMY_COUNT; ++i)
	{
		const auto& BoundingBox1{ m_Objects[i]->GetBoundingBox() };

		for (UINT j = 0; j < ENEMY_COUNT; ++j)
		{
			if (i == j)
			{
				continue;
			}

			const auto& BoundingBox2{ m_Objects[j]->GetBoundingBox() };

			if (BoundingBox1.Intersects(BoundingBox2))
			{
				XMFLOAT3 ToEnemy1{ Vector3::Subtract(BoundingBox1.Center, BoundingBox2.Center) };
				XMFLOAT3 ToEnemy2{ Vector3::Subtract(BoundingBox2.Center, BoundingBox1.Center) };

				ToEnemy1 = Vector3::Normalize(ToEnemy1);
				ToEnemy2 = Vector3::Normalize(ToEnemy2);

				m_Objects[i]->Move(ToEnemy1, 10.0f * ElapsedTime, (void*)m_Terrain.get());
				m_Objects[j]->Move(ToEnemy2, 10.0f * ElapsedTime, (void*)m_Terrain.get());
			}
		}
	}
}

void CGameScene::CheckBulletByEnemyCollision()
{
	for (UINT i = 0; i < m_Objects.size(); ++i)
	{
		if (m_Objects[i]->IsActive())
		{
			m_Player->CheckBulletByEnemyCollision(((CEnemyObject*)m_Objects[i].get()));
		}
	}
}

void CGameScene::CheckBulletByTerrainCollision()
{
	m_Player->CheckBulletByTerrainCollision((void*)m_Terrain.get());
}

void CGameScene::PickObjectPointedByCursor(int Xpos, int Ypos)
{
	XMFLOAT4X4 ViewMatrix{ m_Player->GetCamera()->GetViewMatrix() };
	XMFLOAT4X4 ProjectionMatrix{ m_Player->GetCamera()->GetProjectionMatrix() };

	// ȭ�� ��ǥ���� �� (xClient, yClient)�� ȭ�� ��ǥ ��ȯ�� ����ȯ�� ���� ��ȯ�� ����ȯ�� �Ѵ�.
	// �� ����� ī�޶� ��ǥ���� ���̴�.
	// �� ��, ���� ����� ī�޶󿡼� z-������ �Ÿ��� 1.0f�̹Ƿ� z-��ǥ�� 1.0f�� �����Ѵ�.
	XMFLOAT3 PickPosition{};

	PickPosition.x = (((2.0f * Xpos) / FRAME_BUFFER_WIDTH) - 1) / ProjectionMatrix._11;
	PickPosition.y = -(((2.0f * Ypos) / FRAME_BUFFER_HEIGHT) - 1) / ProjectionMatrix._22;
	PickPosition.z = 1.0f;

	UINT Intersected{};
	float HitDistance{ FLT_MAX }, NearestHitDistance{ FLT_MAX };

	shared_ptr<CObject> NearestObject{};

	// ���̴��� ��� ���� ��ü�鿡 ���� ���콺 ��ŷ�� �����Ͽ� ī�޶�� ���� ����� ���� ��ü�� ���Ѵ�.
	for (UINT i = 0; i < m_Shaders.size(); ++i)
	{
		shared_ptr<CObject> IntersectedObject{ m_Shaders[i]->PickObjectByRayIntersection(PickPosition, ViewMatrix, HitDistance) };

		if (IntersectedObject && (HitDistance < NearestHitDistance))
		{
			NearestHitDistance = HitDistance;
			NearestObject = IntersectedObject;
		}
	}

	if (NearestObject)
	{
		m_SelectedObject = NearestObject;
	}
}

const shared_ptr<CObject>& CGameScene::GetSelectedObject()
{
	return m_SelectedObject;
}
