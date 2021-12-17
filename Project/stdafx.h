#pragma once

// 콘솔창 출력
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

// DirectX
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#define DEBUG_MODE
#define TERRAIN_TESSELLATION

// C 헤더 파일
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <wrl.h>
#include <shellapi.h>

// C++ 헤더 파일
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <ctime>

using namespace std;

// DirectX 헤더 파일
#include "D3DX12.h"
#include <D3Dcompiler.h>
#include <DXGI1_4.h>
#include <DXGIDebug.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;

// FMOD 사운드 헤더 파일
#include "fmod.hpp"

using namespace FMOD;

// CLIENT RECT
#define FRAME_BUFFER_WIDTH	 1600
#define FRAME_BUFFER_HEIGHT	  900

// TITLE SCENE OBJECT COUNT
#define	BUTTON_COUNT			2

// GAME SCENE OBJECT COUNT
#define ENEMY_COUNT				8
#define BOX_COUNT				3
#define WALL_COUNT				5
#define TREE_COUNT			  100
#define SMOKE_COUNT			   50

// WALL CENTER
#define WALL_CENTER			XMFLOAT3(-40.0f, 20.0f, 20.0f)
#define WALL_HALF_LENGTH	20.0f

#define RANDOM_COLOR		XMFLOAT4(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX)
#define EPSILON				1.0e-10f

//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE
//#define _WITH_PRESENT_PARAMETERS
//#define _WITH_SYNCH_SWAPCHAIN

extern bool IsCursorActive;
extern bool IsSolidTerrain;
extern bool IsInside;
extern bool IsFreeCamera;

extern ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList,
	void* Data, const UINT64& Bytes, D3D12_HEAP_TYPE D3D12HeapType, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer,
	const UINT64& Width, UINT Height, UINT16 DepthOrArraySize, UINT16 MipLevels, D3D12_RESOURCE_DIMENSION D3D12ResourceDimension, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DxgiFormat);
extern ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList,
	void* Data, const UINT64& Bytes, D3D12_HEAP_TYPE D3D12HeapType, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer);
extern ComPtr<ID3D12Resource> CreateTextureResourceFromDDSFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList,
	const wchar_t* FileName, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer);

inline float GetRandomNumber(float Min, float Max)
{
	return Min + (Max - Min) * ((float)rand() / (RAND_MAX));
}

inline void ThrowIfFailed(HRESULT Result)
{
    if (FAILED(Result))
    {
		TCHAR DebugMessage[64]{};

		_stprintf_s(DebugMessage, _countof(DebugMessage), _T("[ERROR] HRESULT of 0x%x\n"), Result);
		OutputDebugString(DebugMessage);
    }
}

inline bool IsZero(float fValue)
{ 
	return((fabsf(fValue) < EPSILON));
}

inline bool IsEqual(float fA, float fB)
{ 
	return(::IsZero(fA - fB));
}

inline float InverseSqrt(float fValue)
{ 
	return 1.0f / sqrtf(fValue);
}

inline void Swap(float* pfS, float* pfT)
{ 
	float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp;
}

namespace Vector3
{
	// 3차원 벡터가 영벡터인 가를 반환하는 함수이다.
	inline bool IsZero(const XMFLOAT3& Vector)
	{
		if (::IsZero(Vector.x) && ::IsZero(Vector.y) && ::IsZero(Vector.z))
		{
			return true;
		}

		return false;
	}

