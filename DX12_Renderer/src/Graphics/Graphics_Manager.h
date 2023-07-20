#pragma once

#include "../extern/d3dx12.h"
#include <wrl.h>
#include <dxgi1_6.h>
#include <windows.h>

using Microsoft::WRL::ComPtr;

class Graphics_Manager
{
public:
    static Graphics_Manager& get_instance() { static Graphics_Manager instance; return instance; }
    
    Graphics_Manager(const Graphics_Manager&) = delete;
    Graphics_Manager& operator=(const Graphics_Manager&) = delete;
    ~Graphics_Manager();

    void init(HWND window_handle);
    void update();
    void render();
    void close();
    
private:
    Graphics_Manager();

private:
    ComPtr<IDXGIFactory4> m_dxgi_factory;
    ComPtr<ID3D12Device2> m_device;
    ComPtr<ID3D12CommandQueue> m_command_queue;
    ComPtr<IDXGISwapChain3> m_swap_chain;
    ComPtr<ID3D12DescriptorHeap> m_rtv_heap;
    ComPtr<ID3D12CommandAllocator> m_command_allocator;
    ComPtr<ID3D12GraphicsCommandList> m_command_list;
    ComPtr<ID3D12Fence> m_fence;
    HANDLE m_fence_event;
    UINT64 m_fence_count = 0;
    UINT m_rtv_size;
    ComPtr<ID3D12Resource> m_rtvs[2];
    HWND* m_window_handle;
};
