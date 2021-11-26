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
	// 배경 객체를 생성한다.
	m_Background = make_shared<CBackgroundObject>(D3D12Device, D3D12GraphicsCommandList);

	// 버튼 객체를 생성한다.
	const UINT ButtonCount{ 2 };

	m_Buttons = make_shared<CButtonObject>(D3D12Device, D3D12GraphicsCommandList, ButtonCount);

	vector<shared_ptr<CBilboardObject>> Objects{};

	// 타이틀 화면에는 배경과 버튼 2개가 있다.
	Objects.reserve(ButtonCount + 1);
	Objects.push_back(m_Background);
	Objects.push_back(m_Buttons);

	// 쉐이더를 생성한다.
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
	D3D12DescriptorRanges[0].BaseShaderRegister = 0; // 텍스쳐(t0)
	D3D12DescriptorRanges[0].RegisterSpace = 0;
	D3D12DescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	CD3DX12_ROOT_PARAMETER D3D12RootParameters[1]{};

	D3D12RootParameters[0].InitAsDescriptorTable(1, &D3D12DescriptorRanges[0]); // 텍스쳐 정보

	D3D12_ROOT_SIGNATURE_FLAGS D3D12RootSignatureFlags{ D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT };	// IA단계를 허용

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
	// Release() 오류가 나는 이유를 모르겠음
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
	// 카메라 생성
	shared_ptr<CCamera> Camera{ make_shared<CCamera>() };

	Camera->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	Camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
	Camera->GenerateProjectionMatrix(90.0f, (float)FRAME_BUFFER_WIDTH / (float)FRAME_BUFFER_HEIGHT, 1.0f, 1000.0f);

	// 플레이어 생성
	m_Player = make_shared<CPlayer>(D3D12Device, D3D12GraphicsCommandList);
	m_Player->SetCamera(Camera);
	m_Player->SetActive(true);
	m_Player->SetPosition(XMFLOAT3(20.0f, 0.0f, 10.0f));

	// 스카이 박스 객체를 생성한다.
	m_SkyBox = make_shared<CSkyBoxObject>(D3D12Device, D3D12GraphicsCommandList);

	// 지형 객체를 생성한다.
#ifdef TERRAIN_TESSELLATION
	m_Terrain = make_shared<CTerrainObject>(D3D12Device, D3D12GraphicsCommandList, L"Image/HeightMap.raw", 257, 257, 13, 13, XMFLOAT3(1.0f, 0.6f, 1.0f));
#else
	m_Terrain = make_shared<CTerrainObject>(D3D12Device, D3D12GraphicsCommandList, L"Image/HeightMap.raw", 257, 257, 257, 257, XMFLOAT3(1.0f, 0.6f, 1.0f));
