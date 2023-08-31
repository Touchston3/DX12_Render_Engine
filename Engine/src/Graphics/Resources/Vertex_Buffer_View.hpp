#pragma once
#include "Resource_View.hpp"

namespace VE::GFX::Resources
{
	class Vertex_Buffer_View : public Resource_View
	{
	public:
		Vertex_Buffer_View(Resource& resource, UINT size, UINT stride);

		const D3D12_VERTEX_BUFFER_VIEW& get_view_description() const { return m_view_description; }
	private:
		D3D12_VERTEX_BUFFER_VIEW m_view_description;

	};
}
