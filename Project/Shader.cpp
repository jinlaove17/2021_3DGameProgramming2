#include "stdafx.h"
#include "Shader.h"

// ============================================== CShader ==============================================

void CShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	return;
}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	return;
}

void CShader::ReleaseShaderVariables()
{
	return;
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	return D3D12InputLayoutDesc;
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC D3D12RasterizerDesc{};

	D3D12RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	D3D12RasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	D3D12RasterizerDesc.FrontCounterClockwise = false;
	D3D12RasterizerDesc.DepthBias = 0;
	D3D12RasterizerDesc.DepthBiasClamp = 0.0f;
	D3D12RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	D3D12RasterizerDesc.DepthClipEnable = true;
	D3D12RasterizerDesc.MultisampleEnable = false;
	D3D12RasterizerDesc.AntialiasedLineEnable = false;
	D3D12RasterizerDesc.ForcedSampleCount = 0;
	D3D12RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return D3D12RasterizerDesc;
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC D3D12BlendDesc{};

	D3D12BlendDesc.AlphaToCoverageEnable = false;
	D3D12BlendDesc.IndependentBlendEnable = false;
	D3D12BlendDesc.RenderTarget[0].BlendEnable = false;
	D3D12BlendDesc.RenderTarget[0].LogicOpEnable = false;
	D3D12BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	D3D12BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	D3D12BlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	D3D12BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	D3D12BlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	D3D12BlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	D3D12BlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	D3D12BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return D3D12BlendDesc;
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC D3D12DepthStencilDesc{};

	D3D12DepthStencilDesc.DepthEnable = true;
	D3D12DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	D3D12DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	D3D12DepthStencilDesc.StencilEnable = false;
	D3D12DepthStencilDesc.StencilReadMask = 0xff;
	D3D12DepthStencilDesc.StencilWriteMask = 0xff;
	D3D12DepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	D3D12DepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return D3D12DepthStencilDesc;
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_SHADER_BYTECODE CShader::CreateHullShader(ID3DBlob* D3D12ShaderBlob)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_SHADER_BYTECODE CShader::CreateDomainShader(ID3DBlob* D3D12ShaderBlob)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_SHADER_BYTECODE CShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

void CShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature)
{
	ComPtr<ID3DBlob> D3D12VertexShaderBlob{}, D3D12HullShaderBlob{}, D3D12DomainShaderBlob{}, D3D12GeometryShaderBlob{}, D3D12PixelShaderBlob{};

	m_D3D12PipelineStateDesc.pRootSignature = D3D12RootSignature;
	m_D3D12PipelineStateDesc.VS = CreateVertexShader(D3D12VertexShaderBlob.Get());
	m_D3D12PipelineStateDesc.HS = CreateHullShader(D3D12HullShaderBlob.Get());
	m_D3D12PipelineStateDesc.DS = CreateDomainShader(D3D12DomainShaderBlob.Get());
	m_D3D12PipelineStateDesc.GS = CreateGeometryShader(D3D12GeometryShaderBlob.Get());
	m_D3D12PipelineStateDesc.PS = CreatePixelShader(D3D12PixelShaderBlob.Get());
	m_D3D12PipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_D3D12PipelineStateDesc.BlendState = CreateBlendState();
	m_D3D12PipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_D3D12PipelineStateDesc.InputLayout = CreateInputLayout();
	m_D3D12PipelineStateDesc.SampleMask = UINT_MAX;
	m_D3D12PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_D3D12PipelineStateDesc.NumRenderTargets = 1;
	m_D3D12PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_D3D12PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_D3D12PipelineStateDesc.SampleDesc.Count = 1;
	m_D3D12PipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	m_D3D12PipelineStates.emplace_back();
}

void CShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	return;
}

shared_ptr<CObject> CShader::PickObjectByRayIntersection(const XMFLOAT3& PickPosition, const XMFLOAT4X4& ViewMatrix, float& NearHitDistance)
{
	return nullptr;
}

