#include "stdafx.h"
#include "Object.h"

// ======================================================= CTexture =======================================================

CTexture::CTexture(UINT TextureType) :
	m_Type{ TextureType }
{

}

void CTexture::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT RootParameterNum, UINT GpuHandleIndex)
{
	D3D12GraphicsCommandList->SetGraphicsRootDescriptorTable(RootParameterNum, m_D3D12SrvGpuDescriptorHandles[GpuHandleIndex]);
}

void CTexture::ReleaseShaderVariables()
{

}

void CTexture::ReleaseUploadBuffers()
{
	for (UINT i = 0; i < m_D3D12TextureUploadBuffers.size(); ++i)
	{
		if (m_D3D12TextureUploadBuffers[i])
		{
			m_D3D12TextureUploadBuffers[i]->Release();
		}

		m_D3D12TextureUploadBuffers[i] = nullptr;
	}
}

void CTexture::CreateTexture(ID3D12Device* D3D12Device, const UINT64& Width, UINT Height, D3D12_RESOURCE_STATES D3D12ResourceStates, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DxgiFormat, const D3D12_CLEAR_VALUE& D3D12ClearValue)
{
	m_D3D12Textures.push_back(CreateTexture2DResource(D3D12Device, Width, Height, 1, 0, D3D12ResourceStates, D3D12ResourceFlags, DxgiFormat, D3D12ClearValue));
}

void CTexture::LoadTextureFromDDSFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const wchar_t* FileName)
{
	m_D3D12TextureUploadBuffers.emplace_back();
	m_D3D12Textures.push_back(CreateTextureResourceFromDDSFile(D3D12Device, D3D12GraphicsCommandList, FileName, D3D12_RESOURCE_STATE_GENERIC_READ, (--m_D3D12TextureUploadBuffers.end())->GetAddressOf()));
}

UINT CTexture::GetTextureType() const
{
	return m_Type;
}

UINT CTexture::GetTextureCount() const
{
	return (UINT)m_D3D12Textures.size();
}

ID3D12Resource* CTexture::GetResource(UINT Index)
{
	return m_D3D12Textures[Index].Get();
}

void CTexture::SetGpuDescriptorHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& D3D12SrvGpuDescriptorHandle)
{
	m_D3D12SrvGpuDescriptorHandles.push_back(D3D12SrvGpuDescriptorHandle);
}

