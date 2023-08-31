#include "Depth_Stencil_View.hpp"

using namespace VE::GFX::Resources;
Depth_Stencil_View::Depth_Stencil_View(ID3D12Device& device, Resource& resource, Resource_View_Heap& heap) :
	Resource_View(resource),
	m_view_description(
		{
			.Format = DXGI_FORMAT_D32_FLOAT,
			.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
			.Flags = D3D12_DSV_FLAG_NONE,
		})
{
	device.CreateDepthStencilView
	(
		m_resource.Get(),
		&m_view_description,
		*heap.CPU_begin()
	);
}
