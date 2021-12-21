#include "stdafx.h"
#include "Mesh.h"

// ================================================= CVertexP =================================================

CVertexP::CVertexP(const XMFLOAT3& Position) :
	m_Position{ Position }
{

}

// ================================================= CVertexPU =================================================

CVertexPU::CVertexPU(const XMFLOAT3& Position, const XMFLOAT2& UV0) : CVertexP{ Position },
	m_UV0{ UV0 }
{

}

// ================================================= CVertexPN =================================================

CVertexPN::CVertexPN(const XMFLOAT3& Position, const XMFLOAT3& Normal) : CVertexP{ Position },
	m_Normal{ Normal }
{

}

// ================================================= CVertexPNU =================================================

CVertexPNU::CVertexPNU(const XMFLOAT3& Position, const XMFLOAT3& Normal, const XMFLOAT2& UV0) : CVertexPN{ Position, Normal },
	m_UV0{ UV0 }
{

}

// ================================================= CVertexPNUU =================================================

CVertexPNU2::CVertexPNU2(const XMFLOAT3& Position, const XMFLOAT3& Normal, const XMFLOAT2& UV0, const XMFLOAT2& UV1) : CVertexPNU{ Position, Normal, UV0 },
	m_UV1{ UV1 }
{

}

// ================================================= CMesh ================================================= 

CMesh::CMesh(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, MeshInfo* MeshInfo)
{
	m_Positions = move(MeshInfo->m_Positions);

	vector<CVertexPNU> Vertices{};

	Vertices.reserve(m_Positions.size());

	for (UINT i = 0; i < m_Positions.size(); ++i)
	{
		Vertices.emplace_back(m_Positions[i], MeshInfo->m_Normals[i], MeshInfo->m_UVs[i]);
	}

	m_D3D12VertexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Vertices.data(), sizeof(CVertexPNU) * (UINT)Vertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12VertexUploadBuffer.GetAddressOf());
	
	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(CVertexPNU);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(CVertexPNU) * (UINT)Vertices.size();

	// Indices
	m_Indices = move(MeshInfo->m_Indices);

	m_D3D12IndexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, m_Indices.data(), sizeof(UINT) * (UINT)m_Indices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_D3D12IndexUploadBuffer.GetAddressOf());

	m_D3D12IndexBufferView.BufferLocation = m_D3D12IndexBuffer->GetGPUVirtualAddress();
	m_D3D12IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_D3D12IndexBufferView.SizeInBytes = sizeof(UINT) * (UINT)m_Indices.size();

	// BoundingBox
	m_BoundingBox.Center = MeshInfo->m_Center;
	m_BoundingBox.Extents = MeshInfo->m_Extents;
}

void CMesh::ReleaseUploadBuffers()
{
	if (m_D3D12VertexUploadBuffer)
	{
		m_D3D12VertexUploadBuffer->Release();
	}

	if (m_D3D12IndexUploadBuffer)
	{
		m_D3D12IndexUploadBuffer->Release();

	}

	m_D3D12VertexUploadBuffer = nullptr;
	m_D3D12IndexUploadBuffer = nullptr;
}

void CMesh::SetBoundingBox(const BoundingOrientedBox& BoundingBox)
{
	m_BoundingBox = BoundingBox;
}

const BoundingOrientedBox& CMesh::GetBoundingBox() const
{
	return m_BoundingBox;
}

