#include "Render_Target_View.hpp"

using namespace VE::GFX::Resources;
Render_Target_View::Render_Target_View(ID3D12Device& device, Resource& resource, D3D12_CPU_DESCRIPTOR_HANDLE handle) :
	Resource_View(resource),
	m_view_description({})
{
	device.CreateRenderTargetView(m_resource.Get(), nullptr, handle);
}
