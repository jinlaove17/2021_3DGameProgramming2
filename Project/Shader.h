#pragma once
#include "Player.h"

class CCamera;

// ============================================== CShader ==============================================

class CShader
{
protected:
	vector<ComPtr<ID3D12PipelineState>>		m_D3D12PipelineStates{};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC		m_D3D12PipelineStateDesc{};

	ComPtr<ID3D12DescriptorHeap>			m_D3D12CbvSrvUavDescriptorHeap{};
	D3D12_CPU_DESCRIPTOR_HANDLE				m_D3D12CpuDescriptorHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE				m_D3D12GpuDescriptorHandle{};

public:
	CShader() = default;
	virtual ~CShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateDomainShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob);
	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	virtual shared_ptr<CObject> PickObjectByRayIntersection(const XMFLOAT3& PickPosition, const XMFLOAT4X4& ViewMatrix, float& NearHitDistance);

	void CreateCbvSrvUavDescriptorHeaps(ID3D12Device* D3D12Device, UINT CbvCount, UINT SrvCount, UINT UavCount);
	void CreateShaderResourceViews(ID3D12Device* D3D12Device, CTexture* Texture);

	D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR* FileName, const CHAR* ShaderName, const CHAR* ShaderModelName, ID3DBlob* D3D12ShaderBlob);
};

// ============================================== CTitleShader ==============================================

class CTitleShader : public CShader
{
private:
	vector<shared_ptr<CBilboardObject>>		m_Objects{};

public:
	CTitleShader(vector<shared_ptr<CBilboardObject>>& Objects);
	virtual ~CTitleShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob);
	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CPlayerShader ==============================================

class CPlayerShader : public CShader
{
private:
	shared_ptr<CPlayer>						m_Player{};

public:
	CPlayerShader(const shared_ptr<CPlayer>& Player);
	virtual ~CPlayerShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob);
	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CBulletShader ==============================================

class CBulletShader : public CShader
{
private:
	vector<shared_ptr<CBulletObject>>&		m_Bullets;

public:
	CBulletShader(vector<shared_ptr<CBulletObject>>& Bullets);
	virtual ~CBulletShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob);
	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CObjectShader ==============================================

class CObjectShader : public CShader
{
protected:
	vector<shared_ptr<CObject>>&			m_Objects;

public:
	CObjectShader(vector<shared_ptr<CObject>>& Objects);
	virtual ~CObjectShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob);
	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature);

	virtual shared_ptr<CObject> PickObjectByRayIntersection(const XMFLOAT3& PickPosition, const XMFLOAT4X4& ViewMatrix, float& NearHitDistance);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CTerrainShader ==============================================

class CTerrainShader : public CShader
{
private:
	shared_ptr<CTerrainObject>				m_Terrain{};

public:
	CTerrainShader(const shared_ptr<CTerrainObject>& Terrain);
	virtual ~CTerrainShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob);
	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CTessellationTerrainShader ==============================================

class CTessellationTerrainShader : public CTerrainShader
{
public:
	CTessellationTerrainShader(const shared_ptr<CTerrainObject>& Terrain);
	virtual ~CTessellationTerrainShader() = default;

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateDomainShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob);
	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature);
};

// ============================================== CSkyBoxShader ==============================================

class CSkyBoxShader : public CShader
{
private:
	shared_ptr<CSkyBoxObject>				m_SkyBox{};

public:
	CSkyBoxShader(const shared_ptr<CSkyBoxObject>& SkyBox);
	virtual ~CSkyBoxShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob);
	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CHpBarShader ==============================================

class CHpBarShader : public CShader
{
private:
	shared_ptr<CHpBarObject>				m_HpBars{};

public:
	CHpBarShader(const shared_ptr<CHpBarObject>& HpBars);
	virtual ~CHpBarShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob);
	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CBilboardShader ==============================================

class CBilboardShader : public CShader
{
private:
	vector<shared_ptr<CBilboardObject>>		m_Objects{};

public:
	CBilboardShader(vector<shared_ptr<CBilboardObject>>& Objects);
	virtual ~CBilboardShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob);
	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CSpriteBilboardShader ==============================================

class CSpriteBilboardShader : public CShader
{
private:
	vector<shared_ptr<CBilboardObject>>		m_Objects{};

public:
	CSpriteBilboardShader(vector<shared_ptr<CBilboardObject>>& Objects);
	virtual ~CSpriteBilboardShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob);
	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CMirrorShader ==============================================

class CMirrorShader : public CShader
{
public:
	shared_ptr<CObject>						m_Mirror{};

	CMirrorShader(const shared_ptr<CObject>& Mirror);
	virtual ~CMirrorShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob);
	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature);

	void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT PSONum);
};