UINT CMesh::CheckRayIntersection(const XMFLOAT3& RayOrigin, const XMFLOAT3& RayDirection, float& NearestHitDistance)
{
	// 하나의 메쉬에서 광선은 여러 개의 삼각형과 교차할 수 있으므로 교차하는 삼각형들 중 가장 가까운 삼각형을 찾는다.
	UINT Intersections{};
	int Offset{ (m_D3D12PrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1 };

	// 메쉬의 프리미티브(삼각형)들의 개수이다.
	// 삼각형 리스트인 경우 (정점의 개수 / 3) 또는 (인덱스의 개수 / 3), 삼각형 스트립의 경우 (정점의 개수 - 2) 또는 (인덱스의 개수 – 2)이다.
	UINT Primitives{ (m_D3D12PrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (UINT)(m_Indices.size() / 3) : (UINT)(m_Indices.size() - 2) };

	// 광선은 모델 좌표계로 표현된다.
	// 모델 좌표계의 광선과 메쉬의 바운딩 박스(모델 좌표계)와의 교차를 검사한다.
	bool Intersected{ m_BoundingBox.Intersects(XMLoadFloat3(&RayOrigin), XMLoadFloat3(&RayDirection), NearestHitDistance) };

	// 모델 좌표계의 광선이 메쉬의 바운딩 박스와 교차하면 메쉬와의 교차를 검사한다.
	if (Intersected)
	{
		// 메쉬의 모든 프리미티브(삼각형)들에 대하여 픽킹 광선과의 충돌을 검사한다.
		// 충돌하는 모든 삼각형을 찾아 광선의 시작점(실제로는 카메라 좌표계의 원점)에 가장 가까운 삼각형을 찾는다.
		float NearHitDistance{ FLT_MAX };
		float HitDistance{};

		for (UINT i = 0; i < Primitives; ++i)
		{
			XMVECTOR V0{ XMLoadFloat3(&m_Positions[m_Indices[Offset * i]]) };
			XMVECTOR V1{ XMLoadFloat3(&m_Positions[m_Indices[Offset * i + 1]]) };
			XMVECTOR V2{ XMLoadFloat3(&m_Positions[m_Indices[Offset * i + 2]]) };

			Intersected = TriangleTests::Intersects(XMLoadFloat3(&RayOrigin), XMLoadFloat3(&RayDirection), V0, V1, V2, HitDistance);

			if (Intersected)
			{
				if (HitDistance < NearHitDistance)
				{
					NearestHitDistance = NearHitDistance = HitDistance;
				}

				++Intersections;
			}
		}
	}

	return Intersections;
}

void CMesh::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT PSONum)
{

}

void CMesh::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12GraphicsCommandList->IASetPrimitiveTopology(m_D3D12PrimitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12VertexBufferView };

	D3D12GraphicsCommandList->IASetVertexBuffers(0, 1, VertexBufferViews);

	if (m_D3D12IndexBuffer)
	{
		D3D12GraphicsCommandList->IASetIndexBuffer(&m_D3D12IndexBufferView);
		D3D12GraphicsCommandList->DrawIndexedInstanced((UINT)m_Indices.size(), 1, 0, 0, 0);
	}
	else
	{
		D3D12GraphicsCommandList->DrawInstanced((UINT)m_Positions.size(), 1, 0, 0);
	}
}

void CMesh::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT PSONum)
{

}

void CMesh::PostRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT PSONum)
{

}

// ================================================= CHeightMapImage ================================================= 

CHeightMapImage::CHeightMapImage(LPCTSTR FileName, int Width, int Length, const XMFLOAT3& Scale) :
	m_Width{ Width },
	m_Length{ Length },
	m_Scale{ Scale }
{
	BYTE* HeightMapPixels{ new BYTE[m_Width * m_Length]{} };
	HANDLE FileHandle{ CreateFile(FileName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, nullptr) };
	DWORD BytesRead{};

	ReadFile(FileHandle, HeightMapPixels, m_Width * m_Length, &BytesRead, nullptr);
	CloseHandle(FileHandle);

	m_Pixels = new BYTE[m_Width * m_Length]{};

	for (int y = 0; y < m_Length; ++y)
	{
		for (int x = 0; x < m_Width; ++x)
		{
			m_Pixels[x + ((m_Length - 1 - y) * m_Width)] = HeightMapPixels[x + (y * m_Width)];
		}
	}
	
	if (HeightMapPixels)
	{
		delete[] HeightMapPixels;
	}
}

CHeightMapImage::~CHeightMapImage()
{
	if (m_Pixels)
	{
		delete[] m_Pixels;
		m_Pixels = nullptr;
	}
}

BYTE* CHeightMapImage::GetPixels()
{
	return m_Pixels;
}

int CHeightMapImage::GetWidth() const
{
	return m_Width;
}

int CHeightMapImage::GetLength() const
{
	return m_Length;
}