void CShader::CreateCbvSrvUavDescriptorHeaps(ID3D12Device* D3D12Device, UINT CbvCount, UINT SrvCount, UINT UavCount)
{
	D3D12_DESCRIPTOR_HEAP_DESC D3D12DescriptorHeapDesc{};

	D3D12DescriptorHeapDesc.NumDescriptors = CbvCount + SrvCount + UavCount;
	D3D12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	D3D12DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	D3D12DescriptorHeapDesc.NodeMask = 0;

	ThrowIfFailed(D3D12Device->CreateDescriptorHeap(&D3D12DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_D3D12CbvSrvUavDescriptorHeap.GetAddressOf()));

	m_D3D12CpuDescriptorHandle = m_D3D12CbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_D3D12GpuDescriptorHandle = m_D3D12CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

void CShader::CreateShaderResourceViews(ID3D12Device* D3D12Device, CTexture* Texture)
{
	D3D12_CPU_DESCRIPTOR_HANDLE D3D12CpuDescriptorStartHandle{ m_D3D12CbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	D3D12_GPU_DESCRIPTOR_HANDLE D3D12GpuDescriptorStartHandle{ m_D3D12CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart() };
	UINT DescriptorIncrementSize{ D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) };

	for (UINT i = 0; i < Texture->GetTextureCount(); ++i)
	{
		ID3D12Resource* ShaderResource{ Texture->GetResource(i) };
		D3D12_SHADER_RESOURCE_VIEW_DESC D3D12ShaderResourceViewDesc{ Texture->GetShaderResourceViewDesc(i) };

		D3D12Device->CreateShaderResourceView(ShaderResource, nullptr/*&D3D12ShaderResourceViewDesc*/, m_D3D12CpuDescriptorHandle);

		m_D3D12CpuDescriptorHandle.ptr += DescriptorIncrementSize;

		Texture->SetGpuDescriptorHandle(m_D3D12GpuDescriptorHandle);

		m_D3D12GpuDescriptorHandle.ptr += DescriptorIncrementSize;
	}
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(const WCHAR* FileName, const CHAR* ShaderName, const CHAR* ShaderModelName, ID3DBlob* D3D12ShaderBlob)
{
	UINT CompileFlags{};

#ifdef DEBUG_MODE
	CompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ThrowIfFailed(D3DCompileFromFile(FileName, nullptr, nullptr, ShaderName, ShaderModelName, CompileFlags, 0, &D3D12ShaderBlob, nullptr));

	D3D12_SHADER_BYTECODE D3D12ShaderBytecode{};

	D3D12ShaderBytecode.BytecodeLength = D3D12ShaderBlob->GetBufferSize();
	D3D12ShaderBytecode.pShaderBytecode = D3D12ShaderBlob->GetBufferPointer();

	return D3D12ShaderBytecode;
}

// ============================================== CTitleShader ==============================================

CTitleShader::CTitleShader(vector<shared_ptr<CBilboardObject>>& Objects) :
	m_Objects{ move(Objects) }
{
	
}

void CTitleShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>(RESOURCE_TEXTURE2D) };

	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Background.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Button.dds");

	CShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 2, 0);
	CShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	for (const auto& Object : m_Objects)
	{
		Object->SetMaterial(Material);
		Object->SetTexture(Texture);
	}
}

D3D12_INPUT_LAYOUT_DESC CTitleShader::CreateInputLayout()
{
	const UINT InputElementCount{ 6 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "DIRECTION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[3] = { "ROW", 0, DXGI_FORMAT_R32_UINT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[4] = { "COLUMN", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[5] = { "INDEX", 0, DXGI_FORMAT_R32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_DEPTH_STENCIL_DESC CTitleShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC D3D12DepthStencilDesc{};

	D3D12DepthStencilDesc.DepthEnable = false;
	D3D12DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	D3D12DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	D3D12DepthStencilDesc.StencilEnable = false;
	D3D12DepthStencilDesc.StencilReadMask = 0xff;
	D3D12DepthStencilDesc.StencilWriteMask = 0xff;
	D3D12DepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	D3D12DepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return D3D12DepthStencilDesc;
}

D3D12_SHADER_BYTECODE CTitleShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"TitleShader.hlsl", "VS_Title", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CTitleShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"TitleShader.hlsl", "GS_Title", "gs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CTitleShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"TitleShader.hlsl", "PS_Title", "ps_5_1", D3D12ShaderBlob);
}

void CTitleShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature)
{
	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	m_D3D12PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	if (m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

void CTitleShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12GraphicsCommandList->SetDescriptorHeaps(1, m_D3D12CbvSrvUavDescriptorHeap.GetAddressOf());
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());

	for (const auto& Object : m_Objects)
	{
		Object->Render(D3D12GraphicsCommandList, Camera);
	}
}

// ============================================== CPlayerShader ==============================================