	inline XMFLOAT3 XMVectorToFloat3(const XMVECTOR& Vector)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, Vector);

		return Result;
	}

	inline XMFLOAT3 Add(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMLoadFloat3(&Vector1) + XMLoadFloat3(&Vector2));

		return Result;
	}

	inline XMFLOAT3 Add(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2, float Scalar)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMLoadFloat3(&Vector1) + Scalar * XMLoadFloat3(&Vector2));

		return Result;
	}

	inline XMFLOAT3 Subtract(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMLoadFloat3(&Vector1) - XMLoadFloat3(&Vector2));

		return Result;
	}

	inline XMFLOAT3 ScalarProduct(const XMFLOAT3& Vector, const float Scalar, bool Normalize)
	{
		XMFLOAT3 Result{};

		if (Normalize)
		{
			XMStoreFloat3(&Result, XMVector3Normalize(Scalar * XMLoadFloat3(&Vector)));
		}
		else
		{
			XMStoreFloat3(&Result, XMLoadFloat3(&Vector) * Scalar);
		}

		return Result;
	}

	inline float DotProduct(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMVector3Dot(XMLoadFloat3(&Vector1), XMLoadFloat3(&Vector2)));

		return Result.x;
	}

	inline XMFLOAT3 CrossProduct(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2, bool Normalize)
	{
		XMFLOAT3 Result{};

		if (Normalize)
		{
			XMStoreFloat3(&Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&Vector1), XMLoadFloat3(&Vector2))));
		}
		else
		{
			XMStoreFloat3(&Result, XMVector3Cross(XMLoadFloat3(&Vector1), XMLoadFloat3(&Vector2)));
		}

		return Result;
	}

	inline XMFLOAT3 Normalize(const XMFLOAT3& Vector)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMVector3Normalize(XMLoadFloat3(&Vector)));

		return Result;
	}

	inline float Length(const XMFLOAT3& Vector)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMVector3Length(XMLoadFloat3(&Vector)));

		return Result.x;
	}

	inline float Angle(const XMVECTOR& Vector1, const XMVECTOR& Vector2)
	{
		XMVECTOR Result{ XMVector3AngleBetweenNormals(Vector1, Vector2) };

		return XMConvertToDegrees(XMVectorGetX(Result));
	}

	inline float Angle(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2)
	{
		return Angle(XMLoadFloat3(&Vector1), XMLoadFloat3(&Vector2));
	}

	inline XMFLOAT3 TransformNormal(const XMFLOAT3& Vector, const XMMATRIX& Matrix)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMVector3TransformNormal(XMLoadFloat3(&Vector), Matrix));

		return Result;
	}

	inline XMFLOAT3 TransformNormal(const XMFLOAT3& Vector, const XMFLOAT4X4& Matrix)
	{
		return TransformNormal(Vector, XMLoadFloat4x4(&Matrix));
	}

	inline XMFLOAT3 TransformCoord(const XMFLOAT3& Vector, const XMMATRIX& Matrix)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMVector3TransformCoord(XMLoadFloat3(&Vector), Matrix));

		return Result;
	}

	inline XMFLOAT3 TransformCoord(const XMFLOAT3& Vector, const XMFLOAT4X4& Matrix)
	{
		return TransformCoord(Vector, XMLoadFloat4x4(&Matrix));
	}
}

namespace Vector4
{
	inline XMFLOAT4 Add(const XMFLOAT4& Vector1, const XMFLOAT4& Vector2)
	{
		XMFLOAT4 Result{};

		XMStoreFloat4(&Result, XMLoadFloat4(&Vector1) + XMLoadFloat4(&Vector2));

		return Result;
	}

	inline XMFLOAT4 Multiply(const XMFLOAT4& Vector1, const XMFLOAT4& Vector2)
	{
		XMFLOAT4 Result{};

		XMStoreFloat4(&Result, XMLoadFloat4(&Vector1) * XMLoadFloat4(&Vector2));

		return Result;
	}

	inline XMFLOAT4 Multiply(float Scalar, const XMFLOAT4& Vector)
	{
		XMFLOAT4 Result{};

		XMStoreFloat4(&Result, Scalar * XMLoadFloat4(&Vector));

		return Result;
	}
}

// 행렬의 연산
namespace Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixIdentity());

		return Result;
	}

	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& Matrix1, const XMFLOAT4X4& Matrix2)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMLoadFloat4x4(&Matrix1) * XMLoadFloat4x4(&Matrix2));

		return Result;
	}

	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& Matrix1, const XMMATRIX& Matrix2)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMLoadFloat4x4(&Matrix1) * Matrix2);

		return Result;
	}

	inline XMFLOAT4X4 Multiply(const XMMATRIX& Matrix1, const XMFLOAT4X4& Matrix2)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, Matrix1 * XMLoadFloat4x4(&Matrix2));

		return Result;
	}

	inline XMFLOAT4X4 RotationYawPitchRoll(float Pitch, float Yaw, float Roll)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixRotationRollPitchYaw(XMConvertToRadians(Pitch), XMConvertToRadians(Yaw), XMConvertToRadians(Roll)));

		return Result;
	}

	inline XMFLOAT4X4 RotationAxis(const XMFLOAT3& Axis, float Angle)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixRotationAxis(XMLoadFloat3(&Axis), XMConvertToRadians(Angle)));

		return Result;
	}

	inline XMFLOAT4X4 Inverse(const XMFLOAT4X4& Matrix)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixInverse(nullptr, XMLoadFloat4x4(&Matrix)));

		return Result;
	}

	inline XMFLOAT4X4 Transpose(const XMFLOAT4X4& Matrix)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixTranspose(XMLoadFloat4x4(&Matrix)));

		return Result;
	}

	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));

		return Result;
	}

	inline XMFLOAT4X4 LookAtLH(const XMFLOAT3& Position, const XMFLOAT3& FocusPosition, const XMFLOAT3& UpDirection)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixLookAtLH(XMLoadFloat3(&Position), XMLoadFloat3(&FocusPosition), XMLoadFloat3(&UpDirection)));

		return Result;
	}
}