const D3D12_GPU_DESCRIPTOR_HANDLE& CTexture::GetGpuDescriptorHandle(UINT Index) const
{
	return m_D3D12SrvGpuDescriptorHandles[Index];
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(UINT Index)
{
	ID3D12Resource* ShaderResource{ GetResource(Index) };
	D3D12_RESOURCE_DESC D3D12ResourceDesc{ ShaderResource->GetDesc() };
	D3D12_SHADER_RESOURCE_VIEW_DESC D3D12ShaderResourceViewDesc{};

	D3D12ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch (m_Type)
	{
	case RESOURCE_TEXTURE2D: // (D3D12ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D), (D3D12ResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: // []
		D3D12ShaderResourceViewDesc.Format = D3D12ResourceDesc.Format;
		D3D12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		D3D12ShaderResourceViewDesc.Texture2D.MipLevels = -1;
		D3D12ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		D3D12ShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		D3D12ShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: // (D3D12ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D), (D3D12ResourceDesc.DepthOrArraySize != 1)
		D3D12ShaderResourceViewDesc.Format = D3D12ResourceDesc.Format;
		D3D12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		D3D12ShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		D3D12ShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		D3D12ShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		D3D12ShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		D3D12ShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		D3D12ShaderResourceViewDesc.Texture2DArray.ArraySize = D3D12ResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: // (D3D12ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D), (D3D12ResourceDesc.DepthOrArraySize == 6)
		D3D12ShaderResourceViewDesc.Format = D3D12ResourceDesc.Format;
		D3D12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		D3D12ShaderResourceViewDesc.TextureCube.MipLevels = 1;
		D3D12ShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		D3D12ShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	}

	return D3D12ShaderResourceViewDesc;
}

// ======================================================= CMaterial =======================================================

CMaterial::CMaterial(const XMFLOAT4& Albedo, const XMFLOAT4& Specular, const XMFLOAT4& Ambient, const XMFLOAT4& Emissive)
{
	m_Color.m_Albedo = Albedo;
	m_Color.m_Specular = Specular;
	m_Color.m_Ambient = Ambient;
	m_Color.m_Emissive = Emissive;
}

// ======================================================= CObject =======================================================

CObject::CObject(const CObject& Rhs)
{
	m_FrameName = Rhs.m_FrameName;
	m_TransformMatrix = Rhs.m_TransformMatrix;
	m_WorldMatrix = Matrix4x4::Identity();

	if (Rhs.m_Mesh)
	{
		m_Mesh = Rhs.m_Mesh;
	}

	if (Rhs.m_Material)
	{
		m_Material = Rhs.m_Material;
	}
	
	m_Child = nullptr;
	m_Sibling = nullptr;
}

pair<shared_ptr<CObject>, BoundingOrientedBox> CObject::LoadGeometryFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const string& FileName)
{
	ifstream InFile{ FileName };
	string Word{};
	shared_ptr<CObject> Object{};

	while (InFile >> Word)
	{
		if (!Word.compare("<Hierarchy>:"))
		{
			cout << FileName << " 로드 중..." << endl;
			Object = CObject::LoadFrameHierarchyFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
		}
		else if (!Word.compare("</Hierarchy>"))
		{
			cout << FileName << " 로드 완료!" << endl;
			break;
		}
	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR DebugMessage[256]{};
	_stprintf_s(DebugMessage, 256, _T("\nFrame Hierarchy(%s)\n"), InFile);
	OutputDebugString(DebugMessage);
	CObject::PrintFrameInfo(Object, NULL);
#endif

	return make_pair(Object->m_Child, Object->GetBoundingBox());
}

shared_ptr<CObject> CObject::LoadFrameHierarchyFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ifstream& InFile)
{
	string Word{};
	shared_ptr<CObject> Object{};

	while (InFile >> Word)
	{
		if (!Word.compare("<Frame>:"))
		{
			Object = make_shared<CObject>();

			InFile >> Object->m_FrameName;
		}
		else if (!Word.compare("<Transform>:"))
		{
			XMFLOAT3 Position{}, RotationAngle{}, Scale{};
			XMFLOAT4 Quaternion{};

			InFile >> Position.x >> Position.y >> Position.z;
			InFile >> RotationAngle.x >> RotationAngle.y >> RotationAngle.z;
			InFile >> Scale.x >> Scale.y >> Scale.z;
			InFile >> Quaternion.x >> Quaternion.y >> Quaternion.z >> Quaternion.w;
		}
		else if (!Word.compare("<TransformMatrix>:"))
		{
			InFile >> Object->m_TransformMatrix._11 >> Object->m_TransformMatrix._12 >> Object->m_TransformMatrix._13 >> Object->m_TransformMatrix._14;
			InFile >> Object->m_TransformMatrix._21 >> Object->m_TransformMatrix._22 >> Object->m_TransformMatrix._23 >> Object->m_TransformMatrix._24;
			InFile >> Object->m_TransformMatrix._31 >> Object->m_TransformMatrix._32 >> Object->m_TransformMatrix._33 >> Object->m_TransformMatrix._34;
			InFile >> Object->m_TransformMatrix._41 >> Object->m_TransformMatrix._42 >> Object->m_TransformMatrix._43 >> Object->m_TransformMatrix._44;
		}
		else if (!Word.compare("<TotalBounds>:"))
		{
			XMFLOAT3 Center{};
			XMFLOAT3 Extents{};
			XMFLOAT4 Orientation{ 0.0f, 0.0f, 0.0f, 1.0f };

			InFile >> Center.x >> Center.y >> Center.z;
			InFile >> Extents.x >> Extents.y >> Extents.z;

			BoundingOrientedBox BoundingBox{ Center, Extents, Orientation };

			Object->SetBoundingBox(BoundingBox);
		}
		else if (!Word.compare("<Mesh>:"))
		{
			MeshInfo* MeshInfo{ Object->LoadMeshInfo(InFile) };

			if (MeshInfo)
			{
				shared_ptr<CMesh> Mesh{ make_shared<CMesh>(D3D12Device, D3D12GraphicsCommandList, MeshInfo) };

				Object->SetMesh(Mesh);

				delete MeshInfo;
			}
		}
		else if (!Word.compare("<Material>:"))
		{
			MaterialInfoFromFile* MaterialInfo{ CObject::LoadMaterialsInfoFromFile(InFile) };

			if (MaterialInfo)
			{
				XMFLOAT4 Albedo{ MaterialInfo->m_Color.m_Albedo };
				XMFLOAT4 Specular{ MaterialInfo->m_Color.m_Specular.x, MaterialInfo->m_Color.m_Specular.y, MaterialInfo->m_Color.m_Specular.z, MaterialInfo->m_Glossiness * 255.0f };
				XMFLOAT4 Ambient{ 0.2f, 0.2f, 0.2f, 0.1f };
				XMFLOAT4 Emissive{ MaterialInfo->m_Color.m_Emissive };

				shared_ptr<CMaterial> Material{ make_shared<CMaterial>(Albedo, Specular, Ambient, Emissive) };

				Object->SetMaterial(Material);

				delete MaterialInfo;
			}
		}
		else if (!Word.compare("<Children>:"))
		{
			UINT ChildCount{};

			InFile >> ChildCount;

			if (ChildCount > 0)
			{
				for (UINT i = 0; i < ChildCount; ++i)
				{
					shared_ptr<CObject> ChildObject = CObject::LoadFrameHierarchyFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);

					if (ChildObject)
					{
						Object->SetChild(ChildObject);
					}

#ifdef _WITH_DEBUG_RUNTIME_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, _T("(Child Frame: %p) (Parent Frame: %p)\n"), ChildObject, Object);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!Word.compare("</Frame>"))
		{
			break;
		}
	}

	return Object;
}

MeshInfo* CObject::LoadMeshInfo(ifstream& InFile)
{
	MeshInfo* Info{ new MeshInfo{} };
	string Word{};
	UINT VertexCount{};

	InFile >> VertexCount;

	Info->m_Positions.reserve(VertexCount);
	Info->m_Normals.reserve(VertexCount);
	Info->m_UVs.reserve(VertexCount);
	Info->m_Indices.reserve(VertexCount);

	while (InFile >> Word)
	{
		if (!Word.compare("<Bounds>:"))
		{
			InFile >> Info->m_Center.x >> Info->m_Center.y >> Info->m_Center.z;
			InFile >> Info->m_Extents.x >> Info->m_Extents.y >> Info->m_Extents.z;
		}
		else if (!Word.compare("<Positions>:"))
		{
			UINT PositionCount{};

			InFile >> PositionCount;

			if (PositionCount > 0)
			{
				for (UINT i = 0; i < PositionCount; ++i)
				{
					XMFLOAT3 Position{};

					InFile >> Position.x >> Position.y >> Position.z;

					Info->m_Positions.push_back(Position);
				}
			}
		}
		else if (!Word.compare("<Normals>:"))
		{
			UINT NormalCount{};

			InFile >> NormalCount;

			if (NormalCount > 0)
			{
				for (UINT i = 0; i < NormalCount; ++i)
				{
					XMFLOAT3 Normal{};

					InFile >> Normal.x >> Normal.y >> Normal.z;

					Info->m_Normals.push_back(Normal);
				}
			}
		}
		else if (!Word.compare("<UVs>:"))
		{
			UINT UVCount{};

			InFile >> UVCount;

			if (UVCount > 0)
			{
				for (UINT i = 0; i < UVCount; ++i)
				{
					XMFLOAT2 UV{};

					InFile >> UV.x >> UV.y;

					Info->m_UVs.push_back(UV);
				}
			}
		}
		else if (!Word.compare("<Indices>:"))
		{
			UINT IndexCount{};

			InFile >> IndexCount;

			if (IndexCount > 0)
			{
				for (UINT i = 0; i < IndexCount; ++i)
				{
					UINT Index{};

					InFile >> Index;

					Info->m_Indices.push_back(Index);
				}
			}
		}
		if (!Word.compare("</Mesh>"))
		{
			break;
		}
	}

	return Info;
}

MaterialInfoFromFile* CObject::LoadMaterialsInfoFromFile(ifstream& InFile)
{
	MaterialInfoFromFile* MaterialInfo{ new MaterialInfoFromFile{} };
	string Word{};

	while (InFile >> Word)
	{
		if (!Word.compare("<AlbedoColor>:"))
		{
			InFile >> MaterialInfo->m_Color.m_Albedo.x >> MaterialInfo->m_Color.m_Albedo.y >> MaterialInfo->m_Color.m_Albedo.z >> MaterialInfo->m_Color.m_Albedo.w;
		}
		else if (!Word.compare("<SpecularColor>:"))
		{
			InFile >> MaterialInfo->m_Color.m_Specular.x >> MaterialInfo->m_Color.m_Specular.y >> MaterialInfo->m_Color.m_Specular.z >> MaterialInfo->m_Color.m_Specular.w;
		}
		else if (!Word.compare("<EmissiveColor>:"))
		{
			InFile >> MaterialInfo->m_Color.m_Emissive.x >> MaterialInfo->m_Color.m_Emissive.y >> MaterialInfo->m_Color.m_Emissive.z >> MaterialInfo->m_Color.m_Emissive.w;
		}
		else if (!Word.compare("<SpecularHighlight>:"))
		{
			InFile >> MaterialInfo->m_SpecularHighlight;
		}
		else if (!Word.compare("<GlossyReflection>:"))
		{
			InFile >> MaterialInfo->m_GlossyReflection;
		}
		else if (!Word.compare("<Glossiness>:"))
		{
			InFile >> MaterialInfo->m_Glossiness;
		}
		else if (!Word.compare("<Smoothness>:"))
		{
			InFile >> MaterialInfo->m_Smoothness;
		}
		else if (!Word.compare("<Metallic>:"))
		{
			InFile >> MaterialInfo->m_Metallic;
		}
		else if (!Word.compare("</Material>"))
		{
			break;
		}
	}

	return MaterialInfo;
}

shared_ptr<CObject> CObject::CopyObject(const shared_ptr<CObject>& Object)
{
	if (Object)
	{
		shared_ptr<CObject> NewObject{ make_shared<CObject>(*Object) };

		if (Object->m_Sibling)
		{
			NewObject->m_Sibling = CopyObject(Object->m_Sibling);
		}

		if (Object->m_Child)
		{
			NewObject->m_Child = CopyObject(Object->m_Child);
		}

		return NewObject;
	}

	return nullptr;
}

void CObject::OnInitialize()
{

}

void CObject::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	//UINT Bytes{ (sizeof(CB_OBJECT) + 255) & ~255 };

	//m_D3D12ObjectConstantBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes,
	//	D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	//ThrowIfFailed(m_D3D12ObjectConstantBuffer->Map(0, nullptr, (void**)&m_MappedObject));
}

void CObject::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CObject* Object)
{
	//if (Object)
	//{
	//	Object->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);

	//	if (Object->m_Sibling)
	//	{
	//		CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList, Object->m_Sibling.get());
	//	}

	//	if (Object->m_Child)
	//	{
	//		CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList, Object->m_Child.get());
	//	}
	//}
}

