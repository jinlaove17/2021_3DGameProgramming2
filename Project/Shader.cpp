#include "stdafx.h"
#include "Shader.h"

// ============================================== CShader ==============================================

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
// ============================================== CShader ==============================================

D3D12_SHADER_BYTECODE CComputeShader::CreateComputeShader(ID3DBlob* D3D12ShaderBlob)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

void CComputeShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, const XMUINT3& ThreadGroups)
{
	D3D12_CACHED_PIPELINE_STATE D3D12CachedPipelineState{};
	D3D12_COMPUTE_PIPELINE_STATE_DESC D3D12ComputePipelineStateDesc{};
	ComPtr<ID3DBlob> D3D12ComputeShaderBlob{};

	D3D12ComputePipelineStateDesc.pRootSignature = D3D12RootSignature;
	D3D12ComputePipelineStateDesc.CS = CreateComputeShader(D3D12ComputeShaderBlob.Get());
	D3D12ComputePipelineStateDesc.NodeMask = 0;
	D3D12ComputePipelineStateDesc.CachedPSO = D3D12CachedPipelineState;
	D3D12ComputePipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	m_D3D12PipelineStates.emplace_back();
	ThrowIfFailed(D3D12Device->CreateComputePipelineState(&D3D12ComputePipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	m_ThreadGroups = ThreadGroups;
}

void CComputeShader::Dispatch(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());
	D3D12GraphicsCommandList->Dispatch(m_ThreadGroups.x, m_ThreadGroups.y, m_ThreadGroups.z);
}

void CComputeShader::Dispatch(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const XMUINT3& ThreadGroups)
{
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());
	D3D12GraphicsCommandList->Dispatch(ThreadGroups.x, ThreadGroups.y, ThreadGroups.z);
}

// ============================================== CGraphicsShader ==============================================

void CGraphicsShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	return;
}

void CGraphicsShader::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	return;
}

void CGraphicsShader::ReleaseShaderVariables()
{
	return;
}

D3D12_INPUT_LAYOUT_DESC CGraphicsShader::CreateInputLayout(UINT PSONum)
{
	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	return D3D12InputLayoutDesc;
}

D3D12_RASTERIZER_DESC CGraphicsShader::CreateRasterizerState(UINT PSONum)
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

D3D12_BLEND_DESC CGraphicsShader::CreateBlendState(UINT PSONum)
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

D3D12_DEPTH_STENCIL_DESC CGraphicsShader::CreateDepthStencilState(UINT PSONum)
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

D3D12_STREAM_OUTPUT_DESC CGraphicsShader::CreateStreamOutputState(UINT PSONum)
{
	D3D12_STREAM_OUTPUT_DESC D3D12StreamOutputDesc{};

	return D3D12StreamOutputDesc;
}

D3D12_SHADER_BYTECODE CGraphicsShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_SHADER_BYTECODE CGraphicsShader::CreateHullShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_SHADER_BYTECODE CGraphicsShader::CreateDomainShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_SHADER_BYTECODE CGraphicsShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_SHADER_BYTECODE CGraphicsShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE CGraphicsShader::GetPrimitiveType(UINT PSONum)
{
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
}

void CGraphicsShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC D3D12GraphicsPipelineState{};
	ComPtr<ID3DBlob> D3D12VertexShaderBlob{}, D3D12HullShaderBlob{}, D3D12DomainShaderBlob{}, D3D12GeometryShaderBlob{}, D3D12PixelShaderBlob{};

	D3D12GraphicsPipelineState.pRootSignature = D3D12RootSignature;
	D3D12GraphicsPipelineState.VS = CreateVertexShader(D3D12VertexShaderBlob.Get(), PSONum);
	D3D12GraphicsPipelineState.HS = CreateHullShader(D3D12HullShaderBlob.Get(), PSONum);
	D3D12GraphicsPipelineState.DS = CreateDomainShader(D3D12DomainShaderBlob.Get(), PSONum);
	D3D12GraphicsPipelineState.GS = CreateGeometryShader(D3D12GeometryShaderBlob.Get(), PSONum);
	D3D12GraphicsPipelineState.PS = CreatePixelShader(D3D12PixelShaderBlob.Get(), PSONum);
	D3D12GraphicsPipelineState.RasterizerState = CreateRasterizerState(PSONum);
	D3D12GraphicsPipelineState.BlendState = CreateBlendState(PSONum);
	D3D12GraphicsPipelineState.DepthStencilState = CreateDepthStencilState(PSONum);
	D3D12GraphicsPipelineState.StreamOutput = CreateStreamOutputState(PSONum);
	D3D12GraphicsPipelineState.InputLayout = CreateInputLayout(PSONum);
	D3D12GraphicsPipelineState.SampleMask = UINT_MAX;
	D3D12GraphicsPipelineState.PrimitiveTopologyType = GetPrimitiveType(PSONum);
	D3D12GraphicsPipelineState.NumRenderTargets = 1;
	D3D12GraphicsPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D12GraphicsPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	D3D12GraphicsPipelineState.SampleDesc.Count = 1;
	D3D12GraphicsPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	m_D3D12PipelineStates.emplace_back();
	ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&D3D12GraphicsPipelineState, __uuidof(ID3D12PipelineState), (void**)(--m_D3D12PipelineStates.end())->GetAddressOf()));

	if (D3D12GraphicsPipelineState.InputLayout.pInputElementDescs)
	{
		delete[] D3D12GraphicsPipelineState.InputLayout.pInputElementDescs;
	}
}

void CGraphicsShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	return;
}

shared_ptr<CObject> CGraphicsShader::PickObjectByRayIntersection(const XMFLOAT3& PickPosition, const XMFLOAT4X4& ViewMatrix, float& NearHitDistance)
{
	return nullptr;
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

	CGraphicsShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 2, 0);
	CGraphicsShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	for (const auto& Object : m_Objects)
	{
		Object->SetMaterial(Material);
		Object->SetTexture(Texture);
	}
}

D3D12_INPUT_LAYOUT_DESC CTitleShader::CreateInputLayout(UINT PSONum)
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

D3D12_DEPTH_STENCIL_DESC CTitleShader::CreateDepthStencilState(UINT PSONum)
{
	D3D12_DEPTH_STENCIL_DESC D3D12DepthStencilDesc{ CGraphicsShader::CreateDepthStencilState(PSONum) };

	D3D12DepthStencilDesc.DepthEnable = false;

	return D3D12DepthStencilDesc;
}

D3D12_SHADER_BYTECODE CTitleShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"TitleShader.hlsl", "VS_Title", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CTitleShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"TitleShader.hlsl", "GS_Title", "gs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CTitleShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"TitleShader.hlsl", "PS_Title", "ps_5_1", D3D12ShaderBlob);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE CTitleShader::GetPrimitiveType(UINT PSONum)
{
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
}

void CTitleShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
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
	auto FrameObject{ CObject::LoadGeometryFromFile(D3D12Device, D3D12GraphicsCommandList, "Model/Tank.txt") };
	shared_ptr<CMesh> Mesh{ make_shared<CMesh>() };
	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>(RESOURCE_TEXTURE2D) };

	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/TankTexture.dds");

	CGraphicsShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 1, 0);
	CGraphicsShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	Mesh->SetBoundingBox(FrameObject.second);	
	m_Player->SetMesh(Mesh);
	m_Player->SetMaterial(Material);
	m_Player->SetTexture(Texture);
	m_Player->SetChild(FrameObject.first);
	m_Player->OnInitialize();
	m_Player->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList, FrameObject.first.get());
}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout(UINT PSONum)
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

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Lighting", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Lighting", "ps_5_1", D3D12ShaderBlob);
}

void CPlayerShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
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

	CGraphicsShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 1, 0);
	CGraphicsShader::CreateShaderResourceViews(D3D12Device, Texture.get());

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

D3D12_INPUT_LAYOUT_DESC CBulletShader::CreateInputLayout(UINT PSONum)
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

