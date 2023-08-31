#pragma once
#include "Resource_View.hpp"
#include <dxgi1_6.h>

namespace VE::GFX::Resources
{
	class Index_Buffer_View : public Resource_View
	{
	public:
		Index_Buffer_View(Resource& resource, UINT size, DXGI_FORMAT format);

		const D3D12_INDEX_BUFFER_VIEW& get_view_description() const { return m_view_description; }
	private:
		D3D12_INDEX_BUFFER_VIEW m_view_description;
	};
}
