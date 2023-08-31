#pragma once
#include "Resource_View.hpp"
#include "Resource_View_Heap.hpp"

namespace VE::GFX::Resources
{
	class Depth_Stencil_View : Resource_View
	{
	public:
		Depth_Stencil_View(ID3D12Device& device, Resource& resource, Resource_View_Heap& heap);

		const D3D12_DEPTH_STENCIL_VIEW_DESC& get_view_description() const { return m_view_description; }

	private:
		const D3D12_DEPTH_STENCIL_VIEW_DESC m_view_description;
	};
}
