#pragma once
#include "Resource.hpp"

namespace VE::GFX::Resources
{
	class Texture_Resource : public Resource
	{
		//This whole thing needs to be redone from scratch. I should probably have a seperate texture object that has a description and manages the actual loading. What I have going rn is beyond stupid. 
	public:
		Texture_Resource(ID3D12Device& device, D3D12_HEAP_TYPE heap_type, UINT width, UINT height);
		Texture_Resource(ID3D12Device& device, D3D12_HEAP_TYPE heap_type, UINT width, UINT height, UINT mip_count, DXGI_FORMAT format);
		Texture_Resource(ID3D12Device& device, D3D12_HEAP_TYPE heap_type, UINT width, UINT height, D3D12_RESOURCE_FLAGS flags);
		Texture_Resource(ID3D12Device& device, UINT width, UINT height);

	};
}
