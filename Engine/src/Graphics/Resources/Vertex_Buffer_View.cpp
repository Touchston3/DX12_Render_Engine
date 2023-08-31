#include "Vertex_Buffer_View.hpp"

using namespace VE::GFX::Resources;

Vertex_Buffer_View::Vertex_Buffer_View(Resource& resource, UINT size, UINT stride) :
	Resource_View(resource),
	m_view_description(
		{
			.BufferLocation = m_resource.get_gpu_address(),
			.SizeInBytes = size,
			.StrideInBytes = stride,
		})
{
}
