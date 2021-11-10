#pragma once
#include "Timer.h"
#include "Scene.h"

class CFramework
{
private:
	bool								m_IsActive{ true };

	TCHAR								m_Title[64]{};
	HINSTANCE							m_hInstance{};
	HWND								m_hWnd{};

	bool								m_Msaa4xEnable{};
	UINT								m_Msaa4xQualityLevels{};

	D3D12_VIEWPORT						m_D3D12Viewport{ 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	D3D12_RECT							m_D3D12ScissorRect{ 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };

	ComPtr<IDXGIFactory4>				m_DXGIFactory{};
	ComPtr<ID3D12Device>				m_D3D12Device{};

	ComPtr<IDXGISwapChain3>				m_DXGISwapChain{};
	static const UINT					m_SwapChainBufferCount{ 2 };
	UINT								m_SwapChainBufferIndex{};

	ComPtr<ID3D12Resource>				m_D3D12RenderTargetBuffers[m_SwapChainBufferCount]{};
	ComPtr<ID3D12DescriptorHeap>		m_D3D12RtvDescriptorHeap{};
	UINT								m_RtvDescriptorIncrementSize{};

	ComPtr<ID3D12Resource>				m_D3D12DepthStencilBuffer{};
	ComPtr<ID3D12DescriptorHeap>		m_D3D12DsvDescriptorHeap{};
	UINT								m_DsvDescriptorIncrementSize{};

	ComPtr<ID3D12CommandQueue>			m_D3D12CommandQueue{};
	ComPtr<ID3D12CommandAllocator>		m_D3D12CommandAllocator{};
	ComPtr<ID3D12GraphicsCommandList>	m_D3D12GraphicsCommandList{};

	ComPtr<ID3D12Fence>					m_D3D12Fence{};
	UINT64								m_FenceValues[m_SwapChainBufferCount]{};
	HANDLE								m_FenceEvent{};

	unique_ptr<CTimer>					m_Timer{};

public:
	CFramework();
	~CFramework() = default;

	void SetActive(bool IsActive);

	void OnCreate(HINSTANCE hInstance, HWND hWnd);
	void OnDestroy();

	void BuildObjects();
	void ReleaseObjects();

	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateSwapChain();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void ChangeSwapChainState();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void SetViewport(int TopLeftX, int TopLeftY, UINT Width, UINT Height, float MinDepth, float MaxDepth);
	void SetScissorRect(LONG Left, LONG Top, LONG Right, LONG Bottom);
	void RSSetViewportsAndScissorRects();

	void ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	void ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	void ProcessInput();

	void Animate();
	void Render();

	void PopulateCommandList();
	void FrameAdvance();
};
