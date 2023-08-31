#include "Resource.hpp"
#include "../../Log/Error.hpp"
using namespace VE::GFX::Resources;

Resource::Resource(Swap_Chain& swap_chain, UINT index)
{
	swap_chain.Get()->GetBuffer(index, IID_PPV_ARGS(&m_resource));
}

Resource::Resource(ID3D12Device& device, const D3D12_HEAP_TYPE& heap_type, const CD3DX12_RESOURCE_DESC& resource_desc) :
	Resource(device, heap_type, default_heap_state(heap_type), resource_desc)
{
}

Resource::Resource(ID3D12Device& device, const D3D12_HEAP_TYPE& heap_type, const D3D12_RESOURCE_STATES& heap_states, const CD3DX12_RESOURCE_DESC& resource_desc) 
{  
	const CD3DX12_HEAP_PROPERTIES heap_props = CD3DX12_HEAP_PROPERTIES(heap_type);
	DEBUG_ASSERT(device.CreateCommittedResource
	(
		&heap_props,
		D3D12_HEAP_FLAG_NONE,
		&resource_desc,
		heap_states,
		nullptr,
		IID_PPV_ARGS(&m_resource)
	));
}

Resource::Resource(ID3D12Device& device, const D3D12_HEAP_TYPE& heap_type, const D3D12_RESOURCE_STATES& heap_states, const CD3DX12_RESOURCE_DESC& resource_desc, const D3D12_CLEAR_VALUE& clear_value)
{
	const CD3DX12_HEAP_PROPERTIES heap_props = CD3DX12_HEAP_PROPERTIES(heap_type);
	DEBUG_ASSERT(device.CreateCommittedResource
	(
		&heap_props,
		D3D12_HEAP_FLAG_NONE,
		&resource_desc,
		heap_states,
		&clear_value,
		IID_PPV_ARGS(&m_resource)
	));
}

D3D12_RESOURCE_STATES Resource::default_heap_state(const D3D12_HEAP_TYPE& heap_type)
{
	switch(heap_type)
	{
	case D3D12_HEAP_TYPE_UPLOAD:
		return D3D12_RESOURCE_STATE_GENERIC_READ;	
	case D3D12_HEAP_TYPE_DEFAULT:
		return D3D12_RESOURCE_STATE_COPY_DEST;
	}
}