void CObject::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	XMFLOAT4X4 WorldMatrix{};

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));

	D3D12GraphicsCommandList->SetGraphicsRoot32BitConstants(0, 16, &WorldMatrix, 0);

	if (m_Material)
	{
		XMFLOAT4 Albedo{ m_Material->m_Color.m_Albedo };
		XMFLOAT4 Specular{ m_Material->m_Color.m_Specular };
		XMFLOAT4 Ambient{ m_Material->m_Color.m_Ambient };
		XMFLOAT4 Emissive{ m_Material->m_Color.m_Emissive };

		D3D12GraphicsCommandList->SetGraphicsRoot32BitConstants(0, 4, &Albedo, 16);
		D3D12GraphicsCommandList->SetGraphicsRoot32BitConstants(0, 4, &Specular, 20);
		D3D12GraphicsCommandList->SetGraphicsRoot32BitConstants(0, 4, &Ambient, 24);
		D3D12GraphicsCommandList->SetGraphicsRoot32BitConstants(0, 4, &Emissive, 28);
	}
}

void CObject::ReleaseShaderVariables()
{
	m_D3D12ObjectConstantBuffer->Unmap(0, nullptr);
}

void CObject::ReleaseUploadBuffers()
{
	if (m_Mesh)
	{
		m_Mesh->ReleaseUploadBuffers();
	}

	if (m_Material)
	{
		if (m_Material->m_Texture)
		{
			m_Material->m_Texture->ReleaseUploadBuffers();
		}
	}
}

void CObject::Move(const XMFLOAT3& Direction, float Distance)
{
	XMFLOAT3 Shift{ Vector3::ScalarProduct(Direction, Distance, false) };
	XMFLOAT3 Position{ GetPosition() };

	SetPosition(XMFLOAT3(Position.x + Shift.x, Position.y + Shift.y, Position.z + Shift.z));
}

void CObject::Move(const XMFLOAT3& Direction, float Distance, void* TerrainContext)
{
	XMFLOAT3 Shift{ Vector3::ScalarProduct(Direction, Distance, false) };
	XMFLOAT3 Position{ GetPosition() };

	// 객체가 지형 밖으로 이동하지 못하도록 제한한다.
	if (TerrainContext)
	{
		CTerrainObject* TerrainObject{ (CTerrainObject*)TerrainContext };
		float MaxExtent{ max(m_BoundingBox.Extents.x, m_BoundingBox.Extents.z) };

		if (Position.x - MaxExtent < 0.0f)
		{
			if (Shift.x < 0.0f)
			{
				Shift.x = 0.0f;
			}
		}
		else if (Position.x + MaxExtent > TerrainObject->GetWidth())
		{
			if (Shift.x > 0.0f)
			{
				Shift.x = 0.0f;
			}
		}

		if (Position.z - MaxExtent < 0.0f)
		{
			if (Shift.z < 0.0f)
			{
				Shift.z = 0.0f;
			}
		}
		else if (Position.z + MaxExtent > TerrainObject->GetLength())
		{
			if (Shift.z > 0.0f)
			{
				Shift.z = 0.0f;
			}
		}
	}

	SetPosition(XMFLOAT3(Position.x + Shift.x, Position.y + Shift.y, Position.z + Shift.z));
}

