#pragma once
#include <windows.h>
#include <initguid.h>
#include <d3d12.h>
#include "../d3dx12.h"
#include <wrl.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include "Swap_Chain.hpp"
#include "Resources/Vertex_Buffer_View.hpp"
#include "Resources/Index_Buffer_View.hpp"
#include "Resources/Resource_View_Heap.hpp"
#include "Resources/Depth_Stencil_View.hpp"
#include "Resources/Buffer_Resource.hpp"
#include "Resources/Texture_Resource.hpp"
#include "Resources/Render_Target_View.hpp"
namespace VE
{
	class Window;
}
namespace VE::GFX 
{
	struct Vertex
	{
		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT2 m_uv;
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

		std::wstring get_shader_path(const std::wstring& name);
		void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);
	private:
		Window* m_window;
		UINT64 m_fence_value;
		CD3DX12_RECT m_scissor_rect;
		CD3DX12_VIEWPORT m_viewport;

		Microsoft::WRL::ComPtr<ID3D12Device2> m_device;


		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_root_signature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipeline_state;

		Resources::Buffer_Resource* m_index_buffer_resource;
		Resources::Buffer_Resource* m_vertex_buffer_resource;
		Resources::Texture_Resource* m_depth_stencil_resource;
		Resources::Texture_Resource* m_texture_resource;
		Resources::Resource* m_render_target_resources[2]; 

		Resources::Index_Buffer_View* m_ibv;
		Resources::Vertex_Buffer_View* m_vbv;
		Resources::Depth_Stencil_View* m_dsv;
		Resources::Render_Target_View* m_rtvs[2];

		Resources::Resource_View_Heap* m_dsv_heap;
		Resources::Resource_View_Heap* m_rtv_heap;
		Resources::Resource_View_Heap* m_srv_heap;

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

/*
	Planning application:
		-Resource and Resource View objects
		-- Note: CBVs, UAVs, SRVs, and samplers are the only views visible to shaders



*/