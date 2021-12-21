#pragma once

struct CB_CAMERA
{
	XMFLOAT4X4							m_ViewMatrix{};
	XMFLOAT4X4							m_ProjectionMatrix{};

	XMFLOAT3							m_Position{};
};

class CCamera
{
public:
	enum { FIRST_PERSON, THIRD_PERSON };

private:
	int									m_Mode{ THIRD_PERSON };
	float								m_TimeLag{ 0.15f };

	XMFLOAT4X4							m_ViewMatrix{ Matrix4x4::Identity() };
	XMFLOAT4X4							m_ProjectionMatrix{ Matrix4x4::Identity() };

	XMFLOAT3							m_Right{};
	XMFLOAT3							m_Up{};
	XMFLOAT3							m_Look{};
	XMFLOAT3							m_Position{};

	XMFLOAT3							m_Offset{ 0.0f, 10.0f, -15.0f };

	float								m_Pitch{};
	float								m_Roll{};
	float								m_Yaw{};
	
	ComPtr<ID3D12Resource>				m_D3D12CameraConstantBuffer{};
	CB_CAMERA*							m_MappedCamera{};

	BoundingFrustum						m_Frustum{};

public:
	CCamera() = default;
	virtual ~CCamera() = default;

	void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	void ReleaseShaderVariables();

	void GenerateViewMatrix(const XMFLOAT3& Position, const XMFLOAT3& Look);
	void RegenerateViewMatrix();
	const XMFLOAT4X4& GetViewMatrix() const;

	void GenerateOrthographicsProjectionMatrix(float Width, float Height, float NearZ, float FarZ);
	void GeneratePerspectiveProjectionMatrix(float FOVAngleY, float AspectRatio, float NearZ, float FarZ);
	const XMFLOAT4X4& GetProjectionMatrix() const;

	void GenerateFrustum();
	bool IsInFrustum(const BoundingOrientedBox& BoundingBox);

	void SetMode(int Mode);
	int GetMode() const;

	const XMFLOAT3& GetRight() const;
	const XMFLOAT3& GetUp() const;
	const XMFLOAT3& GetLook() const;

	void SetPosition(const XMFLOAT3& Position);
	const XMFLOAT3& GetPosition() const;

	void Move(const XMFLOAT3& Shift);
	void Rotate(float Pitch, float Yaw, float Roll);
	void Rotate(float Pitch, float Yaw, float Roll, const XMFLOAT3& Right, const XMFLOAT3& Up, const XMFLOAT3& Look, const XMFLOAT3& Position);
	void Rotate(const XMFLOAT3& Right, const XMFLOAT3& Up, const XMFLOAT3& Look, const XMFLOAT3& Position, float ElapsedTime);
};

// ============================================== CLightCamera ==============================================

class CLightCamera : public CCamera
{
private:
	D3D12_VIEWPORT						m_D3D12Viewport{ 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	D3D12_RECT							m_D3D12ScissorRect{ 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };

public:
	CLightCamera() = default;
	virtual ~CLightCamera() = default;

	void SetViewport(int TopLeftX, int TopLeftY, UINT Width, UINT Height, float MinDepth, float MaxDepth);
	void SetScissorRect(LONG Left, LONG Top, LONG Right, LONG Bottom);
	void RSSetViewportsAndScissorRects(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
};