void CObject::Animate(float ElapsedTime)
{
	XMFLOAT3 Gravity{ 0.0f, -1.0f, 0.0f };

	Move(Gravity, 9.8f * ElapsedTime, nullptr);
}

void CObject::Animate(float ElapsedTime, const XMFLOAT3& Target)
{

}

void CObject::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	UpdateShaderVariables(D3D12GraphicsCommandList);

	if (m_Material)
	{
		if (m_Material->m_Texture)
		{
			m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 3, 0);
		}
	}

	if (IsVisible(Camera))
	{
		if (m_Mesh)
		{
			m_Mesh->Render(D3D12GraphicsCommandList);
		}
	}

	if (m_Sibling)
	{
		m_Sibling->Render(D3D12GraphicsCommandList, Camera);
	}

	if (m_Child)
	{
		m_Child->Render(D3D12GraphicsCommandList, Camera);
	}
}

bool CObject::IsActive() const
{
	return m_IsActive;
}

void CObject::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

void CObject::SetWorldMatrix(const XMFLOAT4X4 WorldMatrix)
{
	m_WorldMatrix = WorldMatrix;
}

const XMFLOAT4X4& CObject::GetWorldMatrix() const
{
	return m_WorldMatrix;
}

void CObject::SetRight(const XMFLOAT3& Right)
{
	m_TransformMatrix._11 = Right.x;
	m_TransformMatrix._12 = Right.y;
	m_TransformMatrix._13 = Right.z;

	UpdateTransform(Matrix4x4::Identity());
}

XMFLOAT3 CObject::GetRight() const
{
	return XMFLOAT3(m_WorldMatrix._11, m_WorldMatrix._12, m_WorldMatrix._13);
}

void CObject::SetUp(const XMFLOAT3& Up)
{
	m_TransformMatrix._21 = Up.x;
	m_TransformMatrix._22 = Up.y;
	m_TransformMatrix._23 = Up.z;

	UpdateTransform(Matrix4x4::Identity());
}

XMFLOAT3 CObject::GetUp() const
{
	return XMFLOAT3(m_WorldMatrix._21, m_WorldMatrix._22, m_WorldMatrix._23);
}

void CObject::SetLook(const XMFLOAT3& Look)
{
	m_TransformMatrix._31 = Look.x;
	m_TransformMatrix._32 = Look.y;
	m_TransformMatrix._33 = Look.z;

	UpdateTransform(Matrix4x4::Identity());
}

XMFLOAT3 CObject::GetLook() const
{
	return XMFLOAT3(m_WorldMatrix._31, m_WorldMatrix._32, m_WorldMatrix._33);
}

void CObject::SetPosition(const XMFLOAT3& Position)
{
	m_TransformMatrix._41 = Position.x;
	m_TransformMatrix._42 = Position.y;
	m_TransformMatrix._43 = Position.z;

	UpdateTransform(Matrix4x4::Identity());
}

XMFLOAT3 CObject::GetPosition() const
{
	return XMFLOAT3(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43);
}

void CObject::SetMovingDirection(const XMFLOAT3& MovingDirection)
{
	m_MovingDirection = MovingDirection;
}

XMFLOAT3 CObject::GetMovingDirection() const
{
	return m_MovingDirection;
}

void CObject::SetMesh(const shared_ptr<CMesh>& Mesh)
{
	if (Mesh)
	{
		m_Mesh = Mesh;
	}
}

void CObject::SetMaterial(const shared_ptr<CMaterial>& Material)
{
	m_Material = Material;
}

void CObject::SetTexture(const shared_ptr<CTexture>& Texture)
{
	if (m_Material)
	{
		m_Material->m_Texture = Texture;
	}
}

void CObject::SetChild(const shared_ptr<CObject>& ChildObject)
{
	if (m_Child)
	{
		if (ChildObject)
		{
			ChildObject->m_Sibling = m_Child->m_Sibling;
			m_Child->m_Sibling = ChildObject;
		}
	}
	else
	{
		m_Child = ChildObject;
	}
}

shared_ptr<CObject> CObject::FindFrame(const string& FrameName)
{
	if (!FrameName.compare(m_FrameName))
	{
		return shared_from_this();
	}

	if (m_Sibling)
	{
		return m_Sibling->FindFrame(FrameName);
	}

	if (m_Child)
	{
		return m_Child->FindFrame(FrameName);
	}

	return nullptr;
}

void CObject::UpdateTransform(const XMFLOAT4X4& TransformMatrix)
{
	m_WorldMatrix = Matrix4x4::Multiply(m_TransformMatrix, TransformMatrix);

	if (m_Sibling)
	{
		m_Sibling->UpdateTransform(TransformMatrix);
	}

	if (m_Child)
	{
		m_Child->UpdateTransform(m_WorldMatrix);
	}
}

void CObject::UpdateBoundingBox()
{
	if (m_Mesh)
	{
		m_Mesh->GetBoundingBox().Transform(m_BoundingBox, XMLoadFloat4x4(&m_WorldMatrix));
		XMStoreFloat4(&m_BoundingBox.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_BoundingBox.Orientation)));
	}
}

void CObject::SetBoundingBox(const BoundingOrientedBox& BoundingBox)
{
	m_BoundingBox = BoundingBox;
}

const BoundingOrientedBox& CObject::GetBoundingBox() const
{
	return m_BoundingBox;
}