const XMFLOAT3& CHeightMapImage::GetScale() const
{
	return m_Scale;
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER

float CHeightMapImage::GetHeight(float Xpos, float Zpos)
{
	Xpos /= m_Scale.x;
	Zpos /= m_Scale.z;

	if ((Xpos < 0.0f) || (Zpos < 0.0f) || (Xpos >= m_Width) || (Zpos >= m_Length))
	{
		return 0.0f;
	}

	int IntXpos{ (int)Xpos };
	int IntZpos{ (int)Zpos };
	float XPercent{ Xpos - IntXpos };
	float ZPercent{ Zpos - IntZpos };

	float BottomLeft{ (float)m_Pixels[IntXpos + (IntZpos * m_Width)] };
	float BottomRight{ (float)m_Pixels[(IntXpos + 1) + (IntZpos * m_Width)] };
	float TopLeft{ (float)m_Pixels[IntXpos + ((IntZpos + 1) * m_Width)] };
	float TopRight{ (float)m_Pixels[(IntXpos + 1) + ((IntZpos + 1) * m_Width)] };

#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	// z-좌표가 1, 3, 5, ...인 경우 인덱스가 오른쪽에서 왼쪽으로 나열된다.
	if ((IntZpos % 2) == 1)
	{
		if (ZPercent >= XPercent)
		{
			BottomRight = BottomLeft + (TopRight - TopLeft);
		}
		else
		{
			TopLeft = TopRight + (BottomLeft - BottomRight);
		}
	}
	// z-좌표가 0, 2, 4, ...인 경우 인덱스가 왼쪽에서 오른쪽으로 나열된다.
	else
	{
		if (ZPercent < 1.0f - XPercent)
		{
			TopRight = TopLeft + (BottomRight - BottomLeft);
		}
		else
		{
			BottomLeft = TopLeft + (BottomRight - TopRight);
		}
	}
#endif

	float TopHeight{ TopLeft * (1 - XPercent) + TopRight * XPercent };
	float BottomHeight{ BottomLeft * (1 - XPercent) + BottomRight * XPercent };
	float Height{ BottomHeight * (1 - ZPercent) + TopHeight * ZPercent };

	return Height;
}

XMFLOAT3 CHeightMapImage::GetNormal(float Xpos, float Zpos)
{
	if ((Xpos < 0.0f) || (Zpos < 0.0f) || (Xpos >= m_Width) || (Zpos >= m_Length))
	{
		return XMFLOAT3(0.0f, 1.0f, 0.0f);
	}

	UINT Index{ (UINT)(Xpos + (Zpos * m_Width)) };
	int AddToIndexX{ (Xpos < m_Width - 1) ? 1 : -1 };
	int AddToIndexZ{ (Zpos < m_Length - 1) ? (int)m_Width : -(int)m_Width };

	float Ypos1{ (float)m_Pixels[Index] * m_Scale.y };
	float Ypos2{ (float)m_Pixels[Index + AddToIndexX] * m_Scale.y };
	float Ypos3{ (float)m_Pixels[Index + AddToIndexZ] * m_Scale.y };

	XMFLOAT3 Edge1{ XMFLOAT3(0.0f, Ypos3 - Ypos1, m_Scale.z) };
	XMFLOAT3 Edge2{ XMFLOAT3(m_Scale.x, Ypos2 - Ypos1, 0.0f) };
	XMFLOAT3 Normal{ Vector3::CrossProduct(Edge1, Edge2, true) };

	return Normal;
}

// ================================================= CHeightMapMesh ================================================= 

CHeightMapMesh::CHeightMapMesh(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, int XStart, int ZStart, int Width, int Length, const XMFLOAT3& Scale, void* Context) :
	m_Width{ Width },
	m_Length{ Length },
	m_Scale{ Scale }
{
	CHeightMapImage* HeightMapImage{ (CHeightMapImage*)Context };
	int ImageWidth{ HeightMapImage->GetWidth() };
	int ImageLength{ HeightMapImage->GetLength() };
	vector<CVertexPNU2> Vertices{};

#ifdef TERRAIN_TESSELLATION
	m_D3D12PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
	m_Positions.reserve((size_t)(Width * Length / 12));
	Vertices.reserve((size_t)(Width * Length / 12));

	UINT Increment{ 4 };

	for (int z = (ZStart + Length - 1); z >= ZStart; z -= Increment)
	{
		for (int x = XStart; x < (XStart + Width); x += Increment)
		{
			float Height{ OnGetHeight(x, z, HeightMapImage) };
			XMFLOAT3 Position{ m_Scale.x * x, Height, m_Scale.z * z };
			XMFLOAT3 Normal{ HeightMapImage->GetNormal((float)x, (float)z) };
			XMFLOAT2 UV0{ (float)(x) / (float)(ImageWidth - 1), (float)(ImageLength - 1 - z) / (float)(ImageLength - 1) };
			XMFLOAT2 UV1{ (float)(2.5f * x) / (XStart + Width), (float)(2.5f * z) / (ZStart + Length) };

			Vertices.emplace_back(Position, Normal, UV0, UV1);
			m_Positions.push_back(Position);
		}
	}

#else
	m_D3D12PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_Positions.reserve(Width * Length);
	Vertices.reserve(Width * Length);

	for (int z = ZStart; z < (ZStart + Length); ++z)
	{
		for (int x = XStart; x < (XStart + Width); ++x)
		{
			float Height{ OnGetHeight(x, z, HeightMapImage) };
			XMFLOAT3 Position{ m_Scale.x * x, Height, m_Scale.z * z };
			XMFLOAT3 Normal{ HeightMapImage->GetNormal((float)x, (float)z) };
			XMFLOAT2 UV0{ (float)(x) / (float)(ImageWidth - 1), (float)(ImageLength - 1 - z) / (float)(ImageLength - 1) };
			XMFLOAT2 UV1{ (float)(5.0f * x) / (XStart + Width), (float)(5.0f * z) / (ZStart + Length) };

			Vertices.emplace_back(Position, Normal, UV0, UV1);
			m_Positions.push_back(Position);
		}
	}
#endif

	m_D3D12VertexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Vertices.data(), sizeof(CVertexPNU2) * (UINT)Vertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12VertexUploadBuffer.GetAddressOf());

	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(CVertexPNU2);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(CVertexPNU2) * (UINT)Vertices.size();

#ifndef TERRAIN_TESSELLATION
	m_Indices.reserve((2 * Width)* (Length - 1) + (Length - 1) - 1);

	for (int z = 0; z < Length - 1; ++z)
	{
		if ((z % 2) == 0)
		{
			for (int x = 0; x < (int)Width; x++)
			{
				if ((x == 0) && (z > 0))
				{
					m_Indices.push_back((UINT)(x + (z * Width)));
				}

				m_Indices.push_back((UINT)(x + (z * Width)));
				m_Indices.push_back((UINT)(x + (z * Width)) + Width);
			}
		}
		else
		{
			for (int x = Width - 1; x >= 0; --x)
			{
				if (x == (Width - 1))
				{
					m_Indices.push_back((UINT)(x + (z * Width)));
				}

				m_Indices.push_back((UINT)(x + (z * Width)));
				m_Indices.push_back((UINT)(x + (z * Width)) + Width);
			}
		}
	}

	m_D3D12IndexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, m_Indices.data(), sizeof(UINT) * (UINT)m_Indices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_D3D12IndexUploadBuffer.GetAddressOf());

	m_D3D12IndexBufferView.BufferLocation = m_D3D12IndexBuffer->GetGPUVirtualAddress();
	m_D3D12IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_D3D12IndexBufferView.SizeInBytes = sizeof(UINT) * (UINT)m_Indices.size();
