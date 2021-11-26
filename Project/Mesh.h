#pragma once

struct MeshInfo
{
	XMFLOAT3						m_Center{};
	XMFLOAT3						m_Extents{};
	vector<XMFLOAT3>				m_Positions{};
	vector<XMFLOAT3>				m_Normals{};
	vector<XMFLOAT2>				m_UVs{};
	vector<UINT>					m_Indices{};
};

// ================================================= CVertexP =================================================

class CVertexP
{
private:
	XMFLOAT3						m_Position{};

public:
	CVertexP(const XMFLOAT3& Position);
	~CVertexP() = default;
};

// ================================================= CVertexPU =================================================

class CVertexPU : public CVertexP
{
private:
	XMFLOAT2						m_UV0{};

public:
	CVertexPU(const XMFLOAT3& Position, const XMFLOAT2& UV0);
	~CVertexPU() = default;
};

// ================================================= CVertexPN =================================================

class CVertexPN : public CVertexP
{
private:
	XMFLOAT3						m_Normal{};

public:
	CVertexPN(const XMFLOAT3& Position, const XMFLOAT3& Normal);
	~CVertexPN() = default;
};

// ================================================= CVertexPNU =================================================

class CVertexPNU : public CVertexPN
{
private:
	XMFLOAT2						m_UV0{};

public:
	CVertexPNU(const XMFLOAT3& Position, const XMFLOAT3& Normal, const XMFLOAT2& UV0);
	~CVertexPNU() = default;
};

// ================================================= CVertexPNU2 =================================================

class CVertexPNU2 : public CVertexPNU
{
private:
	XMFLOAT2						m_UV1{};

public:
	CVertexPNU2(const XMFLOAT3& Position, const XMFLOAT3& Normal, const XMFLOAT2& UV0, const XMFLOAT2& UV1);
	~CVertexPNU2() = default;
};

// ================================================= CMesh =================================================

class CMesh
{
protected:
	D3D12_PRIMITIVE_TOPOLOGY		m_D3D12PrimitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	vector<XMFLOAT3>				m_Positions{};
	ComPtr<ID3D12Resource>			m_D3D12VertexBuffer{};
	ComPtr<ID3D12Resource>			m_D3D12VertexUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW		m_D3D12VertexBufferView{};

	vector<UINT>					m_Indices{};
	ComPtr<ID3D12Resource>			m_D3D12IndexBuffer{};
	ComPtr<ID3D12Resource>			m_D3D12IndexUploadBuffer{};
	D3D12_INDEX_BUFFER_VIEW			m_D3D12IndexBufferView{};

	BoundingOrientedBox				m_BoundingBox{};

public:
	CMesh() = default;
	CMesh(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, MeshInfo* MeshInfo);
	~CMesh() = default;

	void ReleaseUploadBuffers();

	void SetBoundingBox(const BoundingOrientedBox& BoundingBox);
	const BoundingOrientedBox& GetBoundingBox() const;

	UINT CheckRayIntersection(const XMFLOAT3& RayOrigin, const XMFLOAT3& RayDirection, float& NearestHitDistance);

	void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
};

// ================================================= CHeightMapImage ================================================= 

class CHeightMapImage
{
private:
	BYTE*							m_Pixels{};

	int								m_Width{};
	int								m_Length{};
	XMFLOAT3						m_Scale{};

public:
	CHeightMapImage(LPCTSTR FileName, int Width, int Length, const XMFLOAT3& Scale);
	~CHeightMapImage();

	BYTE* GetPixels();

	int GetWidth() const;
	int GetLength() const;
	const XMFLOAT3& GetScale() const;

	float GetHeight(float Xpos, float Zpos);
	XMFLOAT3 GetNormal(float Xpos, float Zpos);
};

// ================================================= CHeightMapMesh ================================================= 

class CHeightMapMesh : public CMesh
{
protected:
	int								m_Width{};
	int								m_Length{};
	XMFLOAT3						m_Scale{};

public:
	CHeightMapMesh(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, int XStart, int ZStart, int Width, int Length, const XMFLOAT3& Scale, void* Context);
	virtual ~CHeightMapMesh() = default;

	int GetWidth() const;
	int GetLength() const;
	const XMFLOAT3& GetScale() const;

	float OnGetHeight(int Xpos, int Zpos, void* Context);
};

// ================================================= CRectMesh ================================================= 

class CRectMesh : public CMesh
{
public:
	CRectMesh(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, float Width, float Height, float Depth, float XPostion, float YPosition, float ZPosition);
	virtual ~CRectMesh() = default;
};

// ================================================= CBilboardMesh ================================================= 

class CBilboardMesh
{
private:
	XMFLOAT3						m_Position{};
	XMFLOAT3						m_Direction{};
	XMFLOAT2						m_Size{};

public:
	CBilboardMesh(const XMFLOAT3& Position, const XMFLOAT2& Size);
	~CBilboardMesh() = default;

	void SetPosition(const XMFLOAT3& Position);
	const XMFLOAT3& GetPosition() const;

	void SetDirection(const XMFLOAT3& Direction);
	const XMFLOAT3& GetDirection() const;

	void SetSize(const XMFLOAT2& Size);
	const XMFLOAT2& GetSize() const;
};

// ================================================= CSpriteBilboardMesh ================================================= 

#define SPRIETE_LOOP	-1

class CSpriteBilboardMesh : public CBilboardMesh
{
private:
	UINT							m_SpriteRow{};
	UINT							m_SpriteColumn{};

	float							m_FrameTime{};

public:
	CSpriteBilboardMesh(const XMFLOAT3& Position, const XMFLOAT2& Size);
	CSpriteBilboardMesh(const XMFLOAT3& Position, const XMFLOAT2& Size, UINT SpriteRow, UINT SpriteColumn, float FrameTime);
	~CSpriteBilboardMesh() = default;

	void SetSpriteRow(UINT SpriteRow);
	UINT GetSpriteRow() const;

	void SetSpriteColumn(UINT SpriteColumn);
	UINT GetSpriteColumn() const;

	void SetFrameTime(float FrameTime);
	float GetFrameTime() const;

	void IncreaseFrameTime(float ElpasedTime, int LoopCount);
	void IncreaseFrameTime(float ElapsedTime, float Duration);
};