void CObject::KeepDistanceToTerrain(float ElapsedTime, void* TerrainContext, float DistanceToTerrain)
{
	if (m_IsActive)
	{
		CTerrainObject* TerrainObject{ (CTerrainObject*)TerrainContext };
		XMFLOAT3 Position{ GetPosition() };

		float Height{ TerrainObject->GetHeight(Position.x, Position.z) + DistanceToTerrain };

		if (Position.y < Height)
		{
			Position.y = Height;

			SetPosition(Position);
		}

		UpdateBoundingBox();
	}
}

bool CObject::IsVisible(CCamera* Camera) const
{
	bool Visible{};

	if (m_Mesh)
	{
		BoundingOrientedBox BoundingBox{ m_Mesh->GetBoundingBox() };

		BoundingBox.Transform(BoundingBox, XMLoadFloat4x4(&m_WorldMatrix));

		if (Camera)
		{
			Visible = Camera->IsInFrustum(BoundingBox);
		}
	}

	return Visible;
}

void CObject::LookAtTarget(const XMFLOAT3& Target)
{
	XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };
	XMFLOAT3 Look{ Vector3::Normalize(Vector3::Subtract(Target, GetPosition())) };
	XMFLOAT3 Right{ Vector3::CrossProduct(WorldUp, Look, false) };

	SetUp(WorldUp);
	SetLook(Look);
	SetRight(Right);
}

void CObject::LookAtTarget(const XMFLOAT3& Target, float ElapsedTime)
{
	XMFLOAT3 ToPlayer{ Vector3::Normalize(Vector3::Subtract(Target, GetPosition())) };
	XMFLOAT3 NormalizedLook{ Vector3::Normalize(GetLook()) };

	SetMovingDirection(ToPlayer);

	// 스칼라 삼중적을 이용하여 현재 바라보고 있는 방향에서 최소 회전으로 플레이어를 바라보도록 한다.
	float TripleProduct{ Vector3::DotProduct(GetUp(), Vector3::CrossProduct(NormalizedLook, m_MovingDirection, false)) };
	float Radian{ acos(Vector3::DotProduct(NormalizedLook, m_MovingDirection)) };

	if (!isnan(Radian))
	{
		if (TripleProduct > 0.0f)
		{
			Rotate(GetUp(), XMConvertToDegrees(+Radian * ElapsedTime));
		}
		else if (TripleProduct < 0.0f)
		{
			Rotate(GetUp(), XMConvertToDegrees(-Radian * ElapsedTime));
		}
	}
}

void CObject::GenerateRayForPicking(const XMFLOAT3& PickPosition, const XMFLOAT4X4& ViewMatrix, XMFLOAT3& PickRayOrigin, XMFLOAT3& PickRayDirection)
{
	XMFLOAT4X4 WorldViewMatrix{ Matrix4x4::Multiply(m_WorldMatrix, ViewMatrix) };
	XMFLOAT4X4 InverseWorldViewMatrix{ Matrix4x4::Inverse(WorldViewMatrix) };
	XMFLOAT3 CameraOrigin{};

	// 카메라 좌표계의 원점을 모델 좌표계로 변환한다.
	PickRayOrigin = Vector3::TransformCoord(CameraOrigin, InverseWorldViewMatrix);

	// 카메라 좌표계의 점(마우스 좌표를 역변환하여 구한 점)을 모델 좌표계로 변환한다.
	PickRayDirection = Vector3::TransformCoord(PickPosition, InverseWorldViewMatrix);

	// 광선의 방향 벡터를 구한다.
	PickRayDirection = Vector3::Normalize(Vector3::Subtract(PickRayDirection, PickRayOrigin));
}

UINT CObject::PickObjectByRayIntersection(const XMFLOAT3& PickPosition, const XMFLOAT4X4& ViewMatrix, float& HitDistance)
{
	UINT Intersected{};

	if (m_Mesh)
	{
		XMFLOAT3 PickRayOrigin{};
		XMFLOAT3 PickRayDirection{};

		// 모델 좌표계의 광선을 생성한다.
		GenerateRayForPicking(PickPosition, ViewMatrix, PickRayOrigin, PickRayDirection);

		// 모델 좌표계의 광선과 메쉬의 교차를 검사한다.
		Intersected = m_Mesh->CheckRayIntersection(PickRayOrigin, PickRayDirection, HitDistance);
	}

	return Intersected;
}

void CObject::Scale(float Pitch, float Yaw, float Roll)
{
	XMMATRIX ScaleMatrix{ XMMatrixScaling(Pitch, Yaw, Roll) };

	m_TransformMatrix = Matrix4x4::Multiply(ScaleMatrix, m_TransformMatrix);

	UpdateTransform(Matrix4x4::Identity());
}

void CObject::Rotate(const XMFLOAT3& Axis, float Angle)
{
	XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(Axis, Angle) };

	m_TransformMatrix = Matrix4x4::Multiply(RotationMatrix, m_TransformMatrix);

	UpdateTransform(Matrix4x4::Identity());
}

// ======================================================= CEnemyObject =======================================================

void CEnemyObject::Move(const XMFLOAT3& Direction, float Distance, void* TerrainContext)
{
	CObject::Move(Direction, Distance, TerrainContext);

	// 객체가 움직일 때, 체력바도 같이 머리 위에서 움직이도록 설정한다.
	if (m_HpBarMesh)
	{
		XMFLOAT3 Position{ GetPosition() };
		float Height{ m_BoundingBox.Extents.y };

		Position.y += (Height + 2.5f);
		m_HpBarMesh->SetPosition(Position);
	}
}

void CEnemyObject::Animate(float ElapsedTime)
{

}

void CEnemyObject::Animate(float ElapsedTime, const XMFLOAT3& Target)
{
	if (m_IsActive)
	{
		if (m_IsExploded)
		{
			const float FrameFPS{ 20.0f };

			m_ExplosionMesh->IncreaseFrameTime(FrameFPS * ElapsedTime, 1);

			if (m_ExplosionMesh->GetFrameTime() < 0.0f)
			{
				m_IsActive = false;
				m_IsExploded = false;
			}
		}
		else
		{
			LookAtTarget(Target, ElapsedTime);
			CObject::Animate(ElapsedTime);
		}
	}
}

void CEnemyObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (m_IsActive && !m_IsExploded)
	{
		UpdateTransform(Matrix4x4::Identity());
		UpdateShaderVariables(D3D12GraphicsCommandList);

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

bool CEnemyObject::IsExploded() const
{
	return m_IsExploded;
}

void CEnemyObject::PrepareExplosion()
{
	XMFLOAT2 HpBar{ m_HpBarMesh->GetSize() };

	HpBar.x -= 1.5f;

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

void CEnemyObject::SetHpBarMesh(CBilboardMesh* HpBarMesh)
{
	m_HpBarMesh = HpBarMesh;
}

void CEnemyObject::SetExplosionMesh(CSpriteBilboardMesh* ExplosionMesh)
{
	m_ExplosionMesh = ExplosionMesh;
}

// ======================================================= CTerrainObject =======================================================

CTerrainObject::CTerrainObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, LPCTSTR FileName, int Width, int Length, int BlockWidth, int BlockLength, const XMFLOAT3& Scale) : CObject(),
	m_HeightMapImage{ make_shared<CHeightMapImage>(FileName, Width, Length, Scale) },
	m_Width{ Width },
	m_Length{ Length },
	m_Scale{ Scale }
{
	shared_ptr<CHeightMapMesh> Mesh{};

#ifdef TERRAIN_TESSELLATION
	int XBlocks{ (Width - 1) / (BlockWidth - 1) };
	int ZBlocks{ (Length - 1) / (BlockLength - 1) };

	m_Meshes.reserve(XBlocks * ZBlocks);

	for (int z = 0, ZStart = 0; z < ZBlocks; ++z)
	{
		for (int x = 0, XStart = 0; x < XBlocks; ++x)
		{
			XStart = x * (BlockWidth - 1);
			ZStart = z * (BlockLength - 1);
			Mesh = make_shared<CHeightMapMesh>(D3D12Device, D3D12GraphicsCommandList, XStart, ZStart, BlockWidth, BlockLength, Scale, m_HeightMapImage.get());
			
			m_Meshes.push_back(Mesh);
		}
	}
#else
	SetMesh(make_shared<CHeightMapMesh>(D3D12Device, D3D12GraphicsCommandList, 0, 0, BlockWidth, BlockLength, Scale, m_HeightMapImage.get()));
#endif
}

void CTerrainObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	UpdateShaderVariables(D3D12GraphicsCommandList);

	if (m_Material)
	{
		if (m_Material->m_Texture)
		{
			m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 4, 0);
		}
	}

#ifdef TERRAIN_TESSELLATION
	for (const auto& Mesh : m_Meshes)
	{
		Mesh->Render(D3D12GraphicsCommandList);
	}
#else
	if (m_Mesh)
	{
		m_Mesh->Render(D3D12GraphicsCommandList);
	}
#endif
}

CHeightMapImage* CTerrainObject::GetImage()
{
	return m_HeightMapImage.get();
}

float CTerrainObject::GetHeight(float Xpos, float Zpos)
{
	return m_HeightMapImage->GetHeight(Xpos, Zpos) * m_Scale.y;
}

XMFLOAT3 CTerrainObject::GetNormal(float Xpos, float Zpos)
{
	return m_HeightMapImage->GetNormal(Xpos, Zpos);
}

int CTerrainObject::GetWidth() const
{
	return (UINT)(m_Scale.x * m_Width);
}

int CTerrainObject::GetLength() const
{
	return (UINT)(m_Scale.z * m_Length);
}

const XMFLOAT3& CTerrainObject::GetScale() const
{
	return m_Scale;
}

int CTerrainObject::GetHeightMapWidth()
{
	return m_HeightMapImage->GetWidth();
}

int CTerrainObject::GetHeightMapLength()
{
	return m_HeightMapImage->GetLength();
}

// ================================================= CWallObject =================================================

void CWallObject::Animate(float ElapsedTime)
{

}

void CWallObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (m_IsActive)
	{
		UpdateTransform(Matrix4x4::Identity());
		UpdateShaderVariables(D3D12GraphicsCommandList);

		if (m_Material)
		{
			if (m_Material->m_Texture)
			{
				m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 3, 2);
			}
		}

		if (m_Child)
		{
			m_Child->Render(D3D12GraphicsCommandList, Camera);
		}
	}
}

// ================================================= CBoxObject =================================================

void CBoxObject::Animate(float ElapsedTime)
{

}

void CBoxObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (m_IsActive)
	{
		UpdateTransform(Matrix4x4::Identity());
		UpdateShaderVariables(D3D12GraphicsCommandList);

		if (m_Material)
		{
			if (m_Material->m_Texture)
			{
				m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 3, 3);
			}
		}

		if (m_Child)
		{
			m_Child->Render(D3D12GraphicsCommandList, Camera);
		}
	}
}

// ================================================= CMirrorObject =================================================

void CMirrorObject::Animate(float ElapsedTime)
{

}

void CMirrorObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	UpdateShaderVariables(D3D12GraphicsCommandList);

	if (m_Material)
	{
		if (m_Material->m_Texture)
		{
			m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 3, 0);
		}
	}

	if (IsVisible(Camera))
	{
		if (m_Mesh)
		{
			m_Mesh->Render(D3D12GraphicsCommandList);
		}
	}

	if (m_Sibling)
	{
		m_Sibling->Render(D3D12GraphicsCommandList, Camera);
	}

	if (m_Child)
	{
		m_Child->Render(D3D12GraphicsCommandList, Camera);
	}
}

// ================================================= CMirrorBackObject =================================================

void CMirrorBackObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	UpdateShaderVariables(D3D12GraphicsCommandList);

	if (m_Material)
	{
		if (m_Material->m_Texture)
		{
			m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 3, 2);
		}
	}

	if (IsVisible(Camera))
	{
		if (m_Mesh)
		{
			m_Mesh->Render(D3D12GraphicsCommandList);
		}
	}

	if (m_Sibling)
	{
		m_Sibling->Render(D3D12GraphicsCommandList, Camera);
	}

	if (m_Child)
	{
		m_Child->Render(D3D12GraphicsCommandList, Camera);
	}
}