#endif
}

int CHeightMapMesh::GetWidth() const
{
	return m_Width;
}

int CHeightMapMesh::GetLength() const
{
	return m_Length;
}

const XMFLOAT3& CHeightMapMesh::GetScale() const
{
	return m_Scale;
}

float CHeightMapMesh::OnGetHeight(int Xpos, int Zpos, void* Context)
{
	CHeightMapImage* HeightMapImage{ (CHeightMapImage*)Context };
	BYTE* HeightMapPixels{ HeightMapImage->GetPixels() };
	XMFLOAT3 Scale{ HeightMapImage->GetScale() };
	int Width = { HeightMapImage->GetWidth() };
	float Height{ HeightMapPixels[Xpos + (Zpos * Width)] * Scale.y };

	return Height;
}

// ================================================= CRectMesh ================================================= 

CRectMesh::CRectMesh(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, float Width, float Height, float Depth, float XPosition, float YPosition, float ZPosition)
{
	vector<CVertexPNU> Vertices{};

	Vertices.reserve(6);
	m_Positions.reserve(6);

	float Xpos{ 0.5f * Width + XPosition };
	float Ypos{ 0.5f * Height + YPosition };
	float Zpos{ 0.5f * Depth + ZPosition };

	if (IsZero(Width))
	{
		if (XPosition >= 0.0f)
		{
			Vertices.emplace_back(XMFLOAT3(Xpos, +Ypos, -Zpos), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
			Vertices.emplace_back(XMFLOAT3(Xpos, -Ypos, -Zpos), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(Xpos, -Ypos, +Zpos), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(Xpos, -Ypos, +Zpos), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(Xpos, +Ypos, +Zpos), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
			Vertices.emplace_back(XMFLOAT3(Xpos, +Ypos, -Zpos), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
			
			m_Positions.emplace_back(Xpos, +Ypos, -Zpos);
			m_Positions.emplace_back(Xpos, -Ypos, -Zpos);
			m_Positions.emplace_back(Xpos, -Ypos, +Zpos);
			m_Positions.emplace_back(Xpos, -Ypos, +Zpos);
			m_Positions.emplace_back(Xpos, +Ypos, +Zpos);
			m_Positions.emplace_back(Xpos, +Ypos, -Zpos);
		}
		else
		{
			Vertices.emplace_back(XMFLOAT3(Xpos, +Ypos, +Zpos), XMFLOAT3(+1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
			Vertices.emplace_back(XMFLOAT3(Xpos, -Ypos, +Zpos), XMFLOAT3(+1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(Xpos, -Ypos, -Zpos), XMFLOAT3(+1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(Xpos, -Ypos, -Zpos), XMFLOAT3(+1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(Xpos, +Ypos, -Zpos), XMFLOAT3(+1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
			Vertices.emplace_back(XMFLOAT3(Xpos, +Ypos, +Zpos), XMFLOAT3(+1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));

			m_Positions.emplace_back(Xpos, +Ypos, +Zpos);
			m_Positions.emplace_back(Xpos, -Ypos, +Zpos);
			m_Positions.emplace_back(Xpos, -Ypos, -Zpos);
			m_Positions.emplace_back(Xpos, -Ypos, -Zpos);
			m_Positions.emplace_back(Xpos, +Ypos, -Zpos);
			m_Positions.emplace_back(Xpos, +Ypos, +Zpos);
		}
	}
	else if (IsZero(Height))
	{
		if (YPosition >= 0.0f)
		{
			Vertices.emplace_back(XMFLOAT3(+Xpos, Ypos, -Zpos), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
			Vertices.emplace_back(XMFLOAT3(+Xpos, Ypos, +Zpos), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(-Xpos, Ypos, +Zpos), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(-Xpos, Ypos, +Zpos), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(-Xpos, Ypos, -Zpos), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
			Vertices.emplace_back(XMFLOAT3(+Xpos, Ypos, -Zpos), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));

			m_Positions.emplace_back(+Xpos, Ypos, -Zpos);
			m_Positions.emplace_back(+Xpos, Ypos, +Zpos);
			m_Positions.emplace_back(-Xpos, Ypos, +Zpos);
			m_Positions.emplace_back(-Xpos, Ypos, +Zpos);
			m_Positions.emplace_back(-Xpos, Ypos, -Zpos);
			m_Positions.emplace_back(+Xpos, Ypos, -Zpos);
		}
		else
		{
			Vertices.emplace_back(XMFLOAT3(+Xpos, Ypos, +Zpos), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));
			Vertices.emplace_back(XMFLOAT3(+Xpos, Ypos, -Zpos), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(-Xpos, Ypos, -Zpos), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(-Xpos, Ypos, -Zpos), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(-Xpos, Ypos, +Zpos), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
			Vertices.emplace_back(XMFLOAT3(+Xpos, Ypos, +Zpos), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));

			m_Positions.emplace_back(+Xpos, Ypos, +Zpos);
			m_Positions.emplace_back(+Xpos, Ypos, -Zpos);
			m_Positions.emplace_back(-Xpos, Ypos, -Zpos);
			m_Positions.emplace_back(-Xpos, Ypos, -Zpos);
			m_Positions.emplace_back(-Xpos, Ypos, +Zpos);
			m_Positions.emplace_back(+Xpos, Ypos, +Zpos);
		}
	}
	else if (IsZero(Depth))
	{
		if (ZPosition >= 0.0f)
		{
			Vertices.emplace_back(XMFLOAT3(+Xpos, +Ypos, Zpos), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));
			Vertices.emplace_back(XMFLOAT3(+Xpos, -Ypos, Zpos), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(-Xpos, -Ypos, Zpos), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(-Xpos, -Ypos, Zpos), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(-Xpos, +Ypos, Zpos), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f));
			Vertices.emplace_back(XMFLOAT3(+Xpos, +Ypos, Zpos), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));

			m_Positions.emplace_back(+Xpos, +Ypos, Zpos);
			m_Positions.emplace_back(+Xpos, -Ypos, Zpos);
			m_Positions.emplace_back(-Xpos, -Ypos, Zpos);
			m_Positions.emplace_back(-Xpos, -Ypos, Zpos);
			m_Positions.emplace_back(-Xpos, +Ypos, Zpos);
			m_Positions.emplace_back(+Xpos, +Ypos, Zpos);
		}
		else
		{
			Vertices.emplace_back(XMFLOAT3(-Xpos, +Ypos, Zpos), XMFLOAT3(0.0f, 0.0f, +1.0f), XMFLOAT2(1.0f, 0.0f));
			Vertices.emplace_back(XMFLOAT3(-Xpos, -Ypos, Zpos), XMFLOAT3(0.0f, 0.0f, +1.0f), XMFLOAT2(1.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(+Xpos, -Ypos, Zpos), XMFLOAT3(0.0f, 0.0f, +1.0f), XMFLOAT2(0.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(+Xpos, -Ypos, Zpos), XMFLOAT3(0.0f, 0.0f, +1.0f), XMFLOAT2(0.0f, 1.0f));
			Vertices.emplace_back(XMFLOAT3(+Xpos, +Ypos, Zpos), XMFLOAT3(0.0f, 0.0f, +1.0f), XMFLOAT2(0.0f, 0.0f));
			Vertices.emplace_back(XMFLOAT3(-Xpos, +Ypos, Zpos), XMFLOAT3(0.0f, 0.0f, +1.0f), XMFLOAT2(1.0f, 0.0f));

			m_Positions.emplace_back(-Xpos, +Ypos, Zpos);
			m_Positions.emplace_back(-Xpos, -Ypos, Zpos);
			m_Positions.emplace_back(+Xpos, -Ypos, Zpos);
			m_Positions.emplace_back(+Xpos, -Ypos, Zpos);
			m_Positions.emplace_back(+Xpos, +Ypos, Zpos);
			m_Positions.emplace_back(-Xpos, +Ypos, Zpos);
		}
	}

	m_D3D12VertexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Vertices.data(), sizeof(CVertexPNU) * (UINT)Vertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12VertexUploadBuffer.GetAddressOf());

	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(CVertexPNU);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(CVertexPNU) * (UINT)Vertices.size();

	m_BoundingBox.Extents = { 0.5f * Width, 0.5f * Height, 0.5f * Depth };
};

// ================================================= CBilboard ================================================= 

CBilboardMesh::CBilboardMesh(const XMFLOAT3& Position, const XMFLOAT3& Direction, const XMFLOAT2& Size) :
	m_Position{ Position },
	m_Direction{ Direction },
	m_Size{ Size }
{

}

void CBilboardMesh::SetPosition(const XMFLOAT3& Position)
{
	m_Position = Position;
}

const XMFLOAT3& CBilboardMesh::GetPosition() const
{
	return m_Position;
}

void CBilboardMesh::SetDirection(const XMFLOAT3& Direction)
{
	m_Direction = Direction;
}

const XMFLOAT3& CBilboardMesh::GetDirection() const
{
	return m_Direction;
}

void CBilboardMesh::SetSize(const XMFLOAT2& Size)
{
	m_Size = Size;
}

const XMFLOAT2& CBilboardMesh::GetSize() const
{
	return m_Size;
}

// ================================================= CSpriteBilboardMesh ================================================= 

CSpriteBilboardMesh::CSpriteBilboardMesh(const XMFLOAT3& Position, const XMFLOAT3& Direction, const XMFLOAT2& Size, UINT SpriteRow, UINT SpriteColumn, float FrameTime) : CBilboardMesh{ Position, Direction, Size },
	m_SpriteRow{ SpriteRow },
	m_SpriteColumn{ SpriteColumn },
	m_FrameTime{ FrameTime }
{

}

void CSpriteBilboardMesh::SetSpriteRow(UINT SpriteRow)
{
	m_SpriteRow = SpriteRow;
}

UINT CSpriteBilboardMesh::GetSpriteRow() const
{
	return m_SpriteRow;
}

void CSpriteBilboardMesh::SetSpriteColumn(UINT SpriteColumn)
{
	m_SpriteColumn = SpriteColumn;
}

UINT CSpriteBilboardMesh::GetSpriteColumn() const
{
	return m_SpriteColumn;
}

void CSpriteBilboardMesh::SetFrameTime(float FrameTime)
{
	m_FrameTime = FrameTime;
}

float CSpriteBilboardMesh::GetFrameTime() const
{
	return m_FrameTime;
}

void CSpriteBilboardMesh::IncreaseFrameTime(float ElapsedTime, int LoopCount)
{
	m_FrameTime += ElapsedTime;

	if (LoopCount == SPRIETE_LOOP)
	{
		return;
	}
	else if (m_FrameTime >= LoopCount * (m_SpriteRow * m_SpriteColumn))
	{
		m_FrameTime = -1.0f;
	}
}

void CSpriteBilboardMesh::IncreaseFrameTime(float ElapsedTime, float Duration)
{
	m_FrameTime += ElapsedTime;

	if (m_FrameTime >= Duration)
	{
		m_FrameTime = -1.0f;
	}
}

// ================================================= CParticleVertex =================================================

CParticleVertex::CParticleVertex(const XMFLOAT3& Position, const XMFLOAT3& Direction, const XMFLOAT2& Size, UINT Level) : CBilboardMesh { Position, Direction, Size },
	m_Level{ Level }
{

}

// ================================================= CParticleMesh =================================================

CParticleMesh::CParticleMesh(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	CreateVertexBuffer(D3D12Device, D3D12GraphicsCommandList);
	CreateStreamOutputBuffer(D3D12Device, D3D12GraphicsCommandList);
}

void CParticleMesh::CreateVertexBuffer(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	m_D3D12PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	vector<CParticleVertex> Vertices{};

	Vertices.reserve(9);

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Vertices.emplace_back(XMFLOAT3(42.5f + 85.0f * j, 120.0f, 42.5f + 85.0f * i), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), 1);
		}
	}

	m_VerticeCount = (UINT)Vertices.size();

	m_D3D12VertexBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Vertices.data(), sizeof(CParticleVertex) * m_VerticeCount,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12VertexUploadBuffer.GetAddressOf());

	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(CParticleVertex);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(CParticleVertex) * m_VerticeCount;
}

void CParticleMesh::CreateStreamOutputBuffer(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	m_MaxParticles = MAX_PARTICLES;

	m_D3D12StreamOutputBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(CParticleVertex) * m_MaxParticles, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_STREAM_OUT, nullptr);
	m_D3D12DrawBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(CParticleVertex) * m_MaxParticles, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);

	UINT64 BufferFilledSize{};

	// 디폴트 타입 리소스
	m_D3D12DefaultBufferFilledSize = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, &BufferFilledSize, sizeof(UINT64), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_STREAM_OUT, nullptr);

	// 업로드 타입 리소스
	m_D3D12UploadBufferFilledSize = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(UINT64), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	m_D3D12UploadBufferFilledSize->Map(0, nullptr, (void**)&m_UploadBufferFilledSize);

	// 리드백 타입 리소스
	m_D3D12ReadBackBufferFilledSize = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(UINT64), D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_STATE_COPY_DEST, nullptr);

	m_D3D12StreamOutputBufferView.BufferLocation = m_D3D12StreamOutputBuffer->GetGPUVirtualAddress();
	m_D3D12StreamOutputBufferView.SizeInBytes = sizeof(CParticleVertex) * m_MaxParticles;
	m_D3D12StreamOutputBufferView.BufferFilledSizeLocation = m_D3D12DefaultBufferFilledSize->GetGPUVirtualAddress();
}

