#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Sound.h"

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02 // []
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

struct MaterialColor
{
	XMFLOAT4							m_Albedo{};
	XMFLOAT4							m_Specular{};
	XMFLOAT4							m_Ambient{};
	XMFLOAT4							m_Emissive{};
};

struct MaterialInfoFromFile
{
	MaterialColor						m_Color{};

	float								m_SpecularHighlight{};
	float								m_GlossyReflection{};
	float								m_Glossiness{};
	float								m_Smoothness{};
	float								m_Metallic{};
};

// ======================================================= CTexture =======================================================

class CTexture
{
private:
	UINT								m_Type{};

	vector<ComPtr<ID3D12Resource>>		m_D3D12Textures{};
	vector<ComPtr<ID3D12Resource>>		m_D3D12TextureUploadBuffers{};
	vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_D3D12SrvGpuDescriptorHandles{};

public:
	CTexture(UINT TextureType);
	~CTexture() = default;

	void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT RootParameterNum, UINT GpuHandleIndex);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();

	void CreateTexture(ID3D12Device* D3D12Device, const UINT64& Width, UINT Height, D3D12_RESOURCE_STATES D3D12ResourceStates, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DxgiFormat, const D3D12_CLEAR_VALUE& D3D12ClearValue);
	void LoadTextureFromDDSFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const wchar_t* FileName);

	UINT GetTextureType() const;
	UINT GetTextureCount() const;

	ID3D12Resource* GetResource(UINT Index);

	void SetGpuDescriptorHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& D3D12SrvGpuDescriptorHandle);
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuDescriptorHandle(UINT Index) const;

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(UINT Index);
};

// ======================================================= CMaterial =======================================================

class CMaterial
{
public:
	MaterialColor						m_Color{};

	shared_ptr<CTexture>				m_Texture{};

public:
	CMaterial() = default;
	CMaterial(const XMFLOAT4& Albedo, const XMFLOAT4& Specular, const XMFLOAT4& Ambient, const XMFLOAT4& Emissive);
	~CMaterial() = default;
};

// ======================================================= CObject =======================================================

struct CB_OBJECT
{
	XMFLOAT4X4							m_WorldMatrix{};

	MaterialColor						m_Material{};
};

class CObject : public enable_shared_from_this<CObject>
{
protected:
	bool								m_IsActive{};

	string								m_FrameName{};
	float								m_FrameTime{};

	XMFLOAT4X4							m_WorldMatrix{ Matrix4x4::Identity() };

	XMFLOAT3							m_MovingDirection{};

	shared_ptr<CMesh>					m_Mesh{};
	shared_ptr<CMaterial>				m_Material{};
	BoundingOrientedBox					m_BoundingBox{};

	shared_ptr<CObject>					m_Sibling{};
	shared_ptr<CObject>					m_Child{};

	ComPtr<ID3D12Resource>				m_D3D12ObjectConstantBuffer{};
	CB_OBJECT*							m_MappedObject{};

public:
	XMFLOAT4X4							m_TransformMatrix{ Matrix4x4::Identity() };

public:
	CObject() = default;
	CObject(const CObject& Rhs);
	virtual ~CObject() = default;

	static pair<shared_ptr<CObject>, BoundingOrientedBox> LoadGeometryFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const string& FileName);
	static shared_ptr<CObject> LoadFrameHierarchyFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ifstream& InFile);
	static MeshInfo* LoadMeshInfo(ifstream& InFile);
	static MaterialInfoFromFile* LoadMaterialsInfoFromFile(ifstream& InFile);
	static shared_ptr<CObject> CopyObject(const shared_ptr<CObject>& Object);

	virtual void OnInitialize();

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CObject* Object);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Move(const XMFLOAT3& Direction, float Distance);
	virtual void Move(const XMFLOAT3& Direction, float Distance, void* TerrainContext);

	virtual void Animate(float ElapsedTime);
	virtual void Animate(float ElapsedTime, const XMFLOAT3& Target);
	virtual void PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	bool IsActive() const;
	void SetActive(bool IsActive);

	void SetWorldMatrix(const XMFLOAT4X4 WorldMatrix);
	const XMFLOAT4X4& GetWorldMatrix() const;

	void SetRight(const XMFLOAT3& Right);
	XMFLOAT3 GetRight() const;

	void SetUp(const XMFLOAT3& Up);
	XMFLOAT3 GetUp() const;

	void SetLook(const XMFLOAT3& Look);
	XMFLOAT3 GetLook() const;

	void SetPosition(const XMFLOAT3& Position);
	XMFLOAT3 GetPosition() const;

	void SetMovingDirection(const XMFLOAT3& MovingDirection);
	XMFLOAT3 GetMovingDirection() const;

	void SetMesh(const shared_ptr<CMesh>& Mesh);
	void SetMaterial(const shared_ptr<CMaterial>& Material);
	void SetTexture(const shared_ptr<CTexture>& Texture);
	
	void SetChild(const shared_ptr<CObject>& ChildObject);
	shared_ptr<CObject> FindFrame(const string& FrameName);

	void UpdateTransform(const XMFLOAT4X4& TransformMatrix);

	void UpdateBoundingBox();
	void SetBoundingBox(const BoundingOrientedBox& BoundingBox);
	const BoundingOrientedBox& GetBoundingBox() const;

	void KeepDistanceToTerrain(float ElapsedTime, void* TerrainContext, float DistanceToTerrain);
	
	bool IsVisible(CCamera* Camera) const;
	void LookAtTarget(const XMFLOAT3& Target);
	void LookAtTarget(const XMFLOAT3& Target, float ElapsedTime);

	void Scale(float Pitch, float Yaw, float Roll);
	void Rotate(const XMFLOAT3& Axis, float Angle);

	void GenerateRayForPicking(const XMFLOAT3& PickPosition, const XMFLOAT4X4& ViewMatrix, XMFLOAT3& PickRayOrigin, XMFLOAT3& PickRayDirection);
	UINT PickObjectByRayIntersection(const XMFLOAT3& PickPosition, const XMFLOAT4X4& ViewMatrix, float& HitDistance);
};