#endif
	m_Terrain->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);

	// 빌보드 객체(나무)를 생성한다.
	m_Trees = make_shared<CTreeObject>(D3D12Device, D3D12GraphicsCommandList, (void*)m_Terrain.get());

	// 플레이어 + 적 객체의 수만큼 빌보드 객체(체력바, 폭발)를 생성한다.
	m_HpBars = make_shared<CHpBarObject>(D3D12Device, D3D12GraphicsCommandList, MAX_ENEMY + 1);
	m_ExplodedEnemies = make_shared<CExplodedEnemyObject>(D3D12Device, D3D12GraphicsCommandList, MAX_ENEMY + 1);
	m_Smoke = make_shared<CSmokeObject>(D3D12Device, D3D12GraphicsCommandList, 50);

	CBilboardMesh* HpBarMesh{ m_HpBars->GetMappedMesh() };
	CSpriteBilboardMesh* ExplosionMesh{ m_ExplodedEnemies->GetMappedMesh() };

	m_Player->SetHpBarMesh(HpBarMesh++);
	m_Player->SetExplosionMesh(ExplosionMesh++);
	m_Player->SetSmokeMesh(m_Smoke->GetMappedMesh());

	for (UINT i = 0; i < MAX_ENEMY; ++i)
	{
		// 적 객체를 생성한다.
		shared_ptr<CEnemyObject> EnemyObject{ make_shared<CEnemyObject>() };

		// 체력바 객체와 폭발 객체로부터 리소스의 매핑된 주소를 가져온 후 적 객체에 설정한다. 
		EnemyObject->OnInitialize();
		EnemyObject->SetActive(true);
		EnemyObject->SetHpBarMesh(HpBarMesh++);
		EnemyObject->SetExplosionMesh(ExplosionMesh++);
		EnemyObject->Rotate(EnemyObject->GetUp(), 180.0f);
		EnemyObject->SetPosition(XMFLOAT3(20.0f + 30.0f * i, 0.0f, m_Terrain->GetLength() - 40.0f - 30.0f * i));
		EnemyObject->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);

		m_Objects.push_back(EnemyObject);
	}

	// 벽 객체를 생성한다.
	XMFLOAT3 Axis{ 0.0f, 1.0f, 0.0f };
	float Height{ m_Terrain->GetHeight(0.0f, 0.0f) };
	shared_ptr<CWallObject> WallObject{};

	WallObject = make_shared<CWallObject>();
	WallObject->SetActive(true);
	WallObject->Scale(22.0f, 20.0f, 1.0f);
	WallObject->SetPosition(XMFLOAT3(-40.0f, 20.0f, 0.0f));
	WallObject->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Objects.push_back(WallObject);

	WallObject = make_shared<CWallObject>();
	WallObject->SetActive(true);
	WallObject->Rotate(Axis, 180.0f);
	WallObject->Scale(22.0f, 20.0f, 1.0f);
	WallObject->SetPosition(XMFLOAT3(-40.0f, 20.0f, 100.0f));
	WallObject->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Objects.push_back(WallObject);

	WallObject = make_shared<CWallObject>();
	WallObject->SetActive(true);
	WallObject->Rotate(Axis, 90.0f);
	WallObject->Scale(20.0f, 20.0f, 1.0f);
	WallObject->SetPosition(XMFLOAT3(-60.0f, 20.0f, 20.0f));
	WallObject->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Objects.push_back(WallObject);

	WallObject = make_shared<CWallObject>();
	WallObject->SetActive(true);
	WallObject->Rotate(Axis, -90.0f);
	WallObject->Scale(20.0f, 20.0f, 1.0f);
	WallObject->SetPosition(XMFLOAT3(-20.0f, 20.0f, 20.0f));
	WallObject->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Objects.push_back(WallObject);

	Axis = { 1.0f, 0.0f, 0.0f };

	WallObject = make_shared<CWallObject>();
	WallObject->SetActive(true);
	WallObject->Rotate(Axis, 90.0f);
	WallObject->Scale(22.0f, 22.0f, 1.0f);
	WallObject->SetPosition(XMFLOAT3(-40.0f, 40.0f, 20.0f));
	WallObject->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	//m_Objects.push_back(WallObject);

	WallObject = make_shared<CWallObject>();
	WallObject->SetActive(true);
	WallObject->Rotate(Axis, 90.0f);
	WallObject->Scale(22.0f, 22.0f, 1.0f);
	WallObject->SetPosition(XMFLOAT3(-40.0f, 0.0f, 20.0f));
	WallObject->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Objects.push_back(WallObject);

	// 스카이 박스의 쉐이더를 생성하고 쉐이더 벡터에 추가한다.
	shared_ptr<CShader> SkyBoxShader{ make_shared<CSkyBoxShader>(m_SkyBox) };

	SkyBoxShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	SkyBoxShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	//m_Shaders.push_back(SkyBoxShader);

	// 지형의 쉐이더를 생성하고 쉐이더 벡터에 추가한다.
#ifdef TERRAIN_TESSELLATION
	shared_ptr<CShader> TerrainShader{ make_shared<CTessellationTerrainShader>(m_Terrain) };
#else
	shared_ptr<CShader> TerrainShader{ make_shared<CTerrainShader>(m_Terrain) };
