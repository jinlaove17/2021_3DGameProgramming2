#include "stdafx.h"
#include "Framework.h"

CFramework::CFramework()
{
	_tcscpy_s(m_Title, _T("D3D12 Sample ("));
}

void CFramework::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

void CFramework::OnCreate(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;
	m_Timer = make_unique<CTimer>();

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
	CreateDepthStencilView();
	BuildObjects();

	CSound::GetInstance()->Init();
	CSound::GetInstance()->Play(CSound::TITLE_BACKGROUND_SOUND, 0.6f);
}

void CFramework::OnDestroy()
{
	WaitForGpuComplete();
	ReleaseObjects();
	CloseHandle(m_FenceEvent);

	m_DXGISwapChain->SetFullscreenState(FALSE, nullptr);
}

void CFramework::BuildObjects()
{
	ThrowIfFailed(m_D3D12GraphicsCommandList->Reset(m_D3D12CommandAllocator.Get(), nullptr));

	unique_ptr<CTitleScene> TitleScene{ make_unique<CTitleScene>() };
	unique_ptr<CGameScene> GameScene{ make_unique<CGameScene>() };

	TitleScene->OnCreate(m_D3D12Device.Get(), m_D3D12GraphicsCommandList.Get());
	GameScene->OnCreate(m_D3D12Device.Get(), m_D3D12GraphicsCommandList.Get());

	ThrowIfFailed(m_D3D12GraphicsCommandList->Close());

	ID3D12CommandList* D3D12CommandLists[] = { m_D3D12GraphicsCommandList.Get() };

	m_D3D12CommandQueue->ExecuteCommandLists(_countof(D3D12CommandLists), D3D12CommandLists);

	WaitForGpuComplete();

	TitleScene->ReleaseUploadBuffers();
	GameScene->ReleaseObjects();

	CScene::m_Scenes.push(move(TitleScene));
	CScene::m_Scenes.push(move(GameScene));

	m_Timer->Reset();
}

void CFramework::ReleaseObjects()
{

}

void CFramework::CreateDirect3DDevice()
{
	UINT nDXGIFactoryFlags{};

#if defined(DEBUG)
	ComPtr<ID3D12Debug> D3D12DebugController{};

	ThrowIfFailed(D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)D3D12DebugController.GetAddressOf()));

	if (D3D12DebugController)
	{
		D3D12DebugController->EnableDebugLayer();
	}

	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	ThrowIfFailed(CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)m_DXGIFactory.GetAddressOf()));

	ComPtr<IDXGIAdapter1> DXGIAdapter{};

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_DXGIFactory->EnumAdapters1(i, DXGIAdapter.GetAddressOf()); i++)
	{
		DXGI_ADAPTER_DESC1 DXGIAdapterDesc{};
		HRESULT Result{};

		DXGIAdapter->GetDesc1(&DXGIAdapterDesc);

		if (DXGIAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		Result = D3D12CreateDevice(DXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)m_D3D12Device.GetAddressOf());

		if (SUCCEEDED(Result))
		{
			break;
		}
		else
		{
			ThrowIfFailed(Result);
		}
	}

	// 모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.
	if (!DXGIAdapter)
	{
		ThrowIfFailed(m_DXGIFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void**)DXGIAdapter.GetAddressOf()));
		ThrowIfFailed(D3D12CreateDevice(DXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)m_D3D12Device.GetAddressOf()));
	}

	// 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS D3D12MsaaQualityLevels{};

	D3D12MsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D12MsaaQualityLevels.SampleCount = 4;

	// Msaa4x 다중 샘플링
	D3D12MsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	D3D12MsaaQualityLevels.NumQualityLevels = 0;

	// 디바이스가 지원하는 다중 샘플의 품질 수준을 확인한다.
	ThrowIfFailed(m_D3D12Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &D3D12MsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)));

	// 다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화한다.
	m_Msaa4xQualityLevels = D3D12MsaaQualityLevels.NumQualityLevels;
	m_Msaa4xEnable = (m_Msaa4xQualityLevels > 1) ? true : false;

	// 펜스를 생성하고 펜스 값을 0으로 설정한다.
	ThrowIfFailed(m_D3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)m_D3D12Fence.GetAddressOf()));

	// 펜스와 동기화를 위한 이벤트 객체를 생성한다.(이벤트 객체의 초기값은 FALSE이다.)
	// 이벤트가 실행되면(Signal) 이벤트의 값을 자동적으로 FALSE가 되도록 생성한다.
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC D3D12CommandQueueDesc{};

	D3D12CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	D3D12CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	// 직접(Direct) 명령 큐를 생성한다.
	ThrowIfFailed(m_D3D12Device->CreateCommandQueue(&D3D12CommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)m_D3D12CommandQueue.GetAddressOf()));

	// 직접(Direct) 명령 할당자를 생성한다.
	ThrowIfFailed(m_D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)m_D3D12CommandAllocator.GetAddressOf()));

	// 직접(Direct) 명령 리스트를 생성한다.
	ThrowIfFailed(m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CommandAllocator.Get(), nullptr, __uuidof(ID3D12GraphicsCommandList), (void**)m_D3D12GraphicsCommandList.GetAddressOf()));

	// 명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다.
	ThrowIfFailed(m_D3D12GraphicsCommandList->Close());
}

void CFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC D3D12DescriptorHeapDesc{};

	D3D12DescriptorHeapDesc.NumDescriptors = m_SwapChainBufferCount;
	D3D12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	D3D12DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	D3D12DescriptorHeapDesc.NodeMask = 0;

	// 렌더 타겟 서술자 힙(서술자의 개수는 스왑체인 버퍼의 개수)을 생성한다.
	ThrowIfFailed(m_D3D12Device->CreateDescriptorHeap(&D3D12DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_D3D12RtvDescriptorHeap.GetAddressOf()));

	// 렌더 타겟 서술자 힙의 원소의 크기를 저장한다.
	m_RtvDescriptorIncrementSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// 깊이-스텐실 서술자 힙(서술자의 개수는 1)을 생성한다.
	D3D12DescriptorHeapDesc.NumDescriptors = 1;
	D3D12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	ThrowIfFailed(m_D3D12Device->CreateDescriptorHeap(&D3D12DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_D3D12DsvDescriptorHeap.GetAddressOf()));

	// 깊이-스텐실 서술자 힙의 원소의 크기를 저장한다.
	m_DsvDescriptorIncrementSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CFramework::CreateSwapChain()
{
#ifdef _WITH_CREATE_SWAPCHAIN_FOR_HWND
	DXGI_SWAP_CHAIN_DESC1 DXGISwapChainDesc{};

	DXGISwapChainDesc.Width = FRAME_BUFFER_WIDTH;
	DXGISwapChainDesc.Height = FRAME_BUFFER_HEIGHT;
	DXGISwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGISwapChainDesc.SampleDesc.Count = (m_Msaa4xEnable) ? 4 : 1;
	DXGISwapChainDesc.SampleDesc.Quality = (m_Msaa4xEnable) ? (m_Msaa4xQualityLevels - 1) : 0;
	DXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	DXGISwapChainDesc.BufferCount = m_SwapChainBufferCount;
	DXGISwapChainDesc.Scaling = DXGI_SCALING_NONE;
	DXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	DXGISwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	DXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc{};

	DXGISwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	DXGISwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	DXGISwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	DXGISwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	DXGISwapChainFullScreenDesc.Windowed = TRUE;

	ThrowIfFailed(m_DXGIFactory->CreateSwapChainForhWnd(m_D3D12CommandQueue.Get(), m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, nullptr, (IDXGISwapChain1**)m_DXGISwapChain.GetAddressOf()));
#else
	DXGI_SWAP_CHAIN_DESC DXGISwapChainDesc{};

	DXGISwapChainDesc.BufferDesc.Width = FRAME_BUFFER_WIDTH;
	DXGISwapChainDesc.BufferDesc.Height = FRAME_BUFFER_HEIGHT;
	DXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	DXGISwapChainDesc.BufferCount = m_SwapChainBufferCount;
	DXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	DXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	DXGISwapChainDesc.OutputWindow = m_hWnd;
	DXGISwapChainDesc.SampleDesc.Count = (m_Msaa4xEnable) ? 4 : 1;
	DXGISwapChainDesc.SampleDesc.Quality = (m_Msaa4xEnable) ? (m_Msaa4xQualityLevels - 1) : 0;
	DXGISwapChainDesc.Windowed = TRUE;
	DXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// 스왑체인을 생성한다.
	ThrowIfFailed(m_DXGIFactory->CreateSwapChain(m_D3D12CommandQueue.Get(), &DXGISwapChainDesc, (IDXGISwapChain**)m_DXGISwapChain.GetAddressOf()));
#endif
	// 스왑체인의 현재 후면버퍼 인덱스를 저장한다.
	m_SwapChainBufferIndex = m_DXGISwapChain->GetCurrentBackBufferIndex();

	// "Alt+Enter" 키의 동작을 비활성화한다.
	ThrowIfFailed(m_DXGIFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();
#endif
}

void CFramework::CreateRenderTargetViews()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12RtvCPUDescriptorHandle{ m_D3D12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	for (UINT i = 0; i < m_SwapChainBufferCount; ++i)
	{
		ThrowIfFailed(m_DXGISwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)m_D3D12RenderTargetBuffers[i].GetAddressOf()));
		m_D3D12Device->CreateRenderTargetView(m_D3D12RenderTargetBuffers[i].Get(), nullptr, D3D12RtvCPUDescriptorHandle);

		D3D12RtvCPUDescriptorHandle.ptr += m_RtvDescriptorIncrementSize;
	}
}

