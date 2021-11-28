#include "stdafx.h"
#include "Camera.h"

void CCamera::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	UINT Bytes{ (sizeof(CB_CAMERA) + 255) & ~255 };

	m_D3D12CameraConstantBuffer = CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	ThrowIfFailed(m_D3D12CameraConstantBuffer->Map(0, nullptr, (void**)&m_MappedCamera));
}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	XMFLOAT4X4 ViewMatrix{};
	XMFLOAT4X4 ProjectionMatrix{};

	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ViewMatrix)));
	XMStoreFloat4x4(&ProjectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjectionMatrix)));

	m_MappedCamera->m_ViewMatrix = ViewMatrix;
	m_MappedCamera->m_ProjectionMatrix = ProjectionMatrix;
	m_MappedCamera->m_Position = m_Position;

	D3D12GraphicsCommandList->SetGraphicsRootConstantBufferView(1, m_D3D12CameraConstantBuffer->GetGPUVirtualAddress());
}

void CCamera::ReleaseShaderVariables()
{
	m_D3D12CameraConstantBuffer->Unmap(0, nullptr);
}

void CCamera::GenerateViewMatrix(const XMFLOAT3& Position, const XMFLOAT3& LookDirection)
{
	m_Position = Position;
	m_LookDirection = LookDirection;

	RegenerateViewMatrix();
}

void CCamera::RegenerateViewMatrix()
{
	const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	m_ViewMatrix = Matrix4x4::LookAtLH(m_Position, Vector3::Add(m_Position, m_LookDirection), WorldUp);
	m_Look = Vector3::Normalize(m_LookDirection);
	m_Right = Vector3::CrossProduct(WorldUp, m_Look, true);
	m_Up = Vector3::CrossProduct(m_Look, m_Right, false);

	GenerateFrustum();
}

const XMFLOAT4X4& CCamera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

void CCamera::GenerateProjectionMatrix(float FOVAngleY, float AspectRatio, float NearZ, float FarZ)
{
	m_ProjectionMatrix = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(FOVAngleY), AspectRatio, NearZ, FarZ);
}

const XMFLOAT4X4& CCamera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

void CCamera::GenerateFrustum()
{
	// 원근 투영 변환 행렬에서 절두체를 생성한다(절두체는 카메라 좌표계로 표현된다).
	m_Frustum.CreateFromMatrix(m_Frustum, XMLoadFloat4x4(&m_ProjectionMatrix));

	// 카메라 변환 행렬의 역행렬을 구한다.
	XMMATRIX InverseViewMatrix{ XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ViewMatrix)) };

	// 절두체를 카메라 변환 행렬의 역행렬로 변환한다(이제 절두체는 월드 좌표계로 표현된다).
	m_Frustum.Transform(m_Frustum, InverseViewMatrix);
}

bool CCamera::IsInFrustum(const BoundingOrientedBox& BoundingBox)
{
	return m_Frustum.Intersects(BoundingBox);
}

void CCamera::SetMode(int Mode)
{
	m_Mode = Mode;
}

int CCamera::GetMode() const
{
	return m_Mode;
}

const XMFLOAT3& CCamera::GetRight() const
{
	return m_Right;
}

const XMFLOAT3& CCamera::GetUp() const
{
	return m_Up;
}

const XMFLOAT3& CCamera::GetLook() const
{
	return m_Look;
}

void CCamera::SetPosition(const XMFLOAT3& Position)
{
	m_Position = Position;
}

const XMFLOAT3& CCamera::GetPosition() const
{
	return m_Position;
}

void CCamera::Move(const XMFLOAT3& Shift)
{
	m_Position.x += Shift.x;
	m_Position.y += Shift.y;
	m_Position.z += Shift.z;

	RegenerateViewMatrix();
}

void CCamera::Rotate(float Pitch, float Yaw, float Roll)
{
	if (Pitch)
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(m_Right, Pitch) };

		m_LookDirection = Vector3::TransformNormal(m_LookDirection, RotationMatrix);
	}

	if (Yaw)
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(m_Up, Yaw) };

		m_LookDirection = Vector3::TransformNormal(m_LookDirection, RotationMatrix);
	}

	if (Roll)
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(m_Look, Roll) };

		m_LookDirection = Vector3::TransformNormal(m_LookDirection, RotationMatrix);
	}

	RegenerateViewMatrix();
}

void CCamera::Rotate(float Pitch, float Yaw, float Roll, const XMFLOAT3& Right, const XMFLOAT3& Up, const XMFLOAT3& Look, const XMFLOAT3& Position)
{
	if (Pitch)
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(Right, Pitch) };

		m_LookDirection = Vector3::TransformNormal(m_LookDirection, RotationMatrix);
	}

	if (Yaw)
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(Up, Yaw) };

		m_LookDirection = Vector3::TransformNormal(m_LookDirection, RotationMatrix);
	}

	if (Roll)
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(Look, Roll) };

		m_LookDirection = Vector3::TransformNormal(m_LookDirection, RotationMatrix);
	}

	m_Position = Position;

	RegenerateViewMatrix();
}

void CCamera::Rotate(const XMFLOAT3& Right, const XMFLOAT3& Up, const XMFLOAT3& Look, const XMFLOAT3& Position, float ElapsedTime)
{
	XMFLOAT4X4 RotationMatrix{ Matrix4x4::Identity() };

	RotationMatrix._11 = Right.x; RotationMatrix._12 = Right.y; RotationMatrix._13 = Right.z;
	RotationMatrix._21 = Up.x; RotationMatrix._22 = Up.y; RotationMatrix._23 = Up.z;
	RotationMatrix._31 = Look.x; RotationMatrix._32 = Look.y; RotationMatrix._33 = Look.z;

	XMFLOAT3 Offset{ Vector3::TransformCoord(m_Offset, RotationMatrix) };
	XMFLOAT3 RotatedPosition{ Vector3::Add(Position, Offset) };
	XMFLOAT3 Direction{ Vector3::Subtract(RotatedPosition, m_Position) };
	float Distance{ Vector3::Length(Direction) };
	float TimeLagScale{ (m_TimeLag) ? ElapsedTime * (1.0f / m_TimeLag) : 1.0f };
	float Shift{ Distance * TimeLagScale };

	if (Shift > Distance || Distance < 0.01f)
	{
		Shift = Distance;
	}

	if (Shift > 0.0f)
	{
		Direction = Vector3::Normalize(Direction);
		m_Position = Vector3::Add(m_Position, Direction, Shift);
		m_LookDirection = Vector3::Subtract(Position, m_Position);
	}

	RegenerateViewMatrix();
}