#endif
	TerrainShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	TerrainShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(TerrainShader);

	// 적 객체의 쉐이더를 생성하고 쉐이더 벡터에 추가한다.
	shared_ptr<CShader> ObjectShader{ make_shared<CObjectShader>(m_Objects) };

	ObjectShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	ObjectShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(ObjectShader);

	// 적 객체의 체력바 쉐이더를 생성하고 쉐이더 벡터에 추가한다.
	shared_ptr<CShader> HpBarShader{ make_shared<CHpBarShader>(m_HpBars) };

	HpBarShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	HpBarShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(HpBarShader);

	// 빌보드 객체들을 담는 벡터를 생성한다.
	vector<shared_ptr<CBilboardObject>> BilboardObjects{};

	BilboardObjects.push_back(m_Trees);

	// 빌보드 쉐이더를 생성한다.
	shared_ptr<CShader> BilboardShader{ make_shared<CBilboardShader>(BilboardObjects) };

	BilboardShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	BilboardShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(BilboardShader);

	// 빌보드 객체들을 담는 벡터를 초기화 한 이후 추가한다.
	BilboardObjects.clear();
	BilboardObjects.reserve(3);
	BilboardObjects.push_back(m_ExplodedEnemies);
	BilboardObjects.push_back(m_Player->GetExplodedBullets());
	BilboardObjects.push_back(m_Smoke);

	// 스프라이트 빌보드 쉐이더를 생성한다.
	shared_ptr<CShader> SpriteBilboardShader{ make_shared<CSpriteBilboardShader>(BilboardObjects) };

	SpriteBilboardShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	SpriteBilboardShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(SpriteBilboardShader);

	// 플레이어 쉐이더를 생성하고 쉐이더 벡터에 추가한다.
	shared_ptr<CShader> PlayerShader{ make_shared<CPlayerShader>(m_Player) };

	PlayerShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	PlayerShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(PlayerShader);

	// 플레이어의 총알 쉐이더를 생성하고 쉐이더 벡터에 추가한다.
	shared_ptr<CShader> BulletShader{ make_shared<CBulletShader>(m_Player->GetBullets()) };

	BulletShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	BulletShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Shaders.push_back(BulletShader);

	// 거울과 거울 쉐이더를 생성한다.
	m_Mirror = make_shared<CObject>();
	m_Mirror->SetPosition(XMFLOAT3(-40.0f, 10.0f, 40.0f));
	m_Mirror->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_MirrorShader = make_shared<CMirrorShader>(m_Mirror);
	m_MirrorShader->CreatePipelineStateObject(D3D12Device, m_D3D12RootSignature.Get());
	m_MirrorShader->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);

	// 조명 및 조명관련 리소스를 생성한다.
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
	D3D12DescriptorRanges[0].BaseShaderRegister = 0; // 텍스쳐(t0)
	D3D12DescriptorRanges[0].RegisterSpace = 0;
	D3D12DescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12DescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	D3D12DescriptorRanges[1].NumDescriptors = 6;
	D3D12DescriptorRanges[1].BaseShaderRegister = 1; // 지형 텍스쳐(t1 ~ t6)
	D3D12DescriptorRanges[1].RegisterSpace = 0;
	D3D12DescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	CD3DX12_ROOT_PARAMETER D3D12RootParameters[6]{};

	D3D12RootParameters[0].InitAsConstantBufferView(0);							// 오브젝트 정보
	D3D12RootParameters[1].InitAsConstantBufferView(1);							// 카메라 정보
	D3D12RootParameters[2].InitAsConstantBufferView(2);							// 조명 정보
	D3D12RootParameters[3].InitAsDescriptorTable(1, &D3D12DescriptorRanges[0]); // 텍스쳐 정보
	D3D12RootParameters[4].InitAsDescriptorTable(1, &D3D12DescriptorRanges[1]); // 지형 텍스쳐 정보
	D3D12RootParameters[5].InitAsConstantBufferView(3);							// 게임씬 정보

	D3D12_ROOT_SIGNATURE_FLAGS D3D12RootSignatureFlags{ D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT };	// IA단계를 허용

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
	UINT Bytes = ((sizeof(CB_GAMESCENE_INFO) + 255) & ~255);

	m_D3D12GameSceneInfoConstantBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	ThrowIfFailed(m_D3D12GameSceneInfoConstantBuffer->Map(0, nullptr, (void**)&m_MappedGameSceneInfo));

	Bytes = ((sizeof(CB_LIGHT) + 255) & ~255);

	m_D3D12LightsConstantBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	ThrowIfFailed(m_D3D12LightsConstantBuffer->Map(0, nullptr, (void**)&m_MappedLights));
}