void CFramework::CreateDepthStencilView()
{
	CD3DX12_RESOURCE_DESC D3D12ResourceDesc{
		D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 1, 1, DXGI_FORMAT_D24_UNORM_S8_UINT,
		(m_Msaa4xEnable) ? (UINT)4 : (UINT)1, (m_Msaa4xEnable) ? (m_Msaa4xQualityLevels - 1) : 0, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL };
	CD3DX12_HEAP_PROPERTIES D3D312HeapProperties{ D3D12_HEAP_TYPE_DEFAULT, 1, 1 };
	CD3DX12_CLEAR_VALUE D3D12ClearValue{ DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0 };

	// 깊이-스텐실 버퍼를 생성한다.
	ThrowIfFailed(m_D3D12Device->CreateCommittedResource(&D3D312HeapProperties, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&D3D12ClearValue, __uuidof(ID3D12Resource), (void**)m_D3D12DepthStencilBuffer.GetAddressOf()));

	CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12DsvCPUDescriptorHandle{ m_D3D12DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	// 깊이-스텐실 버퍼 뷰를 생성한다.
	m_D3D12Device->CreateDepthStencilView(m_D3D12DepthStencilBuffer.Get(), nullptr, D3D12DsvCPUDescriptorHandle);
}

void CFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL FullScreenState{};

	m_DXGISwapChain->GetFullscreenState(&FullScreenState, nullptr);
	m_DXGISwapChain->SetFullscreenState(!FullScreenState, nullptr);

	DXGI_MODE_DESC DXGIModeDesc{};

	DXGIModeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGIModeDesc.Width = FRAME_BUFFER_WIDTH;
	DXGIModeDesc.Height = FRAME_BUFFER_HEIGHT;
	DXGIModeDesc.RefreshRate.Numerator = 60;
	DXGIModeDesc.RefreshRate.Denominator = 1;
	DXGIModeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	DXGIModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	ThrowIfFailed(m_DXGISwapChain->ResizeTarget(&DXGIModeDesc));

	for (UINT i = 0; i < m_SwapChainBufferCount; ++i)
	{
		if (m_D3D12RenderTargetBuffers[i])
		{
			m_D3D12RenderTargetBuffers[i]->Release();
		}
	}

	DXGI_SWAP_CHAIN_DESC DXGISwapChainDesc{};

	ThrowIfFailed(m_DXGISwapChain->GetDesc(&DXGISwapChainDesc));
	ThrowIfFailed(m_DXGISwapChain->ResizeBuffers(m_SwapChainBufferCount, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, DXGISwapChainDesc.BufferDesc.Format, DXGISwapChainDesc.Flags));
	
	m_SwapChainBufferIndex = m_DXGISwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

void CFramework::WaitForGpuComplete()
{
	const UINT64 nFenceValue = ++m_FenceValues[m_SwapChainBufferIndex];

	// GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다.
	ThrowIfFailed(m_D3D12CommandQueue->Signal(m_D3D12Fence.Get(), nFenceValue));

	// 펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다.
	if (m_D3D12Fence->GetCompletedValue() < nFenceValue)
	{
		ThrowIfFailed(m_D3D12Fence->SetEventOnCompletion(nFenceValue, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

void CFramework::MoveToNextFrame()
{
	// 프리젠트를 하면 현재 렌더 타겟(후면버퍼)의 내용이 전면버퍼로 옮겨지고 렌더 타겟 인덱스가 바뀔 것이다.
	m_SwapChainBufferIndex = m_DXGISwapChain->GetCurrentBackBufferIndex();

	WaitForGpuComplete();
}

void CFramework::SetViewport(int TopLeftX, int TopLeftY, UINT Width, UINT Height, float MinDepth, float MaxDepth)
{
	m_D3D12Viewport.TopLeftX = (float)TopLeftX;
	m_D3D12Viewport.TopLeftY = (float)TopLeftY;
	m_D3D12Viewport.Width = (float)Width;
	m_D3D12Viewport.Height = (float)Height;
	m_D3D12Viewport.MinDepth = MinDepth;
	m_D3D12Viewport.MaxDepth = MaxDepth;
}

void CFramework::SetScissorRect(LONG Left, LONG Top, LONG Right, LONG Bottom)
{
	m_D3D12ScissorRect.left = Left;
	m_D3D12ScissorRect.top = Top;
	m_D3D12ScissorRect.right = Right;
	m_D3D12ScissorRect.bottom = Bottom;
}

void CFramework::RSSetViewportsAndScissorRects()
{
	m_D3D12GraphicsCommandList->RSSetViewports(1, &m_D3D12Viewport);
	m_D3D12GraphicsCommandList->RSSetScissorRects(1, &m_D3D12ScissorRect);
}

void CFramework::ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		if (!CScene::m_Scenes.empty())
		{
			CScene::m_Scenes.front()->ProcessMouseMessage(hWnd, Message, wParam, lParam, m_D3D12Device.Get(), m_D3D12GraphicsCommandList.Get());
		}
		break;
	}
}

void CFramework::ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_F9:
			ChangeSwapChainState();
			break;
		default:
			if (!CScene::m_Scenes.empty())
			{
				CScene::m_Scenes.front()->ProcessKeyboardMessage(hWnd, Message, wParam, lParam, m_D3D12Device.Get(), m_D3D12GraphicsCommandList.Get());
			}
		}
	}
}

