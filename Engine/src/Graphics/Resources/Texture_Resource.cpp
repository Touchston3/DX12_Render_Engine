#include "Texture_Resource.hpp"

using namespace VE::GFX::Resources;


Texture_Resource::Texture_Resource(ID3D12Device& device, D3D12_HEAP_TYPE heap_type, UINT width, UINT height, D3D12_RESOURCE_FLAGS flags) :
	Resource(device, heap_type, CD3DX12_RESOURCE_DESC::Tex2D
	(
		DXGI_FORMAT_D32_FLOAT,
		width,
		height,
		1, 0, 1, 0,
		flags
	))
{
}

Texture_Resource::Texture_Resource(ID3D12Device& device, UINT width, UINT height) :
	Resource
	(
		device, 
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		CD3DX12_RESOURCE_DESC::Tex2D
		(
			DXGI_FORMAT_D32_FLOAT,
			width, height,
			1, 0, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		),
{
			.Format = DXGI_FORMAT_D32_FLOAT,
			.DepthStencil = {1.0f, 0 } ,
		}
	)
{
}

Texture_Resource::Texture_Resource(ID3D12Device& device, D3D12_HEAP_TYPE heap_type, UINT width, UINT height) :
	Texture_Resource(device, heap_type, width, height, D3D12_RESOURCE_FLAG_NONE)
{
}

Texture_Resource::Texture_Resource(ID3D12Device& device, D3D12_HEAP_TYPE heap_type, UINT width, UINT height, UINT mip_count, DXGI_FORMAT format) :
	Resource
	(
		device, 
		heap_type,
		CD3DX12_RESOURCE_DESC::Tex2D
		(
			format,
			width, height,
			1, mip_count, 1, 0,
			D3D12_RESOURCE_FLAG_NONE
		)
	)
{
}