D3D12_SHADER_BYTECODE CBulletShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Lighting", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CBulletShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Bullet", "ps_5_1", D3D12ShaderBlob);
}

void CBulletShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
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

	CGraphicsShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 1, 0);
	CGraphicsShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	for (const auto& Object : m_Objects)
	{
		shared_ptr<CObject> CopiedFrameObject{ CObject::CopyObject(FrameObject.first) };

		Object->SetMesh(Mesh);
		Object->SetMaterial(Material);
		Object->SetTexture(Texture);
		Object->SetChild(CopiedFrameObject);
		Object->SetBoundingBox(FrameObject.second);
		Object->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList, CopiedFrameObject.get());
	}
}

D3D12_INPUT_LAYOUT_DESC CObjectShader::CreateInputLayout(UINT PSONum)
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

D3D12_SHADER_BYTECODE CObjectShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Lighting", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CObjectShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Lighting", "ps_5_1", D3D12ShaderBlob);
}

void CObjectShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
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

	CGraphicsShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 6, 0);
	CGraphicsShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	m_Terrain->SetMaterial(Material);
	m_Terrain->SetTexture(Texture);
}

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout(UINT PSONum)
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

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Terrain", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Terrain", "ps_5_1", D3D12ShaderBlob);
}

void CTerrainShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
}

void CTerrainShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12GraphicsCommandList->SetDescriptorHeaps(1, m_D3D12CbvSrvUavDescriptorHeap.GetAddressOf());

#ifdef TERRAIN_TESSELLATION
	D3D12GraphicsCommandList->SetPipelineState(IsSolidTerrain ? m_D3D12PipelineStates[0].Get() : m_D3D12PipelineStates[1].Get());
#else
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());
#endif

	if (m_Terrain)
	{
		m_Terrain->Render(D3D12GraphicsCommandList, Camera);
	}
}

// ============================================== CTessellationTerrainShader ==============================================

CTessellationTerrainShader::CTessellationTerrainShader(const shared_ptr<CTerrainObject>& Terrain) : CTerrainShader(Terrain)
{

}

D3D12_RASTERIZER_DESC CTessellationTerrainShader::CreateRasterizerState(UINT PSONum)
{
	D3D12_RASTERIZER_DESC D3D12RasterizerDesc{ CGraphicsShader::CreateRasterizerState(PSONum) };

	switch (PSONum)
	{
	case 1:
		D3D12RasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
		return D3D12RasterizerDesc;
	default:
		return D3D12RasterizerDesc;
	}
}

D3D12_SHADER_BYTECODE CTessellationTerrainShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_TessellationTerrain", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CTessellationTerrainShader::CreateHullShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "HS_TessellationTerrain", "hs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CTessellationTerrainShader::CreateDomainShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "DS_TessellationTerrain", "ds_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CTessellationTerrainShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_TessellationTerrain", "ps_5_1", D3D12ShaderBlob);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE CTessellationTerrainShader::GetPrimitiveType(UINT PSONum)
{
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
}

void CTessellationTerrainShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature, 0);
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature, 1);
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

	CGraphicsShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 6, 0);
	CGraphicsShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	m_SkyBox->SetMaterial(Material);
	m_SkyBox->SetTexture(Texture);
}

D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout(UINT PSONum)
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

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState(UINT PSONum)
{
	D3D12_DEPTH_STENCIL_DESC D3D12DepthStencilDesc{ CGraphicsShader::CreateDepthStencilState(PSONum) };

	D3D12DepthStencilDesc.DepthEnable = false;
	D3D12DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	D3D12DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;

	return D3D12DepthStencilDesc;
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_SkyBox", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "GS_SkyBox", "gs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_SkyBox", "ps_5_1", D3D12ShaderBlob);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE CSkyBoxShader::GetPrimitiveType(UINT PSONum)
{
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
}

void CSkyBoxShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
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

D3D12_INPUT_LAYOUT_DESC CHpBarShader::CreateInputLayout(UINT PSONum)
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

D3D12_SHADER_BYTECODE CHpBarShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Bilboard", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CHpBarShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "GS_HpBar", "gs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CHpBarShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_HpBar", "ps_5_1", D3D12ShaderBlob);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE CHpBarShader::GetPrimitiveType(UINT PSONum)
{
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
}

void CHpBarShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
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

	CGraphicsShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 1, 0);
	CGraphicsShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	for (const auto& Object : m_Objects)
	{
		Object->SetMaterial(Material);
		Object->SetTexture(Texture);
	}
}

