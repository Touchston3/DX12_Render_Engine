#include "Buffer_Resource.hpp"

using namespace VE::GFX::Resources;

Buffer_Resource::Buffer_Resource(ID3D12Device& device, D3D12_HEAP_TYPE heap_type, UINT buffer_size) : 
	Resource(device, heap_type, CD3DX12_RESOURCE_DESC::Buffer(buffer_size)),
	m_buffer_size(buffer_size)
{
}

