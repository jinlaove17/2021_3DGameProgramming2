#pragma once
#include "Player.h"

class CCamera;
struct Light;

// ============================================== CShader ==============================================

class CShader
{
protected:
	vector<ComPtr<ID3D12PipelineState>>		m_D3D12PipelineStates{};

	ComPtr<ID3D12DescriptorHeap>			m_D3D12CbvSrvUavDescriptorHeap{};
	D3D12_CPU_DESCRIPTOR_HANDLE				m_D3D12CpuDescriptorHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE				m_D3D12GpuDescriptorHandle{};

public:
	void CreateCbvSrvUavDescriptorHeaps(ID3D12Device* D3D12Device, UINT CbvCount, UINT SrvCount, UINT UavCount);
	void CreateShaderResourceViews(ID3D12Device* D3D12Device, CTexture* Texture);

	D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR* FileName, const CHAR* ShaderName, const CHAR* ShaderModelName, ID3DBlob* D3D12ShaderBlob);
};

// ============================================== CShader ==============================================

class CComputeShader : public CShader
{
protected:
	XMUINT3									m_ThreadGroups{};

public:
	CComputeShader() = default;
	virtual ~CComputeShader() = default;

	virtual D3D12_SHADER_BYTECODE CreateComputeShader(ID3DBlob* D3D12ShaderBlob);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, const XMUINT3& ThreadGroups);

	virtual void Dispatch(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void Dispatch(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const XMUINT3& ThreadGroups);
};

// ============================================== CGraphicsShader ==============================================

class CGraphicsShader : public CShader
{
public:
	CGraphicsShader() = default;
	virtual ~CGraphicsShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(UINT PSONum = 0);
	virtual D3D12_BLEND_DESC CreateBlendState(UINT PSONum = 0);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(UINT PSONum = 0);
	virtual D3D12_STREAM_OUTPUT_DESC CreateStreamOutputState(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateDomainShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT PSONum = 0);