void CParticleMesh::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT PSONum)
{
	if (PSONum == 0)
	{
		if (m_IsStart)
		{
			m_IsStart = false;
			m_VerticeCount = 9;

			m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
			m_D3D12VertexBufferView.StrideInBytes = sizeof(CParticleVertex);
			m_D3D12VertexBufferView.SizeInBytes = sizeof(CParticleVertex) * m_VerticeCount;
		}

		// 시작 인덱스를 0으로 만든다.
		// 이때, 업로드 타입의 리소스를 이용하여, 디폴트 타입의 리소스 값에 복사한다.
		*m_UploadBufferFilledSize = 0;

		CD3DX12_RESOURCE_BARRIER ResourceBarrier{};

		ResourceBarrier = ResourceBarrier.Transition(m_D3D12DefaultBufferFilledSize.Get(), D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		D3D12GraphicsCommandList->ResourceBarrier(1, &ResourceBarrier);

		D3D12GraphicsCommandList->CopyResource(m_D3D12DefaultBufferFilledSize.Get(), m_D3D12UploadBufferFilledSize.Get());
		
		ResourceBarrier = ResourceBarrier.Transition(m_D3D12DefaultBufferFilledSize.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		D3D12GraphicsCommandList->ResourceBarrier(1, &ResourceBarrier);
	}
	else if (PSONum == 1)
	{
		m_D3D12VertexBufferView.BufferLocation = m_D3D12DrawBuffer->GetGPUVirtualAddress();
		m_D3D12VertexBufferView.StrideInBytes = sizeof(CParticleVertex);
		m_D3D12VertexBufferView.SizeInBytes = sizeof(CParticleVertex) * m_VerticeCount;
	}
}

void CParticleMesh::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12VertexBufferView };

	D3D12GraphicsCommandList->IASetVertexBuffers(0, 1, VertexBufferViews);
	D3D12GraphicsCommandList->IASetPrimitiveTopology(m_D3D12PrimitiveTopology);
	D3D12GraphicsCommandList->DrawInstanced(m_VerticeCount, 1, 0, 0);
}

