#pragma once
#include <d3d12.h>
#include "../../d3dx12.h"
#include <wrl.h>
#include "../Swap_Chain.hpp"
namespace VE::GFX::Resources
{
	class Resource 
	{
	public:
		Resource(Swap_Chain& swap_chain, UINT index);
		Resource(ID3D12Device& device, const D3D12_HEAP_TYPE& heap_type, const CD3DX12_RESOURCE_DESC& resource_desc);
		Resource(ID3D12Device& device, const D3D12_HEAP_TYPE& heap_type, const D3D12_RESOURCE_STATES& heap_states, const CD3DX12_RESOURCE_DESC& resource_desc);
		Resource(ID3D12Device& device, const D3D12_HEAP_TYPE& heap_type, const D3D12_RESOURCE_STATES& heap_states, const CD3DX12_RESOURCE_DESC& resource_desc, const D3D12_CLEAR_VALUE& clear_value);
		virtual ~Resource() = default;
		inline D3D12_GPU_VIRTUAL_ADDRESS get_gpu_address() const { return m_resource->GetGPUVirtualAddress(); }
		inline ID3D12Resource* Get() const { return m_resource.Get(); }
	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
	private:
		D3D12_RESOURCE_STATES default_heap_state(const D3D12_HEAP_TYPE& heap_type);
	};
}

/*
	Types of Resources:
		-Textures
		-Buffers

	Types of Views:
	//The view is only a struct/description. Accessing it should only return the description/address. 
		//Shader visible -- binding is controlled by application via root sig
		-Constant Buffer CBV
		-Unordered Access UAV
		-Shader Resource SRV
		-Sampler

		//Shader invisible -- binding is controlled by driver via command list recording
		-Render Target RTV
		-Depth Stencil DSV
		-Index Buffer IBV
		-Vertex Buffer VBV
		-Stream Output SOV

	//Suballocation is a thing. I don't really get it yet
	

*/