void CFramework::ProcessInput()
{
	if (!CScene::m_Scenes.empty())
	{
		CScene::m_Scenes.front()->ProcessInput(m_hWnd, m_Timer->GetElapsedTime());
	}
}

void CFramework::Animate()
{
	if (!CScene::m_Scenes.empty())
	{
		CScene::m_Scenes.front()->Animate(m_Timer->GetElapsedTime());
	}
}

void CFramework::Render()
{
	if (!CScene::m_Scenes.empty())
	{
		CScene::m_Scenes.front()->Render(m_D3D12GraphicsCommandList.Get());
	}
}

void CFramework::PopulateCommandList()
{
	ThrowIfFailed(m_D3D12CommandAllocator->Reset());
	ThrowIfFailed(m_D3D12GraphicsCommandList->Reset(m_D3D12CommandAllocator.Get(), nullptr));

	RSSetViewportsAndScissorRects();

	CD3DX12_RESOURCE_BARRIER D3D12ResourceBarrier{};

	D3D12ResourceBarrier = D3D12ResourceBarrier.Transition(m_D3D12RenderTargetBuffers[m_SwapChainBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_BARRIER_FLAG_NONE);
	m_D3D12GraphicsCommandList->ResourceBarrier(1, &D3D12ResourceBarrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12RtvCPUDescriptorHandle{ m_D3D12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12DsvCPUDescriptorHandle{ m_D3D12DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	D3D12RtvCPUDescriptorHandle = D3D12RtvCPUDescriptorHandle.Offset(m_SwapChainBufferIndex * m_RtvDescriptorIncrementSize);

	m_D3D12GraphicsCommandList->ClearRenderTargetView(D3D12RtvCPUDescriptorHandle, Colors::Aquamarine, 0, nullptr);
	m_D3D12GraphicsCommandList->ClearDepthStencilView(D3D12DsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	m_D3D12GraphicsCommandList->OMSetRenderTargets(1, &D3D12RtvCPUDescriptorHandle, TRUE, &D3D12DsvCPUDescriptorHandle);

	Render();

	D3D12ResourceBarrier = D3D12ResourceBarrier.Transition(m_D3D12RenderTargetBuffers[m_SwapChainBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_BARRIER_FLAG_NONE);
	m_D3D12GraphicsCommandList->ResourceBarrier(1, &D3D12ResourceBarrier);

	ThrowIfFailed(m_D3D12GraphicsCommandList->Close());

	ID3D12CommandList* D3D12CommandLists[] = { m_D3D12GraphicsCommandList.Get() };

	m_D3D12CommandQueue->ExecuteCommandLists(_countof(D3D12CommandLists), D3D12CommandLists);
}

void CFramework::FrameAdvance()
{
	if (!m_IsActive)
	{
		return;
	}

	m_Timer->Tick(0.0f);

	ProcessInput();
	Animate();
	PopulateCommandList();
	WaitForGpuComplete();
	ThrowIfFailed(m_DXGISwapChain->Present(1, 0));
	MoveToNextFrame();

	m_Timer->GetFrameRate(m_Title + 14, 50);
	::SetWindowText(m_hWnd, m_Title);
}
