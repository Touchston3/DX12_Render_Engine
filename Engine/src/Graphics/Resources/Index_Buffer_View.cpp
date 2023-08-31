#include "Index_Buffer_View.hpp"

using namespace VE::GFX::Resources;

Index_Buffer_View::Index_Buffer_View(Resource& resource, UINT size, DXGI_FORMAT format) :
	Resource_View(resource),
	m_view_description(
		{
			.BufferLocation = m_resource.get_gpu_address(),
			.SizeInBytes = size,
			.Format = format, 
		})
{
	
}