CPlayerShader::CPlayerShader(const shared_ptr<CPlayer>& Player) :
	m_Player{ Player }
{

}
	
void CPlayerShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// 플레이어의 메쉬, 재질, 텍스쳐 등 설정
	auto FrameObject{ CObject::LoadGeometryFromFile(D3D12Device, D3D12GraphicsCommandList, "Model/Tank.txt") };
	shared_ptr<CMesh> Mesh{ make_shared<CMesh>() };
	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>(RESOURCE_TEXTURE2D) };

	Mesh->SetBoundingBox(FrameObject.second);
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/TankTexture.dds");

	CShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 1, 0);
	CShader::CreateShaderResourceViews(D3D12Device, Texture.get());
	
	m_Player->SetMesh(Mesh);
	m_Player->SetMaterial(Material);
	m_Player->SetTexture(Texture);
	m_Player->SetChild(FrameObject.first);
	m_Player->OnInitialize();
	m_Player->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList, FrameObject.first.get());
}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	const UINT InputElementCount{ 3 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Lighting", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Lighting", "ps_5_1", D3D12ShaderBlob);
}

void CPlayerShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature)
{
	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	if (m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

void CPlayerShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12GraphicsCommandList->SetDescriptorHeaps(1, m_D3D12CbvSrvUavDescriptorHeap.GetAddressOf());
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());

	if (m_Player)
	{
		m_Player->Render(D3D12GraphicsCommandList, Camera);
	}
}

// ============================================== CBulletShader ==============================================

CBulletShader::CBulletShader(vector<shared_ptr<CBulletObject>>& Bullets) :
	m_Bullets{ Bullets }
{

}

void CBulletShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	auto FrameObject{ CObject::LoadGeometryFromFile(D3D12Device, D3D12GraphicsCommandList, "Model/Cube.txt") };
	shared_ptr<CMesh> Mesh{ make_shared<CMesh>() };
	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>(RESOURCE_TEXTURE2D) };

	FrameObject.first->Scale(0.2f, 0.2f, 0.2f);
	Mesh->SetBoundingBox(FrameObject.second);
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/ExplosionSprite.dds");

	CShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 1, 0);
	CShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	for (const auto& Bullet : m_Bullets)
	{
		shared_ptr<CObject> CopiedFrameObject{ CObject::CopyObject(FrameObject.first) };

		Bullet->SetMesh(Mesh);
		Bullet->SetMaterial(Material);
		Bullet->SetTexture(Texture);
		Bullet->SetChild(CopiedFrameObject);
		Bullet->SetBoundingBox(FrameObject.second);
		Bullet->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList, CopiedFrameObject.get());
	}
}

D3D12_INPUT_LAYOUT_DESC CBulletShader::CreateInputLayout()
{
	const UINT InputElementCount{ 3 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_SHADER_BYTECODE CBulletShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Lighting", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CBulletShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Bullet", "ps_5_1", D3D12ShaderBlob);
}

void CBulletShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature)
{
	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	if (m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

void CBulletShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());

	for (UINT i = 0; i < m_Bullets.size(); ++i)
	{
		if (m_Bullets[i])
		{
			m_Bullets[i]->Render(D3D12GraphicsCommandList, Camera);
		}
	}
}

// ============================================== CObjectShader ==============================================

CObjectShader::CObjectShader(vector<shared_ptr<CObject>>& Objects) :
	m_Objects{ Objects }
{

}

void CObjectShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	auto FrameObject{ CObject::LoadGeometryFromFile(D3D12Device, D3D12GraphicsCommandList, "Model/Tank.txt") };
	shared_ptr<CMesh> Mesh{ make_shared<CMesh>() };
	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>(RESOURCE_TEXTURE2D) };

	Mesh->SetBoundingBox(FrameObject.second);
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/TankTexture2.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Brick.dds");

	CShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 2, 0);
	CShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	for (UINT i = 0; i < MAX_ENEMY; ++i)
	{
		shared_ptr<CObject> CopiedFrameObject{ CObject::CopyObject(FrameObject.first) };

		m_Objects[i]->SetMesh(Mesh);
		m_Objects[i]->SetMaterial(Material);
		m_Objects[i]->SetTexture(Texture);
		m_Objects[i]->SetChild(CopiedFrameObject);
		m_Objects[i]->SetBoundingBox(FrameObject.second);
		m_Objects[i]->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList, CopiedFrameObject.get());
	}

	FrameObject = CObject::LoadGeometryFromFile(D3D12Device, D3D12GraphicsCommandList, "Model/Cube.txt");
	Mesh = make_shared<CMesh>();
	Material = make_shared<CMaterial>();
	Mesh->SetBoundingBox(FrameObject.second);

	for (UINT i = MAX_ENEMY; i < m_Objects.size(); ++i)
	{
		shared_ptr<CObject> CopiedFrameObject{ CObject::CopyObject(FrameObject.first) };

		m_Objects[i]->SetMesh(Mesh);
		m_Objects[i]->SetMaterial(Material);
		m_Objects[i]->SetTexture(Texture);
		m_Objects[i]->SetChild(CopiedFrameObject);
		m_Objects[i]->SetBoundingBox(FrameObject.second);
		m_Objects[i]->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList, CopiedFrameObject.get());
	}
}

