#include "Buffer_Resource.hpp"

Buffer_Resource::Buffer_Resource(Heap_Type heap_type, UINT buffer_size, ID3D12Device& device)
{
	
	const auto heap_props = CD3DX12_HEAP_PROPERTIES(get_heap_type(heap_type));
	const auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
	device.CreateCommittedResource(
		&heap_props,
		D3D12_HEAP_FLAG_NONE,
		&resource_desc,
		get_resource_state(heap_type),
		nullptr,
		IID_PPV_ARGS(&m_buffer)
	);
}