D3D12_INPUT_LAYOUT_DESC CBilboardShader::CreateInputLayout(UINT PSONum)
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

D3D12_BLEND_DESC CBilboardShader::CreateBlendState(UINT PSONum)
{
	D3D12_BLEND_DESC D3D12BlendDesc{ CGraphicsShader::CreateBlendState(PSONum) };

	D3D12BlendDesc.AlphaToCoverageEnable = true;
	D3D12BlendDesc.RenderTarget[0].BlendEnable = true;
	D3D12BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	D3D12BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	return D3D12BlendDesc;
}

D3D12_SHADER_BYTECODE CBilboardShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Bilboard", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CBilboardShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "GS_Bilboard", "gs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CBilboardShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Bilboard", "ps_5_1", D3D12ShaderBlob);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE CBilboardShader::GetPrimitiveType(UINT PSONum)
{
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
}

void CBilboardShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
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

	CGraphicsShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 3, 0);
	CGraphicsShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	for (const auto& Object : m_Objects)
	{
		Object->SetMaterial(Material);
		Object->SetTexture(Texture);
	}
}

D3D12_INPUT_LAYOUT_DESC CSpriteBilboardShader::CreateInputLayout(UINT PSONum)
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

D3D12_BLEND_DESC CSpriteBilboardShader::CreateBlendState(UINT PSONum)
{
	D3D12_BLEND_DESC D3D12BlendDesc{ CGraphicsShader::CreateBlendState(PSONum) };

	D3D12BlendDesc.AlphaToCoverageEnable = true;
	D3D12BlendDesc.RenderTarget[0].BlendEnable = true;
	D3D12BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	D3D12BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	return D3D12BlendDesc;
}

D3D12_SHADER_BYTECODE CSpriteBilboardShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_SpriteBilboard", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CSpriteBilboardShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "GS_SpriteBilboard", "gs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CSpriteBilboardShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_SpriteBilboard", "ps_5_1", D3D12ShaderBlob);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE CSpriteBilboardShader::GetPrimitiveType(UINT PSONum)
{
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
}

void CSpriteBilboardShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
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

CMirrorShader::CMirrorShader(const shared_ptr<CPlayer>& Player, vector<shared_ptr<CObject>>& Objects) :
	m_Player{ Player },
	m_InsideObjects{ Objects }
{
	// 거울 객체를 생성한다.
	m_Mirror = make_shared<CMirrorObject>();
	m_Mirror->SetPosition(XMFLOAT3(-40.0f, 12.0f, 40.0f));

	// 거울 뒷면 객체를 생성한다.
	m_MirrorBack = make_shared<CMirrorBackObject>();
	m_MirrorBack->SetPosition(XMFLOAT3(-40.0f, 20.0f, 40.0f));
}

void CMirrorShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// 거울 객체의 메쉬, 재질, 텍스처 설정
	shared_ptr<CMesh> Mesh{ make_shared<CRectMesh>(D3D12Device, D3D12GraphicsCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f) };
	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>(RESOURCE_TEXTURE2D) };

	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Mirror.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/TankTexture.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/Brick.dds");
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/WoodBox.dds");

	CGraphicsShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 4, 0);
	CGraphicsShader::CreateShaderResourceViews(D3D12Device, Texture.get());

	m_Mirror->SetMesh(Mesh);
	m_Mirror->SetMaterial(Material);
	m_Mirror->SetTexture(Texture);

	Mesh = make_shared<CRectMesh>(D3D12Device, D3D12GraphicsCommandList, 40.0f, 40.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	m_MirrorBack->SetMesh(Mesh);
	m_MirrorBack->SetMaterial(Material);
	m_MirrorBack->SetTexture(Texture);

	// 플레이어 객체의 메쉬, 재질, 텍스처 설정
	auto FrameObject{ CObject::LoadGeometryFromFile(D3D12Device, D3D12GraphicsCommandList, "Model/Tank.txt") };

	Mesh = make_shared<CMesh>();
	Material = make_shared<CMaterial>();
	Mesh->SetBoundingBox(FrameObject.second);

	m_Player->SetMesh(Mesh);
	m_Player->SetMaterial(Material);
	m_Player->SetTexture(Texture);
	m_Player->SetChild(FrameObject.first);
	m_Player->OnInitialize();
	m_Player->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList, FrameObject.first.get());

	// 실내 객체(벽, 상자)의 메쉬, 재질, 텍스처 설정
	FrameObject = CObject::LoadGeometryFromFile(D3D12Device, D3D12GraphicsCommandList, "Model/Cube.txt");
	Mesh = make_shared<CMesh>();
	Material = make_shared<CMaterial>();
	Mesh->SetBoundingBox(FrameObject.second);

	for (UINT i = 0; i < m_InsideObjects.size(); ++i)
	{
		shared_ptr<CObject> CopiedFrameObject{ CObject::CopyObject(FrameObject.first) };

		m_InsideObjects[i]->SetMesh(Mesh);
		m_InsideObjects[i]->SetMaterial(Material);
		m_InsideObjects[i]->SetTexture(Texture);
		m_InsideObjects[i]->SetChild(CopiedFrameObject);
		m_InsideObjects[i]->SetBoundingBox(FrameObject.second);
		m_InsideObjects[i]->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList, CopiedFrameObject.get());
	}
}

D3D12_RASTERIZER_DESC CMirrorShader::CreateRasterizerState(UINT PSONum)
{
	D3D12_RASTERIZER_DESC D3D12RasterizerDesc{ CGraphicsShader::CreateRasterizerState(PSONum) };

	switch (PSONum)
	{
	case 3:
		D3D12RasterizerDesc.FrontCounterClockwise = true;
		break;
	}

	return D3D12RasterizerDesc;
}

D3D12_BLEND_DESC CMirrorShader::CreateBlendState(UINT PSONum)
{
	D3D12_BLEND_DESC D3D12BlendDesc{ CGraphicsShader::CreateBlendState(PSONum) };

	switch (PSONum)
	{
	case 2:
		// 렌더타겟을 변경하지 않음
		D3D12BlendDesc.RenderTarget[0].RenderTargetWriteMask = 0;
		break;
	case 4:
		D3D12BlendDesc.RenderTarget[0].BlendEnable = true;
		D3D12BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		D3D12BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		break;
	}

	return D3D12BlendDesc;
}

D3D12_DEPTH_STENCIL_DESC CMirrorShader::CreateDepthStencilState(UINT PSONum)
{
	D3D12_DEPTH_STENCIL_DESC D3D12DepthStencilDesc{ CGraphicsShader::CreateDepthStencilState(PSONum) };

	switch (PSONum)
	{
	case 1:
		D3D12DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	case 2:
		D3D12DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		D3D12DepthStencilDesc.StencilEnable = true;
		D3D12DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		D3D12DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		D3D12DepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		D3D12DepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		break;
	case 3:
	case 4:
		D3D12DepthStencilDesc.StencilEnable = true;
		D3D12DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		D3D12DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
		break;
	}

	return D3D12DepthStencilDesc;
}

D3D12_INPUT_LAYOUT_DESC CMirrorShader::CreateInputLayout(UINT PSONum)
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

D3D12_SHADER_BYTECODE CMirrorShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Lighting", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CMirrorShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_Lighting", "ps_5_1", D3D12ShaderBlob);
}

void CMirrorShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature, 0);
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature, 1);
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature, 2);
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature, 3);
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature, 4);
}

void CMirrorShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	// 플레이어 객체를 렌더링한다.
	D3D12GraphicsCommandList->SetDescriptorHeaps(1, m_D3D12CbvSrvUavDescriptorHeap.GetAddressOf());
	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());
	
	m_Player->Render(D3D12GraphicsCommandList, m_Player->GetCamera());

	if (IsInside)
	{
		// 실내 겍체(벽, 상자)를 렌더링한다.
		for (const auto& Object : m_InsideObjects)
		{
			Object->Render(D3D12GraphicsCommandList, m_Player->GetCamera());
		}

		// 거울 뒷면 객체를 렌더링한다.
		D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[1].Get());

		m_MirrorBack->Render(D3D12GraphicsCommandList, m_Player->GetCamera());

		// 거울을 스텐실 버퍼에 렌더링한다.
		// 이때, 렌더 타겟에는 출력하지 않는다.
		D3D12GraphicsCommandList->OMSetStencilRef(1);
		D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[2].Get());

		m_Mirror->Render(D3D12GraphicsCommandList, m_Player->GetCamera());

		// 거울에 반사된 객체들을 렌더링한다.
		// 이때, 평면의 방정식은 0 * (x + 40) + 0 * (y - 20) - 1 * (z - 40) = 0이다.
		XMVECTOR MirrorPlane{ 0.0f, 0.0f, -1.0f, 40.0f };
		XMMATRIX XMReflectMatrix{ XMMatrixReflect(MirrorPlane) };
		XMFLOAT4X4 TransformMatrix{ m_Player->m_TransformMatrix };
		XMFLOAT4X4 ReflectMatrix{};

		XMStoreFloat4x4(&ReflectMatrix, XMReflectMatrix);

		// 스텐실 버퍼를 갱신할 때, 버퍼의 값을 참조값인 1로 만든다.
		D3D12GraphicsCommandList->OMSetStencilRef(1);
		D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[3].Get());

		// 반사된 플레이어 객체를 렌더링한 후, 변환 행렬을 원래대로 되돌려 놓는다.
		// 처음에는 CBV로 Map() 함수를 통해 수행했었는데, 마지막으로 호출된 값만 쉐이더로 넘어가기 때문에 32BitConstants로 구조를 변경하였다.
		m_Player->m_TransformMatrix = Matrix4x4::Multiply(m_Player->m_TransformMatrix, ReflectMatrix);
		m_Player->Render(D3D12GraphicsCommandList, m_Player->GetCamera());
		m_Player->m_TransformMatrix = TransformMatrix;
		m_Player->UpdateTransform(Matrix4x4::Identity());

		// 반사된 실내(벽, 상자) 객체를 렌더링하고, 변환 행렬을 원래대로 되돌려 놓는다.
		for (int i = 0 ; i < m_InsideObjects.size(); ++i)
		{
			TransformMatrix = m_InsideObjects[i]->m_TransformMatrix;

			m_InsideObjects[i]->m_TransformMatrix = Matrix4x4::Multiply(m_InsideObjects[i]->m_TransformMatrix, ReflectMatrix);
			m_InsideObjects[i]->Render(D3D12GraphicsCommandList, m_Player->GetCamera());
			m_InsideObjects[i]->m_TransformMatrix = TransformMatrix;
			m_InsideObjects[i]->UpdateTransform(Matrix4x4::Identity());
		}

		// 거울 객체를 블렌딩하여 렌더링한다.
		D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[4].Get());

		m_Mirror->Render(D3D12GraphicsCommandList, m_Player->GetCamera());
	}
}

// ============================================== CParticleShader ==============================================