void CGameScene::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// 조명 갱신
	memcpy(m_MappedLights->m_Lights, m_Lights.data(), sizeof(CB_LIGHT) * (UINT)m_Lights.size());
	m_MappedLights->m_GlobalAmbient = m_GlobalAmbient;

	D3D12GraphicsCommandList->SetGraphicsRootConstantBufferView(2, m_D3D12LightsConstantBuffer->GetGPUVirtualAddress());

	// 게임씬 정보 갱신
	D3D12GraphicsCommandList->SetGraphicsRootConstantBufferView(5, m_D3D12GameSceneInfoConstantBuffer->GetGPUVirtualAddress());
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

	// Release() 오류가 나는 이유를 모르겠음
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

	if (m_Player->IsActive() && !m_Player->IsExploded() && !m_FreeCamera)
	{
		m_Player->Rotate(DeltaY, DeltaX, 0.0f, ElapsedTime);

		if ((GetAsyncKeyState('W') & 0x8000) && (GetAsyncKeyState('A') & 0x8000))
		{
			m_Player->Rotate(0.0f, -0.5f, 0.0f, ElapsedTime);
			m_Player->Move(m_Player->GetLook(), +10.0f * ElapsedTime, (void*)m_Terrain.get());
		}
		else if ((GetAsyncKeyState('W') & 0x8000) && (GetAsyncKeyState('D') & 0x8000))
		{
			m_Player->Rotate(0.0f, +0.5f, 0.0f, ElapsedTime);
			m_Player->Move(m_Player->GetLook(), +10.0f * ElapsedTime, (void*)m_Terrain.get());
		}
		else if ((GetAsyncKeyState('S') & 0x8000) && (GetAsyncKeyState('A') & 0x8000))
		{
			m_Player->Rotate(0.0f, +0.5f, 0.0f, ElapsedTime);
			m_Player->Move(m_Player->GetLook(), -10.0f * ElapsedTime, (void*)m_Terrain.get());
		}
		else if ((GetAsyncKeyState('S') & 0x8000) && (GetAsyncKeyState('D') & 0x8000))
		{
			m_Player->Rotate(0.0f, -0.5f, 0.0f, ElapsedTime);
			m_Player->Move(m_Player->GetLook(), -10.0f * ElapsedTime, (void*)m_Terrain.get());
		}
		else if (GetAsyncKeyState('W') & 0x8000)
		{
			m_Player->Move(m_Player->GetLook(), +10.0f * ElapsedTime, (void*)m_Terrain.get());
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			m_Player->Move(m_Player->GetLook(), -10.0f * ElapsedTime, (void*)m_Terrain.get());
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
	else if (!m_Player->IsActive() && m_Player->IsExploded() || m_FreeCamera)
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
		case 'i':
		case 'I':
			m_Player->SetPosition(XMFLOAT3(-40.0f, 55.0f, 15.0f));
			break;
		case 't':
		case 'T':
			m_MappedGameSceneInfo->m_IsActiveTessellation ? m_MappedGameSceneInfo->m_IsActiveTessellation = false : m_MappedGameSceneInfo->m_IsActiveTessellation = true;
			break;
		case 'c':
		case 'C':
			m_FreeCamera ? m_FreeCamera = false : m_FreeCamera = true;
			break;
		default:
			break;
		}
	}
}