D3D12_INPUT_LAYOUT_DESC CObjectShader::CreateInputLayout()
{
	const UINT InputElementCount{ 3 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_SHADER_BYTECODE CObjectShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Lighting", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CObjectShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Object", "ps_5_1", D3D12ShaderBlob);
}

void CObjectShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature)
{
	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	if (m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

shared_ptr<CObject> CObjectShader::PickObjectByRayIntersection(const XMFLOAT3& PickPosition, const XMFLOAT4X4& ViewMatrix, float& NearHitDistance)
{
	float HitDistance{ FLT_MAX };
	shared_ptr<CObject> SelectedObject{};

	NearHitDistance = FLT_MAX;

	for (UINT i = 0; i < m_Objects.size(); ++i)
	{
		UINT Intersected{ m_Objects[i]->PickObjectByRayIntersection(PickPosition, ViewMatrix, HitDistance) };

		if ((Intersected > 0) && (HitDistance < NearHitDistance))
		{
			NearHitDistance = HitDistance;
			SelectedObject = m_Objects[i];
		}
	}

	return SelectedObject;
}

void CObjectShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12GraphicsCommandList->SetDescriptorHeaps(1, m_D3D12CbvSrvUavDescriptorHeap.GetAddressOf());
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());

	for (const auto& Object : m_Objects)
	{
		Object->Render(D3D12GraphicsCommandList, Camera);
	}
}

// ============================================== CTerrainShader ==============================================

CTerrainShader::CTerrainShader(const shared_ptr<CTerrainObject>& Terrain) :
	m_Terrain{ Terrain }
{

}

void CTerrainShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	shared_ptr<CMaterial> Material{ make_shared<CMaterial>(XMFLOAT4(0.2f, 0.8f, 0.15f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)) };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>(RESOURCE_TEXTURE2D) };

	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Terrain_Base_Texture.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Terrain_Detail_Texture0.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Terrain_Detail_Texture1.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Terrain_Detail_Texture2.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Terrain_Alpha_Texture0.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Terrain_Alpha_Texture1.dds");

	CShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 6, 0);
	CShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	m_Terrain->SetMaterial(Material);
	m_Terrain->SetTexture(Texture);
}

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	const UINT InputElementCount{ 4 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[3] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Terrain", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Terrain", "ps_5_1", D3D12ShaderBlob);
}

void CTerrainShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature)
{
	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	if (m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

void CTerrainShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12GraphicsCommandList->SetDescriptorHeaps(1, m_D3D12CbvSrvUavDescriptorHeap.GetAddressOf());
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[1].Get());

	if (m_Terrain)
	{
		m_Terrain->Render(D3D12GraphicsCommandList, Camera);
	}
}

// ============================================== CTessellationTerrainShader ==============================================

CTessellationTerrainShader::CTessellationTerrainShader(const shared_ptr<CTerrainObject>& Terrain) : CTerrainShader(Terrain)
{

}

D3D12_SHADER_BYTECODE CTessellationTerrainShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_TessellationTerrain", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CTessellationTerrainShader::CreateHullShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "HS_TessellationTerrain", "hs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CTessellationTerrainShader::CreateDomainShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "DS_TessellationTerrain", "ds_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CTessellationTerrainShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_TessellationTerrain", "ps_5_1", D3D12ShaderBlob);
}

void CTessellationTerrainShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature)
{
	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	m_D3D12PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	m_D3D12PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	m_D3D12PipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	if (m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

// ============================================== CSkyBoxShader ==============================================

CSkyBoxShader::CSkyBoxShader(const shared_ptr<CSkyBoxObject>& SkyBox) :
	m_SkyBox{ SkyBox }
{

}

void CSkyBoxShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>(RESOURCE_TEXTURE2D) };

	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/SkyBox_Back.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/SkyBox_Front.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/SkyBox_Right.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/SkyBox_Left.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/SkyBox_Top.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/SkyBox_Bottom.dds");

	CShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 6, 0);
	CShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	m_SkyBox->SetMaterial(Material);
	m_SkyBox->SetTexture(Texture);
}

D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout()
{
	const UINT InputElementCount{ 3 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "DIRECTION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC D3D12DepthStencilDesc{};

	D3D12DepthStencilDesc.DepthEnable = false;
	D3D12DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	D3D12DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	D3D12DepthStencilDesc.StencilEnable = false;
	D3D12DepthStencilDesc.StencilReadMask = 0xff;
	D3D12DepthStencilDesc.StencilWriteMask = 0xff;
	D3D12DepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	D3D12DepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return D3D12DepthStencilDesc;
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_SkyBox", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "GS_SkyBox", "gs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_SkyBox", "ps_5_1", D3D12ShaderBlob);
}

void CSkyBoxShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature)
{
	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	m_D3D12PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	if (m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

void CSkyBoxShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12GraphicsCommandList->SetDescriptorHeaps(1, m_D3D12CbvSrvUavDescriptorHeap.GetAddressOf());
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());

	if (m_SkyBox)
	{
		m_SkyBox->Render(D3D12GraphicsCommandList, Camera);
	}
}

// ============================================== CHpBarShader ==============================================

CHpBarShader::CHpBarShader(const shared_ptr<CHpBarObject>& HpBars) :
	m_HpBars{ HpBars }
{

}

D3D12_INPUT_LAYOUT_DESC CHpBarShader::CreateInputLayout()
{
	const UINT InputElementCount{ 3 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "DIRECTION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_SHADER_BYTECODE CHpBarShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Bilboard", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CHpBarShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "GS_HpBar", "gs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CHpBarShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_HpBar", "ps_5_1", D3D12ShaderBlob);
}

void CHpBarShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature)
{
	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	m_D3D12PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	if (m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

void CHpBarShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());

	if (m_HpBars)
	{
		m_HpBars->Render(D3D12GraphicsCommandList, Camera);
	}
}

// ============================================== CBilboardShader ==============================================

CBilboardShader::CBilboardShader(vector<shared_ptr<CBilboardObject>>& Objects) :
	m_Objects{ move(Objects) }
{

}

void CBilboardShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>(RESOURCE_TEXTURE2D) };

	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Tree.dds");

	CShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 1, 0);
	CShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	for (const auto& Object : m_Objects)
	{
		Object->SetMaterial(Material);
		Object->SetTexture(Texture);
	}
}

D3D12_INPUT_LAYOUT_DESC CBilboardShader::CreateInputLayout()
{
	const UINT InputElementCount{ 3 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "DIRECTION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_BLEND_DESC CBilboardShader::CreateBlendState()
{
	D3D12_BLEND_DESC D3D12BlendDesc{};

	D3D12BlendDesc.AlphaToCoverageEnable = true;
	D3D12BlendDesc.IndependentBlendEnable = false;
	D3D12BlendDesc.RenderTarget[0].BlendEnable = true;
	D3D12BlendDesc.RenderTarget[0].LogicOpEnable = false;
	D3D12BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	D3D12BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	D3D12BlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	D3D12BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	D3D12BlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	D3D12BlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	D3D12BlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	D3D12BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return D3D12BlendDesc;
}

D3D12_SHADER_BYTECODE CBilboardShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Bilboard", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CBilboardShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "GS_Bilboard", "gs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CBilboardShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Bilboard", "ps_5_1", D3D12ShaderBlob);
}

void CBilboardShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature)
{
	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	m_D3D12PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	if (m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

void CBilboardShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12GraphicsCommandList->SetDescriptorHeaps(1, m_D3D12CbvSrvUavDescriptorHeap.GetAddressOf());
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());
	
	for (const auto& Object : m_Objects)
	{
		Object->Render(D3D12GraphicsCommandList, Camera);
	}
}

// ============================================== CSpriteBilboardShader ==============================================

CSpriteBilboardShader::CSpriteBilboardShader(vector<shared_ptr<CBilboardObject>>& Objects) :
	m_Objects{ move(Objects) }
{

}

void CSpriteBilboardShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>(RESOURCE_TEXTURE2D) };

	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/HitSprite.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/ExplosionSprite.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/SmokeParticle.dds");

	CShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 3, 0);
	CShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	for (const auto& Object : m_Objects)
	{
		Object->SetMaterial(Material);
		Object->SetTexture(Texture);
	}
}

D3D12_INPUT_LAYOUT_DESC CSpriteBilboardShader::CreateInputLayout()
{
	const UINT InputElementCount{ 6 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "DIRECTION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[3] = { "ROW", 0, DXGI_FORMAT_R32_UINT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[4] = { "COLUMN", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[5] = { "TIME", 0, DXGI_FORMAT_R32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_BLEND_DESC CSpriteBilboardShader::CreateBlendState()
{
	D3D12_BLEND_DESC D3D12BlendDesc{};

	D3D12BlendDesc.AlphaToCoverageEnable = true;
	D3D12BlendDesc.IndependentBlendEnable = false;
	D3D12BlendDesc.RenderTarget[0].BlendEnable = true;
	D3D12BlendDesc.RenderTarget[0].LogicOpEnable = false;
	D3D12BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	D3D12BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	D3D12BlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	D3D12BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	D3D12BlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	D3D12BlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	D3D12BlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	D3D12BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return D3D12BlendDesc;
}

D3D12_SHADER_BYTECODE CSpriteBilboardShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_SpriteBilboard", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CSpriteBilboardShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "GS_SpriteBilboard", "gs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CSpriteBilboardShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_SpriteBilboard", "ps_5_1", D3D12ShaderBlob);
}

void CSpriteBilboardShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature)
{
	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	m_D3D12PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	if (m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

void CSpriteBilboardShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12GraphicsCommandList->SetDescriptorHeaps(1, m_D3D12CbvSrvUavDescriptorHeap.GetAddressOf());
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());

	for (const auto& Object : m_Objects)
	{
		Object->Render(D3D12GraphicsCommandList, Camera);
	}
}

// ============================================== CMirrorShader ==============================================

CMirrorShader::CMirrorShader(const shared_ptr<CObject>& Mirror) :
	m_Mirror{ Mirror }
{

}

void CMirrorShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	shared_ptr<CRectMesh> Mesh{ make_shared<CRectMesh>(D3D12Device, D3D12GraphicsCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f) };
	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>(RESOURCE_TEXTURE2D) };

	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Mirror.dds");

	CShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 1, 0);
	CShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	m_Mirror->SetMesh(Mesh);
	m_Mirror->SetMaterial(Material);
	m_Mirror->SetTexture(Texture);
}

D3D12_INPUT_LAYOUT_DESC CMirrorShader::CreateInputLayout()
{
	const UINT InputElementCount{ 3 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_DEPTH_STENCIL_DESC CMirrorShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC D3D12DepthStencilDesc{};

	D3D12DepthStencilDesc.DepthEnable = true;
	D3D12DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	D3D12DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	D3D12DepthStencilDesc.StencilEnable = true;
	D3D12DepthStencilDesc.StencilReadMask = 0xff;
	D3D12DepthStencilDesc.StencilWriteMask = 0xff;
	D3D12DepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	D3D12DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	D3D12DepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	D3D12DepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return D3D12DepthStencilDesc;
}

D3D12_SHADER_BYTECODE CMirrorShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Lighting", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CMirrorShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Lighting", "ps_5_1", D3D12ShaderBlob);
}

void CMirrorShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature)
{
	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	m_D3D12PipelineStateDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = 0; // 렌더타겟을 변경하지 않음
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	m_D3D12PipelineStateDesc.RasterizerState.FrontCounterClockwise = true;
	m_D3D12PipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	m_D3D12PipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	m_D3D12PipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	CShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
	m_D3D12PipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	m_D3D12PipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	m_D3D12PipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	m_D3D12PipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = true;
	m_D3D12PipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	m_D3D12PipelineStateDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&m_D3D12PipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	if (m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] m_D3D12PipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

void CMirrorShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT PSONum)
{
	if (PSONum == 2)
	{
		D3D12GraphicsCommandList->SetDescriptorHeaps(1, m_D3D12CbvSrvUavDescriptorHeap.GetAddressOf());
	}

	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[PSONum].Get());
}
