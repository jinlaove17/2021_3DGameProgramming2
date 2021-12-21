#pragma once
#include "Player.h"

#define MAX_LIGHTS			1
#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

class CGraphicsShader;
class CDepthRenderShader;
class CShadowMapShader;

struct CB_GAMESCENE_INFO
{
	bool								m_IsTessellationActive{};

	float								m_TotalTime{};
	float								m_ElapsedTime{};
};

struct Light
{
	bool								m_IsActive{};

	XMFLOAT3							m_Position{};
	XMFLOAT3							m_Direction{};

	int									m_Type{};

	XMFLOAT4							m_Diffuse{};
	XMFLOAT4							m_Ambient{};
	XMFLOAT4							m_Specular{};

	XMFLOAT3							m_Attenuation;
	float 								m_Falloff{};
	float								m_Range;
	float 								m_Theta{};
	float								m_Phi{};

	float								PADDING{};

	XMFLOAT4X4							m_ToTextureMatrix{};
};

struct CB_LIGHT
{
	Light								m_Lights[MAX_LIGHTS]{};
	
	XMFLOAT4							m_GlobalAmbient{};
};

// =============================================== CScene ===============================================

class CScene
{
public:
	static queue<unique_ptr<CScene>>	m_Scenes;

protected:
	ComPtr<ID3D12RootSignature>			m_D3D12RootSignature{};

public:
	CScene() = default;
	virtual ~CScene() = default;

	virtual void OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;
	virtual void OnDestroy() = 0;

	virtual void BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;
	virtual void ReleaseObjects() = 0;

	virtual void CreateRootSignature(ID3D12Device* D3D12Device) = 0;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;
	virtual void ReleaseShaderVariables() = 0;

	virtual void ReleaseUploadBuffers() = 0;

	virtual void ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;
	virtual void ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;
	virtual void ProcessInput(HWND hWnd, float ElapsedTime) = 0;
	
	virtual void Animate(float ElapsedTime) = 0;
	virtual void PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;
};

// =============================================== CTitleScene ===============================================

class CTitleScene : public CScene
{
private:
	shared_ptr<CBackgroundObject>		m_Background{};
	shared_ptr<CButtonObject>			m_Buttons{};

	shared_ptr<CGraphicsShader>			m_Shader{};

public:
	CTitleScene() = default;
	virtual ~CTitleScene() = default;

	virtual void OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void OnDestroy();

	virtual void BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseObjects();

	virtual void CreateRootSignature(ID3D12Device* D3D12Device);

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ProcessInput(HWND hWnd, float ElapsedTime);

	virtual void Animate(float ElapsedTime);
	virtual void PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
};

// =============================================== CGameScene ===============================================

class CGameScene : public CScene
{
private:
	shared_ptr<CPlayer>							m_Player{};

	// Àû °´Ã¼
	vector<shared_ptr<CObject>>					m_Objects{};
	shared_ptr<CObject>							m_SelectedObject{};

	// ½Ç³»¿¡ ÀÖ´Â °´Ã¼
	vector<shared_ptr<CObject>>					m_InsideObjects{};

	shared_ptr<CTerrainObject>					m_Terrain{};

	// ºôº¸µå °´Ã¼
	shared_ptr<CHpBarObject>					m_HpBars{};
	shared_ptr<CExplodedEnemyObject>			m_ExplodedEnemies{};
	shared_ptr<CSkyBoxObject>					m_SkyBox{};
	shared_ptr<CTreeObject>						m_Trees{};
	shared_ptr<CSmokeObject>					m_Smoke{};

	vector<shared_ptr<CGraphicsShader>>			m_Shaders{};

	vector<Light>								m_Lights{};
	ComPtr<ID3D12Resource>						m_D3D12LightsConstantBuffer{};
	CB_LIGHT*									m_MappedLights{};
	XMFLOAT4									m_GlobalAmbient{};

	// °ÔÀÓ¾À Á¤º¸
	ComPtr<ID3D12Resource>						m_D3D12GameSceneInfoConstantBuffer{};
	CB_GAMESCENE_INFO*							m_MappedGameSceneInfo{};

	// ±×¸²ÀÚ
	shared_ptr<CDepthRenderShader>				m_DepthRenderShader{};
	shared_ptr<CShadowMapShader>				m_ShadowMapShader{};

public:
	CGameScene() = default;
	virtual ~CGameScene() = default;

	virtual void OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void OnDestroy();

	virtual void BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseObjects();

	virtual void CreateRootSignature(ID3D12Device* D3D12Device);

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ProcessInput(HWND hWnd, float ElapsedTime);

	virtual void Animate(float ElapsedTime);
	virtual void PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	void BuildLights();

	void CheckPlayerByEnemyCollision(float ElapsedTime);
	void CheckPlayerByBoxCollision(float ElapsedTime);
	void CheckEnemyByEnemyCollision(float ElapsedTime);
	void CheckBulletByEnemyCollision();
	void CheckBulletByTerrainCollision();

	void PickObjectPointedByCursor(int Xpos, int Ypos);
	const shared_ptr<CObject>& GetSelectedObject();
};
