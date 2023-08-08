#pragma once
#include <windows.h>
#include <d3d12.h>
#include "../d3dx12.h"
#include <wrl.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include "Swap_Chain.hpp"
//This should honestly probably live in the window namespace. Once I abstract out the verbosity I will probs move it there. 
namespace void_renderer 
{
	class Window;
	struct Vertex
	{
		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT3 m_color;
	};
	class Renderer
	{
	public:
		Renderer(Window* window);
		~Renderer();
		void render();
	private:
		void setup_pipeline();
		void load_data();
		void block();
		void setup_descriptor_heaps();

		std::wstring get_shader_path(const std::wstring& name);
		void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);
	private:
		Window* m_window;
		UINT64 m_fence_value;
		UINT m_rtv_descriptor_size;
		CD3DX12_RECT m_scissor_rect;
		CD3DX12_VIEWPORT m_viewport;

		Microsoft::WRL::ComPtr<ID3D12Device2> m_device;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_rtvs[2];
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtv_descriptor_heap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsv_descriptor_heap;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_root_signature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipeline_state;

		D3D12_VERTEX_BUFFER_VIEW m_vbuffer_description;
		D3D12_INDEX_BUFFER_VIEW m_ibuffer_description;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_index_buffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_depth_buffer;
		const UINT m_index_count = 36;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_command_queue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_command_allocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		HANDLE m_fence_event;
		DirectX::XMMATRIX view_projection;
		DirectX::XMMATRIX m_world;
		Swap_Chain* m_swap_chain;
	};
}