void CParticleMesh::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT PSONum)
{
	if (PSONum == 0)
	{
		D3D12_STREAM_OUTPUT_BUFFER_VIEW StreamOutputBufferViews[] = { m_D3D12StreamOutputBufferView };
		
		D3D12GraphicsCommandList->SOSetTargets(0, 1, StreamOutputBufferViews);

		Render(D3D12GraphicsCommandList);

		CD3DX12_RESOURCE_BARRIER ResourceBarrier{};

		ResourceBarrier = ResourceBarrier.Transition(m_D3D12DefaultBufferFilledSize.Get(), D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		D3D12GraphicsCommandList->ResourceBarrier(1, &ResourceBarrier);
		ResourceBarrier = ResourceBarrier.Transition(m_D3D12StreamOutputBuffer.Get(), D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		D3D12GraphicsCommandList->ResourceBarrier(1, &ResourceBarrier);
		ResourceBarrier = ResourceBarrier.Transition(m_D3D12DrawBuffer.Get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		D3D12GraphicsCommandList->ResourceBarrier(1, &ResourceBarrier);

		D3D12GraphicsCommandList->CopyResource(m_D3D12ReadBackBufferFilledSize.Get(), m_D3D12DefaultBufferFilledSize.Get());
		D3D12GraphicsCommandList->CopyResource(m_D3D12DrawBuffer.Get(), m_D3D12StreamOutputBuffer.Get());

		ResourceBarrier = ResourceBarrier.Transition(m_D3D12DefaultBufferFilledSize.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		D3D12GraphicsCommandList->ResourceBarrier(1, &ResourceBarrier);
		ResourceBarrier = ResourceBarrier.Transition(m_D3D12StreamOutputBuffer.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_STREAM_OUT, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		D3D12GraphicsCommandList->ResourceBarrier(1, &ResourceBarrier);
		ResourceBarrier = ResourceBarrier.Transition(m_D3D12DrawBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		D3D12GraphicsCommandList->ResourceBarrier(1, &ResourceBarrier);
	}
	else if (PSONum == 1)
	{
		D3D12GraphicsCommandList->SOSetTargets(0, 1, nullptr);

		Render(D3D12GraphicsCommandList);
	}
}

void CParticleMesh::PostRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT PSONum)
{
	if (PSONum == 0)
	{
		UINT64* ReadBackBufferFilledSize{};

		m_D3D12ReadBackBufferFilledSize->Map(0, nullptr, (void**)&ReadBackBufferFilledSize);
		m_VerticeCount = UINT(*ReadBackBufferFilledSize) / sizeof(CParticleVertex);
		m_D3D12ReadBackBufferFilledSize->Unmap(0, nullptr);
	}
}
