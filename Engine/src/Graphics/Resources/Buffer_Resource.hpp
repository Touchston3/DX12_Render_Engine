#pragma once
#include "Resource.hpp"

namespace VE::GFX::Resources
{
	class Buffer_Resource : public Resource
	{
	public:
		Buffer_Resource(ID3D12Device& device, D3D12_HEAP_TYPE heap_type, UINT buffer_size);

	private:
		UINT m_buffer_size;
	};
}