// ================================================= CBilboardObject =================================================

void CBilboardObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{

}

void CBilboardObject::ReleaseUploadBuffers()
{
	if (m_D3D12VertexUploadBuffer)
	{
		m_D3D12VertexUploadBuffer->Release();
	}

	m_D3D12VertexUploadBuffer = nullptr;
}

void CBilboardObject::SetMaterial(const shared_ptr<CMaterial>& Material)
{
	m_Material = Material;
}

void CBilboardObject::SetTexture(const shared_ptr<CTexture>& Texture)
{
	if (m_Material)
	{
		m_Material->m_Texture = Texture;
	}
}

// ================================================= CBackgroundObject =================================================

CBackgroundObject::CBackgroundObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	vector<CSpriteBilboardMesh> Vertices{};

	// 뷰포트 좌표 기준 위치와 가로, 세로 길이
	XMFLOAT3 Position{ 0.5f * FRAME_BUFFER_WIDTH, 0.5f * FRAME_BUFFER_HEIGHT, 0.0f };
	XMFLOAT2 Size{ FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };

	Vertices.emplace_back(Position, XMFLOAT3(0.0f, 0.0f, 0.0f), Size, 1, 1, 0.0f);

	m_D3D12VertexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Vertices.data(), sizeof(CSpriteBilboardMesh) * (UINT)Vertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12VertexUploadBuffer.GetAddressOf());

	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(CSpriteBilboardMesh);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(CSpriteBilboardMesh) * (UINT)Vertices.size();
}

void CBackgroundObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (m_Material)
	{
		if (m_Material->m_Texture)
		{
			m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 0, 0);
		}
	}

	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12VertexBufferView };

	D3D12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	D3D12GraphicsCommandList->IASetVertexBuffers(0, 1, VertexBufferViews);
	D3D12GraphicsCommandList->DrawInstanced(1, 1, 0, 0);
}

// ================================================= CButtonObject =================================================

CButtonObject::CButtonObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT ObjectCount) :
	m_ObjectCount { ObjectCount}
{
	m_D3D12VertexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(CSpriteBilboardMesh) * ObjectCount,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	ThrowIfFailed(m_D3D12VertexBuffer->Map(0, nullptr, (void**)&m_MappedMeshes));

	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(CSpriteBilboardMesh);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(CSpriteBilboardMesh) * ObjectCount;

	XMFLOAT3 Position{ 800.0f, 660.0f, 0.0f  };
	XMFLOAT2 Size{ 250.0f, 70.0f };

	for (UINT i = 0; i < ObjectCount; ++i)
	{
		CSpriteBilboardMesh* MappedMesh{ m_MappedMeshes + i };

		Position.y += (100.0f * i);
		MappedMesh->SetPosition(Position);
		MappedMesh->SetSize(Size);
		MappedMesh->SetSpriteRow(2);
		MappedMesh->SetSpriteColumn(2);
		MappedMesh->SetFrameTime(1.0f * i);
	}
}

void CButtonObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (m_Material)
	{
		if (m_Material->m_Texture)
		{
			m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 0, 1);
		}
	}

	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12VertexBufferView };

	D3D12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	D3D12GraphicsCommandList->IASetVertexBuffers(0, 1, VertexBufferViews);
	D3D12GraphicsCommandList->DrawInstanced(2, 1, 0, 0);
}

int CButtonObject::ActiveButton(int ScreenX, int ScreenY)
{
	for (UINT i = 0; i < m_ObjectCount; ++i)
	{
		CSpriteBilboardMesh* MappedMesh{ m_MappedMeshes + i };
		XMFLOAT3 Position{ MappedMesh->GetPosition() };
		XMFLOAT2 Size{ MappedMesh->GetSize() };

		Size.x *= 0.5f;
		Size.y *= 0.5f;

		if ((Position.x - Size.x <= ScreenX && ScreenX <= Position.x + Size.x) &&
			(Position.y - Size.y <= ScreenY && ScreenY <= Position.y + Size.y))
		{
			if (MappedMesh->GetFrameTime() < 2.0f + 1.0f * i)
			{
				CSound::GetInstance()->Play(CSound::BUTTON_SOUND, 0.7f);
			}

			MappedMesh->SetFrameTime(2.0f + 1.0f * i);

			return (i == 0) ? GAME_START : EXIT;
		}
		else
		{
			MappedMesh->SetFrameTime(1.0f * i);
		}
	}

	return NONE;
}

// ================================================= CSkyBoxObject =================================================

CSkyBoxObject::CSkyBoxObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	vector<CBilboardMesh> Vertices{};
	XMFLOAT3 Position{};
	XMFLOAT3 Direction{};
	XMFLOAT2 Size{ 20.0f, 20.0f };

	Vertices.reserve(6);

	Position = { 0.0f, 0.0f, -10.0f };
	Vertices.emplace_back(Position, Direction, Size);

	Position = { 0.0f, 0.0f, +10.0f };
	Vertices.emplace_back(Position, Direction, Size);

	Position = { +10.0f, 0.0f, 0.0f };
	Vertices.emplace_back(Position, Direction, Size);

	Position = { -10.0f, 0.0f, 0.0f };
	Vertices.emplace_back(Position, Direction, Size);

	Position = { 0.0f, +10.0f, 0.0f };
	Vertices.emplace_back(Position, Direction, Size);

	Position = { 0.0f, -10.0f, 0.0f };
	Vertices.emplace_back(Position, Direction, Size);

	m_D3D12VertexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Vertices.data(), sizeof(CBilboardMesh) * (UINT)Vertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12VertexUploadBuffer.GetAddressOf());

	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(CBilboardMesh);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(CBilboardMesh) * (UINT)Vertices.size();
}

void CSkyBoxObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12VertexBufferView };

	D3D12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	D3D12GraphicsCommandList->IASetVertexBuffers(0, 1, VertexBufferViews);

	for (UINT i = 0; i < 6; ++i)
	{
		if (m_Material)
		{
			if (m_Material->m_Texture)
			{
				m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 3, i);
			}
		}

		D3D12GraphicsCommandList->DrawInstanced(1, 1, i, 0);
	}
}

// ================================================= CTreeObject =================================================

CTreeObject::CTreeObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, void* TerrainContext)
{
	if (TerrainContext)
	{
		CTerrainObject* TerrainObject{ (CTerrainObject*)TerrainContext };
		vector<CBilboardMesh> Vertices{};

		Vertices.reserve(100);

		for (UINT z = 0; z < 10; ++z)
		{
			for (UINT x = 0; x < 10; ++x)
			{
				XMFLOAT2 Size{};

				Size.x = Size.y = 10.0f + (rand() % 6);

				XMFLOAT3 Position{};

				Position.x = 15.0f + 25.0f * x;
				Position.z = 15.0f + 25.0f * z;
				Position.y = TerrainObject->GetHeight(Position.x, Position.z) + (0.5f * Size.y);

				XMFLOAT3 Direction{};

				Vertices.emplace_back(Position, Direction, Size);
			}
		}

		m_D3D12VertexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Vertices.data(), sizeof(CBilboardMesh) * (UINT)Vertices.size(),
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12VertexUploadBuffer.GetAddressOf());

		m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
		m_D3D12VertexBufferView.StrideInBytes = sizeof(CBilboardMesh);
		m_D3D12VertexBufferView.SizeInBytes = sizeof(CBilboardMesh) * (UINT)Vertices.size();
	}
}

void CTreeObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (m_Material)
	{
		if (m_Material->m_Texture)
		{
			m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 3, 0);
		}
	}

	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12VertexBufferView };

	D3D12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	D3D12GraphicsCommandList->IASetVertexBuffers(0, 1, VertexBufferViews);
	D3D12GraphicsCommandList->DrawInstanced(100, 1, 0, 0);
}

// ================================================= CHpBarObject =================================================

CHpBarObject::CHpBarObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT ObjectCount) :
	m_ObjectCount{ ObjectCount }
{
	m_D3D12VertexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(CBilboardMesh) * ObjectCount,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	ThrowIfFailed(m_D3D12VertexBuffer->Map(0, nullptr, (void**)&m_MappedMeshes));

	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(CBilboardMesh);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(CBilboardMesh) * ObjectCount;

	// 생성된 빌보드의 크기를 설정한다.
	XMFLOAT2 Size{ 4.5f, 0.5f };

	for (UINT i = 0; i < ObjectCount; ++i)
	{
		CBilboardMesh* MappedMesh{ m_MappedMeshes + i };

		MappedMesh->SetSize(Size);
	}
}

void CHpBarObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12VertexBufferView };

	D3D12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	D3D12GraphicsCommandList->IASetVertexBuffers(0, 1, VertexBufferViews);

	for (UINT i = 0; i < m_ObjectCount; ++i)
	{
		CBilboardMesh* MappedMesh{ m_MappedMeshes + i };

		if (MappedMesh->GetSize().x > 0.0f)
		{
			D3D12GraphicsCommandList->DrawInstanced(1, 1, i, 0);
		}
	}
}

CBilboardMesh* CHpBarObject::GetMappedMesh()
{
	return m_MappedMeshes;
}

// ================================================= CExplodedObject =================================================

CExplodedEnemyObject::CExplodedEnemyObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT ObjectCount) :
	m_ObjectCount{ ObjectCount }
{
	m_D3D12VertexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(CSpriteBilboardMesh) * ObjectCount,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	ThrowIfFailed(m_D3D12VertexBuffer->Map(0, nullptr, (void**)&m_MappedMeshes));

	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(CSpriteBilboardMesh);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(CSpriteBilboardMesh) * ObjectCount;

	// 생성된 빌보드의 크기, 스프라이트의 가로, 세로 개수, 프레임 시간을 설정한다.
	for (UINT i = 0; i < ObjectCount; ++i)
	{
		CSpriteBilboardMesh* MappedMesh{ m_MappedMeshes + i };
		XMFLOAT2 Size{ 10.0f, 10.0f };

		MappedMesh->SetSize(Size);
		MappedMesh->SetSpriteRow(4);
		MappedMesh->SetSpriteColumn(8);
		MappedMesh->SetFrameTime(-1.0f);
	}
}

void CExplodedEnemyObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (m_Material)
	{
		if (m_Material->m_Texture)
		{
			m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 3, 0);
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

CSpriteBilboardMesh* CExplodedEnemyObject::GetMappedMesh()
{
	return m_MappedMeshes;
}

// ================================================= CSmokeObject =================================================

CSmokeObject::CSmokeObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT ObjectCount) :
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

		MappedMesh->SetSpriteRow(2);
		MappedMesh->SetSpriteColumn(2);
		MappedMesh->SetFrameTime(-1.0f);
	}
}

void CSmokeObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (m_Material)
	{
		if (m_Material->m_Texture)
		{
			m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 3, 2);
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

CSpriteBilboardMesh* CSmokeObject::GetMappedMesh()
{
	return m_MappedMeshes;
}

// ================================================= CParticleObject =================================================

CParticleObject::CParticleObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

CParticleObject::~CParticleObject()
{

}

void CParticleObject::ReleaseUploadBuffers()
{
	CObject::ReleaseUploadBuffers();
}

void CParticleObject::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	if (m_Material)
	{
		if (m_Material->m_Texture)
		{
			m_Material->m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList, 3, 0);
		}
	}

	UpdateShaderVariables(D3D12GraphicsCommandList);

	m_Mesh->PreRender(D3D12GraphicsCommandList, 0);
	m_Mesh->Render(D3D12GraphicsCommandList, 0);
	m_Mesh->PostRender(D3D12GraphicsCommandList, 0);
}

void CParticleObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	m_Mesh->PreRender(D3D12GraphicsCommandList, 1);
	m_Mesh->Render(D3D12GraphicsCommandList, 1);
}
