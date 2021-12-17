#include "stdafx.h"
#include "DDSTextureLoader12.h"

bool IsCursorActive;
bool IsSolidTerrain{ true };
bool IsInside;
bool IsFreeCamera;

extern ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList,
	void* Data, const UINT64& Bytes, D3D12_HEAP_TYPE D3D12HeapType, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer,
	const UINT64& Width, UINT Height, UINT16 DepthOrArraySize, UINT16 MipLevels, D3D12_RESOURCE_DIMENSION D3D12ResourceDimension, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DxgiFormat)
{
	CD3DX12_HEAP_PROPERTIES D3D12HeapProperties{ D3D12HeapType };
	CD3DX12_RESOURCE_DESC D3D12ResourceDesc{ D3D12ResourceDimension,
		(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? (UINT64)D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT : 0,
		(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? Bytes : Width,
		(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? (UINT16)1 : Height,
		(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? (UINT16)1 : DepthOrArraySize,
		(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? (UINT16)1 : MipLevels,
		(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? DXGI_FORMAT_UNKNOWN : DxgiFormat,
		1,
		0,
		(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? D3D12_TEXTURE_LAYOUT_ROW_MAJOR : D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12ResourceFlags };

	ComPtr<ID3D12Resource> D3D12Buffer{};
	D3D12_RANGE D3D12ReadRange{};
	UINT8* DataBuffer{};

	switch (D3D12HeapType)
	{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			D3D12_RESOURCE_STATES D3D12ResourceInitialStates{ (D3D12UploadBuffer && Data) ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12ResourceStates };

			ThrowIfFailed(D3D12Device->CreateCommittedResource(&D3D12HeapProperties, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12ResourceInitialStates, nullptr,
				__uuidof(ID3D12Resource), (void**)D3D12Buffer.GetAddressOf()));

			if (D3D12UploadBuffer && Data)
			{
				// 업로드 버퍼를 생성한다.
				D3D12HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

				D3D12ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
				D3D12ResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
				D3D12ResourceDesc.Width = Bytes;
				D3D12ResourceDesc.Height = 1;
				D3D12ResourceDesc.DepthOrArraySize = 1;
				D3D12ResourceDesc.MipLevels = 1;
				D3D12ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
				D3D12ResourceDesc.SampleDesc.Count = 1;
				D3D12ResourceDesc.SampleDesc.Quality = 0;
				D3D12ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
				D3D12ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

				ThrowIfFailed(D3D12Device->CreateCommittedResource(&D3D12HeapProperties, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
					__uuidof(ID3D12Resource), (void**)D3D12UploadBuffer));

				// 업로드 버퍼를 매핑하여 데이터를 복사한다.
				ThrowIfFailed((*D3D12UploadBuffer)->Map(0, &D3D12ReadRange, (void**)&DataBuffer));
				memcpy(DataBuffer, Data, Bytes);
				(*D3D12UploadBuffer)->Unmap(0, nullptr);

				// 업로드 버퍼의 내용을 디폴트 버퍼에 복사한다.
				D3D12GraphicsCommandList->CopyResource(D3D12Buffer.Get(), *D3D12UploadBuffer);

				// 리소스 상태를 변경한다.
				CD3DX12_RESOURCE_BARRIER D3D12ResourceBarrier{};

				D3D12ResourceBarrier = D3D12ResourceBarrier.Transition(D3D12Buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12ResourceStates, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_BARRIER_FLAG_NONE);
				D3D12GraphicsCommandList->ResourceBarrier(1, &D3D12ResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12ResourceStates |= D3D12_RESOURCE_STATE_GENERIC_READ;

			ThrowIfFailed(D3D12Device->CreateCommittedResource(&D3D12HeapProperties, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12ResourceStates, nullptr,
				__uuidof(ID3D12Resource), (void**)D3D12Buffer.GetAddressOf()));

			if (Data)
			{
				ThrowIfFailed(D3D12Buffer->Map(0, &D3D12ReadRange, (void**)&DataBuffer));
				memcpy(DataBuffer, Data, Bytes);
				D3D12Buffer->Unmap(0, nullptr);
			}
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
		{
			D3D12ResourceStates |= D3D12_RESOURCE_STATE_COPY_DEST;

			ThrowIfFailed(D3D12Device->CreateCommittedResource(&D3D12HeapProperties, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12ResourceStates, nullptr,
				__uuidof(ID3D12Resource), (void**)D3D12Buffer.GetAddressOf()));

			if (Data)
			{
				ThrowIfFailed(D3D12Buffer->Map(0, &D3D12ReadRange, (void**)&DataBuffer));
				memcpy(DataBuffer, Data, Bytes);
				D3D12Buffer->Unmap(0, nullptr);
			}
			break;
		}
	}

	return D3D12Buffer;
}

extern ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList,
	void* Data, const UINT64& Bytes, D3D12_HEAP_TYPE D3D12HeapType, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer)
{
	return CreateTextureResource(D3D12Device, D3D12GraphicsCommandList, Data, Bytes, D3D12HeapType, D3D12ResourceStates, D3D12UploadBuffer,
		Bytes, 1, 1, 1, D3D12_RESOURCE_DIMENSION_BUFFER, D3D12_RESOURCE_FLAG_NONE, DXGI_FORMAT_UNKNOWN);
}

extern ComPtr<ID3D12Resource> CreateTextureResourceFromDDSFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList,
	const wchar_t* FileName, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer)
{
	ComPtr<ID3D12Resource> D3D12Texture{};
	std::vector<D3D12_SUBRESOURCE_DATA> Subresources{};
	std::unique_ptr<uint8_t[]> DDSData{};
	DDS_ALPHA_MODE DDSAlphaMode{ DDS_ALPHA_MODE_UNKNOWN };
	bool IsCubeMap{};

	ThrowIfFailed(DirectX::LoadDDSTextureFromFileEx(D3D12Device, FileName, 0, D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT, D3D12Texture.GetAddressOf(),
		DDSData, Subresources, &DDSAlphaMode, &IsCubeMap));

	UINT64 Bytes{ GetRequiredIntermediateSize(D3D12Texture.Get(), 0, (UINT)Subresources.size()) };
	CD3DX12_HEAP_PROPERTIES D3D12HeapPropertiesDesc{ D3D12_HEAP_TYPE_UPLOAD, 1, 1 };
	CD3DX12_RESOURCE_DESC D3D12ResourceDesc{ D3D12_RESOURCE_DIMENSION_BUFFER, 0, Bytes, 1, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };

	ThrowIfFailed(D3D12Device->CreateCommittedResource(&D3D12HeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)D3D12UploadBuffer));
	UpdateSubresources(D3D12GraphicsCommandList, D3D12Texture.Get(), *D3D12UploadBuffer, 0, 0, (UINT)Subresources.size(), Subresources.data());

	CD3DX12_RESOURCE_BARRIER D3D12ResourceBarrier{};

	D3D12ResourceBarrier = D3D12ResourceBarrier.Transition(D3D12Texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12ResourceStates, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_BARRIER_FLAG_NONE);

	D3D12GraphicsCommandList->ResourceBarrier(1, &D3D12ResourceBarrier);

	return D3D12Texture;
}