void CGameScene::Animate(float ElapsedTime)
{
	for (int i = 0 ; i < MAX_ENEMY; ++i)
	{
		m_Objects[i]->Animate(ElapsedTime, m_Player->GetPosition());
		m_Objects[i]->KeepDistanceToTerrain(ElapsedTime, (void*)m_Terrain.get(), m_Objects[i]->GetBoundingBox().Extents.y);
	}

	if (m_Player->IsActive() && !m_FreeCamera)
	{
		m_Player->Animate(ElapsedTime);
		m_Player->KeepDistanceToTerrain(ElapsedTime, (void*)m_Terrain.get(), m_Player->GetBoundingBox().Extents.y);
		m_Player->KeepDistanceToCamera(ElapsedTime, (void*)m_Terrain.get(), 8.0f);
	}
	
	CheckPlayerByEnemyCollision(ElapsedTime);
	CheckEnemyByEnemyCollision(ElapsedTime);
	CheckBulletByEnemyCollision();
	CheckBulletByTerrainCollision();
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
	
	// 거울을 스텐실 버퍼에 렌더링(렌더 타겟에 출력하지 않음)
	D3D12GraphicsCommandList->OMSetStencilRef(1);
	m_MirrorShader->Render(D3D12GraphicsCommandList, 0);
	m_Mirror->Render(D3D12GraphicsCommandList, m_Player->GetCamera());

	// 거울에 반사된 객체를 렌더링
	XMVECTOR MirrorPlane{ 0.0f, 0.0f, -1.0f, 40.0f };
	XMMATRIX XMReflectMatrix{ XMMatrixReflect(MirrorPlane) };
	XMFLOAT4X4 ReflectMatrix{};

	XMStoreFloat4x4(&ReflectMatrix, XMReflectMatrix);
	
	XMFLOAT4X4 TransformMatrix{ m_Player->m_TransformMatrix };

	D3D12GraphicsCommandList->OMSetStencilRef(1);
	m_MirrorShader->Render(D3D12GraphicsCommandList, 1);
	//m_Player->m_TransformMatrix = Matrix4x4::Multiply(m_Player->m_TransformMatrix, ReflectMatrix);
	m_Player->Render(D3D12GraphicsCommandList, m_Player->GetCamera());
	//m_Player->m_TransformMatrix = TransformMatrix;
	//m_Player->UpdateTransform(Matrix4x4::Identity());

	// 거울을 블렌딩
	m_MirrorShader->Render(D3D12GraphicsCommandList, 2);
	m_Mirror->Render(D3D12GraphicsCommandList, m_Player->GetCamera());
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

		for (UINT i = 0; i < MAX_ENEMY; ++i)
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

void CGameScene::CheckEnemyByEnemyCollision(float ElapsedTime)
{
	for (UINT i = 0; i < MAX_ENEMY; ++i)
	{
		const auto& BoundingBox1{ m_Objects[i]->GetBoundingBox() };

		for (UINT j = 0; j < MAX_ENEMY; ++j)
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

	// 화면 좌표계의 점 (xClient, yClient)를 화면 좌표 변환의 역변환과 투영 변환의 역변환을 한다.
	// 그 결과는 카메라 좌표계의 점이다.
	// 이 때, 투영 평면이 카메라에서 z-축으로 거리가 1.0f이므로 z-좌표는 1.0f로 설정한다.
	XMFLOAT3 PickPosition{};

	PickPosition.x = (((2.0f * Xpos) / FRAME_BUFFER_WIDTH) - 1) / ProjectionMatrix._11;
	PickPosition.y = -(((2.0f * Ypos) / FRAME_BUFFER_HEIGHT) - 1) / ProjectionMatrix._22;
	PickPosition.z = 1.0f;

	UINT Intersected{};
	float HitDistance{ FLT_MAX }, NearestHitDistance{ FLT_MAX };

	shared_ptr<CObject> NearestObject{};

	// 쉐이더의 모든 게임 객체들에 대한 마우스 픽킹을 수행하여 카메라와 가장 가까운 게임 객체를 구한다.
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