	virtual DXGI_FORMAT GetRTVFormat(UINT RTVNum = 0, UINT PSONum = 0);
	virtual DXGI_FORMAT GetDSVFormat(UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	virtual shared_ptr<CObject> PickObjectByRayIntersection(const XMFLOAT3& PickPosition, const XMFLOAT4X4& ViewMatrix, float& NearHitDistance);
};

// ============================================== CTitleShader ==============================================

class CTitleShader : public CGraphicsShader
{
private:
	vector<shared_ptr<CBilboardObject>>		m_Objects{};

public:
	CTitleShader(vector<shared_ptr<CBilboardObject>>& Objects);
	virtual ~CTitleShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CPlayerShader ==============================================

class CPlayerShader : public CGraphicsShader
{
private:
	shared_ptr<CPlayer>						m_Player{};

public:
	CPlayerShader(const shared_ptr<CPlayer>& Player);
	virtual ~CPlayerShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CBulletShader ==============================================

class CBulletShader : public CGraphicsShader
{
private:
	vector<shared_ptr<CBulletObject>>&		m_Bullets;

public:
	CBulletShader(vector<shared_ptr<CBulletObject>>& Bullets);
	virtual ~CBulletShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CObjectShader ==============================================

class CObjectShader : public CGraphicsShader
{
protected:
	vector<shared_ptr<CObject>>&			m_Objects;

public:
	CObjectShader(vector<shared_ptr<CObject>>& Objects);
	virtual ~CObjectShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual shared_ptr<CObject> PickObjectByRayIntersection(const XMFLOAT3& PickPosition, const XMFLOAT4X4& ViewMatrix, float& NearHitDistance);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CTerrainShader ==============================================

class CTerrainShader : public CGraphicsShader
{
private:
	shared_ptr<CTerrainObject>				m_Terrain{};

public:
	CTerrainShader(const shared_ptr<CTerrainObject>& Terrain);
	virtual ~CTerrainShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CTessellationTerrainShader ==============================================

class CTessellationTerrainShader : public CTerrainShader
{
public:
	CTessellationTerrainShader(const shared_ptr<CTerrainObject>& Terrain);
	virtual ~CTessellationTerrainShader() = default;

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateDomainShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);
};

// ============================================== CSkyBoxShader ==============================================

class CSkyBoxShader : public CGraphicsShader
{
private:
	shared_ptr<CSkyBoxObject>				m_SkyBox{};

public:
	CSkyBoxShader(const shared_ptr<CSkyBoxObject>& SkyBox);
	virtual ~CSkyBoxShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CHpBarShader ==============================================

class CHpBarShader : public CGraphicsShader
{
private:
	shared_ptr<CHpBarObject>				m_HpBars{};

public:
	CHpBarShader(const shared_ptr<CHpBarObject>& HpBars);
	virtual ~CHpBarShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CBilboardShader ==============================================

class CBilboardShader : public CGraphicsShader
{
private:
	vector<shared_ptr<CBilboardObject>>		m_Objects{};

public:
	CBilboardShader(vector<shared_ptr<CBilboardObject>>& Objects);
	virtual ~CBilboardShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);
	virtual D3D12_BLEND_DESC CreateBlendState(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CSpriteBilboardShader ==============================================

class CSpriteBilboardShader : public CGraphicsShader
{
private:
	vector<shared_ptr<CBilboardObject>>		m_Objects{};

public:
	CSpriteBilboardShader(vector<shared_ptr<CBilboardObject>>& Objects);
	virtual ~CSpriteBilboardShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);
	virtual D3D12_BLEND_DESC CreateBlendState(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CMirrorShader ==============================================

class CMirrorShader : public CGraphicsShader
{
private:
	shared_ptr<CMirrorObject>				m_Mirror{};
	shared_ptr<CMirrorBackObject>			m_MirrorBack{};

	shared_ptr<CPlayer>						m_Player{};
	vector<shared_ptr<CObject>>&			m_InsideObjects;

public:
	CMirrorShader(const shared_ptr<CPlayer>& Player, vector<shared_ptr<CObject>>& Objects);
	virtual ~CMirrorShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(UINT PSONum = 0);
	virtual D3D12_BLEND_DESC CreateBlendState(UINT PSONum = 0);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(UINT PSONum = 0);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CParticleShader ==============================================

class CParticleShader : public CGraphicsShader
{
private:
	shared_ptr<CParticleObject>			m_Particles{};

public:
	CParticleShader() = default;
	virtual ~CParticleShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);
	virtual D3D12_BLEND_DESC CreateBlendState(UINT PSONum = 0);
	virtual D3D12_STREAM_OUTPUT_DESC CreateStreamOutputState(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ============================================== CDepthRenderShader ==============================================

class CDepthRenderShader : public CGraphicsShader
{
protected:
	shared_ptr<CGraphicsShader>			m_ObjectShader{};
	vector<Light>&						m_Lights;

	shared_ptr<CTexture>				m_DepthTexture{};
	shared_ptr<CLightCamera>			m_LightCamera{};

	ComPtr<ID3D12Resource>				m_D3D12DepthBuffer{};
	ComPtr<ID3D12DescriptorHeap>		m_D3D12DsvDescriptorHeap{};

	ComPtr<ID3D12DescriptorHeap>		m_D3D12RtvDescriptorHeap{};

	XMMATRIX							m_ProjectionToTexture{};

public:
	CDepthRenderShader(const shared_ptr<CGraphicsShader>& ObjectShader, vector<Light>& Lights);
	virtual ~CDepthRenderShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual DXGI_FORMAT GetDSVFormat(UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	void CreateRtvAndDsvDescriptorHeaps(ID3D12Device* D3D12Device);
	void CreateRenderTargetViews(ID3D12Device* D3D12Device);
	void CreateDepthStencilView(ID3D12Device* D3D12Device);

	void PrepareShadowMap(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	shared_ptr<CTexture> GetDepthTexture() const;
};

// ============================================== CShadowMapShader ==============================================

class CShadowMapShader : public CGraphicsShader
{
private:
	shared_ptr<CGraphicsShader>			m_ObjectsShader{};
	shared_ptr<CTexture>				m_DepthTexture{};

public:
	CShadowMapShader(const shared_ptr<CGraphicsShader>& ObjectShader, const shared_ptr<CTexture>& DepthTexture);
	virtual ~CShadowMapShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};