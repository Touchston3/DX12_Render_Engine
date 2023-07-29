#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <windows.h>
#include <DirectXMath.h>
#include <filesystem>
#include "../d3dx12.h"

using Microsoft::WRL::ComPtr;

class Graphics_Manager
{
public:
    struct Vertex
    {
        DirectX::XMFLOAT3 m_position;
        DirectX::XMFLOAT3 m_color;
    };
    
    static Graphics_Manager* get() { static Graphics_Manager instance; return &instance; }
    
    Graphics_Manager(const Graphics_Manager&) = delete;
    Graphics_Manager& operator=(const Graphics_Manager&) = delete;
    ~Graphics_Manager();

    void init(HWND window_handle);
    void update();
    void render();
    void close();
    void populate_command_list();    
    void get_hardware_adapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);
private:
    Graphics_Manager();
    std::wstring get_shader_path(const std::wstring& filename);

private:
    const std::wstring m_current_dir = std::filesystem::current_path();
    ComPtr<IDXGIFactory3> m_dxgi_factory;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12CommandQueue> m_command_queue;
    ComPtr<IDXGISwapChain4> m_swap_chain;
    ComPtr<ID3D12DescriptorHeap> m_rtv_heap;
    ComPtr<ID3D12CommandAllocator> m_command_allocator;
    ComPtr<ID3D12GraphicsCommandList> m_command_list;
    ComPtr<ID3D12Fence> m_fence;
    HANDLE m_fence_event;
    UINT64 m_fence_count = 0;
    UINT m_rtv_size;
    ComPtr<ID3D12Resource> m_rtvs[2];
    HWND* m_window_handle;
    ComPtr<ID3D12PipelineState> m_pipeline_state_object;
    ComPtr<ID3D12RootSignature> m_root_signature;
    
    ComPtr<ID3D12Resource> m_vertex_buffer;
    D3D12_VERTEX_BUFFER_VIEW m_vbuffer_view;
    
    ComPtr<ID3D12Resource> m_vertex_shader;
    ComPtr<ID3D12Resource> m_fragment_shader;
    const CD3DX12_RECT m_scissor_rect;
    const CD3DX12_VIEWPORT m_viewport;
    const UINT m_width = 1920;
    const UINT m_height = 1080;
    UINT m_buffer_count = 2;
};