// ======================================================= CEnemyObject =======================================================

class CEnemyObject : public CObject
{
private:
	bool								m_IsExploded{};

	CBilboardMesh*						m_HpBarMesh{};
	CSpriteBilboardMesh*				m_ExplosionMesh{};

public:
	CEnemyObject() = default;
	virtual ~CEnemyObject() = default;

	virtual void Move(const XMFLOAT3& Direction, float Distance, void* TerrainContext);

	virtual void Animate(float ElapsedTime);
	virtual void Animate(float ElapsedTime, const XMFLOAT3& Target);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	bool IsExploded() const;
	void PrepareExplosion();

	void SetHpBarMesh(CBilboardMesh* HpBarMesh);
	void SetExplosionMesh(CSpriteBilboardMesh* ExplosionMesh);
};

// ======================================================= CTerrainObject =======================================================

class CTerrainObject : public CObject
{
private:
	shared_ptr<CHeightMapImage>			m_HeightMapImage{};

#ifdef TERRAIN_TESSELLATION
	vector<shared_ptr<CMesh>>			m_Meshes{};
#endif

	int									m_Width{};
	int									m_Length{};
	XMFLOAT3							m_Scale{};

public:
	CTerrainObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, LPCTSTR FileName, int Width, int Length, int BlockWidth, int BlockLength, const XMFLOAT3& Scale);
	virtual ~CTerrainObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	CHeightMapImage* GetImage();

	float GetHeight(float Xpos, float Zpos);
	XMFLOAT3 GetNormal(float Xpos, float Zpos);

	int GetWidth() const;
	int GetLength() const;

	const XMFLOAT3& GetScale() const;

	int GetHeightMapWidth();
	int GetHeightMapLength();
};

// ================================================= CWallObject =================================================

class CWallObject : public CObject
{
public:
	enum { LOC_FRONT, LOC_LEFT, LOC_RIGHT, LOC_TOP, LOC_BOTTOM };

public:
	CWallObject() = default;
	virtual ~CWallObject() = default;

	virtual void Animate(float ElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ================================================= CBoxObject =================================================

class CBoxObject : public CObject
{
public:
	CBoxObject() = default;
	virtual ~CBoxObject() = default;

	virtual void Animate(float ElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ================================================= CMirrorObject =================================================

class CMirrorObject : public CObject
{
public:
	CMirrorObject() = default;
	virtual ~CMirrorObject() = default;

	virtual void Animate(float ElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ================================================= CMirrorBackObject =================================================

class CMirrorBackObject : public CMirrorObject
{
public:
	CMirrorBackObject() = default;
	virtual ~CMirrorBackObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ================================================= CBilboardObject =================================================

class CBilboardObject
{
protected:
	shared_ptr<CMaterial>				m_Material{};

	ComPtr<ID3D12Resource>				m_D3D12VertexBuffer{};
	ComPtr<ID3D12Resource>				m_D3D12VertexUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW			m_D3D12VertexBufferView{};

public:
	CBilboardObject() = default;
	virtual ~CBilboardObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	void ReleaseUploadBuffers();

	void SetMaterial(const shared_ptr<CMaterial>& Material);
	void SetTexture(const shared_ptr<CTexture>& Texture);
};

// ================================================= CBackgroundObject =================================================

class CBackgroundObject : public CBilboardObject
{
public:
	CBackgroundObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual ~CBackgroundObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ================================================= CButtonObject ==================================================

class CButtonObject : public CBilboardObject
{
public:
	enum { NONE, GAME_START, EXIT };

private:
	UINT								m_ObjectCount{};
	CSpriteBilboardMesh*				m_MappedMeshes{};

public:
	CButtonObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT ObjectCount);
	virtual ~CButtonObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	int ActiveButton(int ScreenX, int ScreenY);
};

// ================================================= CSkyBoxObject =================================================

class CSkyBoxObject : public CBilboardObject
{
public:
	CSkyBoxObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual ~CSkyBoxObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ================================================= CTreeObject =================================================

class CTreeObject : public CBilboardObject
{
public:
	CTreeObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, void* TerrainContext);
	virtual ~CTreeObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};

// ================================================= CHpBarObject =================================================

class CHpBarObject : public CBilboardObject
{
private:
	UINT								m_ObjectCount{};
	CBilboardMesh*						m_MappedMeshes{};

public:
	CHpBarObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT ObjectCount);
	virtual ~CHpBarObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	CBilboardMesh* GetMappedMesh();
};

// ================================================= CExplodedObject =================================================

class CExplodedEnemyObject : public CBilboardObject
{
private:
	UINT								m_ObjectCount{};
	CSpriteBilboardMesh*				m_MappedMeshes{};

public:
	CExplodedEnemyObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT ObjectCount);
	virtual ~CExplodedEnemyObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	CSpriteBilboardMesh* GetMappedMesh();
};

// ================================================= CSmokeObject =================================================

class CSmokeObject : public CBilboardObject
{
private:
	UINT								m_ObjectCount{};
	CSpriteBilboardMesh*				m_MappedMeshes{};

public:
	CSmokeObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT ObjectCount);
	virtual ~CSmokeObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	CSpriteBilboardMesh* GetMappedMesh();
};

// ================================================= CParticleObject =================================================

class CParticleObject : public CObject
{
public:
	CParticleObject(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual ~CParticleObject();

	virtual void ReleaseUploadBuffers();

	virtual void PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};
