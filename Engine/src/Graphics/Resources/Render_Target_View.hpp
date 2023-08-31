#pragma once
#include "Resource_View.hpp"

namespace VE::GFX::Resources
{
	class Render_Target_View : public Resource_View

	{
	public:
		Render_Target_View(ID3D12Device& device, Resource& resource, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	private:
		D3D12_RENDER_TARGET_VIEW_DESC m_view_description;
	};
}