void CParticleShader::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	shared_ptr<CMesh> Mesh{ make_shared<CParticleMesh>(D3D12Device, D3D12GraphicsCommandList) };
	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>(RESOURCE_TEXTURE2D) };

	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, L"Image/SnowParticle.dds");

	m_Particles = make_shared<CParticleObject>(D3D12Device, D3D12GraphicsCommandList);
	m_Particles->SetMesh(Mesh);
	m_Particles->SetMaterial(Material);
	m_Particles->SetTexture(Texture);

	CGraphicsShader::CreateCbvSrvUavDescriptorHeaps(D3D12Device, 0, 1, 0);
	CGraphicsShader::CreateShaderResourceViews(D3D12Device, Texture.get());
}

D3D12_INPUT_LAYOUT_DESC CParticleShader::CreateInputLayout(UINT PSONum)
{
	const UINT InputElementCount{ 4 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "DIRECTION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[3] = { "LEVEL", 0, DXGI_FORMAT_R32_UINT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_BLEND_DESC CParticleShader::CreateBlendState(UINT PSONum)
{
	D3D12_BLEND_DESC D3D12BlendDesc{ CGraphicsShader::CreateBlendState(PSONum) };

	D3D12BlendDesc.AlphaToCoverageEnable = true;
	D3D12BlendDesc.RenderTarget[0].BlendEnable = true;
	D3D12BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	D3D12BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	return D3D12BlendDesc;
}

D3D12_STREAM_OUTPUT_DESC CParticleShader::CreateStreamOutputState(UINT PSONum)
{
	D3D12_STREAM_OUTPUT_DESC D3D12StreamOutputDesc{};

	switch (PSONum)
	{
	case 0:
	{
		UINT* BufferStrides{ new UINT[1]{} };
		BufferStrides[0] = sizeof(CParticleVertex);

		const UINT EntryCount{ 4 };
		D3D12_SO_DECLARATION_ENTRY* D3D12DeclEntries{ new D3D12_SO_DECLARATION_ENTRY[EntryCount]{} };

		D3D12DeclEntries[0] = { 0, "POSITION", 0, 0, 3, 0 };
		D3D12DeclEntries[1] = { 0, "DIRECTION", 0, 0, 3, 0 };
		D3D12DeclEntries[2] = { 0, "SIZE", 0, 0, 2, 0 };
		D3D12DeclEntries[3] = { 0, "LEVEL", 0, 0, 1, 0 };

		D3D12StreamOutputDesc.NumEntries = EntryCount;
		D3D12StreamOutputDesc.pSODeclaration = D3D12DeclEntries;
		D3D12StreamOutputDesc.NumStrides = 1;
		D3D12StreamOutputDesc.pBufferStrides = BufferStrides;
		D3D12StreamOutputDesc.RasterizedStream = D3D12_SO_NO_RASTERIZED_STREAM;
	}
	break;
	}

	return D3D12StreamOutputDesc;
}

D3D12_SHADER_BYTECODE CParticleShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	switch (PSONum)
	{
	case 0:
	case 1:
		return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_Particle", "vs_5_1", D3D12ShaderBlob);
	default:
		return CGraphicsShader::CreateVertexShader(D3D12ShaderBlob, PSONum);
	}
}

D3D12_SHADER_BYTECODE CParticleShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	switch (PSONum)
	{
	case 0:
		return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "GS_ParticleStreamOutput", "gs_5_1", D3D12ShaderBlob);
	case 1:
		return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "GS_ParticleDraw", "gs_5_1", D3D12ShaderBlob);
	default:
		return CGraphicsShader::CreateGeometryShader(D3D12ShaderBlob, PSONum);
	}
}

D3D12_SHADER_BYTECODE CParticleShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_ParticleDraw", "ps_5_1", D3D12ShaderBlob);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE CParticleShader::GetPrimitiveType(UINT PSONum)
{
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
}

void CParticleShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	// 스트림 출력용 PSO
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature, 0);

	// 그리기용 PSO
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature, 1);
}

void CParticleShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12GraphicsCommandList->SetDescriptorHeaps(1, m_D3D12CbvSrvUavDescriptorHeap.GetAddressOf());

	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[0].Get());
	m_Particles->PreRender(D3D12GraphicsCommandList);

	D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[1].Get());
	m_Particles->Render(D3D12GraphicsCommandList, Camera);
}
