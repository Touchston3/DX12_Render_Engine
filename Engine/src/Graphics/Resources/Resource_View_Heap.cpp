#include "Resource_View_Heap.hpp"
#include "../../Log/Error.hpp"

using namespace VE::GFX::Resources;

Resource_View_Heap::Iterator<D3D12_CPU_DESCRIPTOR_HANDLE>::Iterator(D3D12_CPU_DESCRIPTOR_HANDLE handle, UINT stride) :
	m_handle(handle),
	m_stride(stride)
{
	
}
Resource_View_Heap::Iterator<D3D12_GPU_DESCRIPTOR_HANDLE>::Iterator(D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT stride) :
	m_handle(handle),
	m_stride(stride)
{
	
}




Resource_View_Heap::Resource_View_Heap(ID3D12Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT num_views) :
	Resource_View_Heap(device, type, num_views, D3D12_DESCRIPTOR_HEAP_FLAG_NONE)
{
}

Resource_View_Heap::Resource_View_Heap(ID3D12Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT num_views, D3D12_DESCRIPTOR_HEAP_FLAGS flags) :
	m_view_size(device.GetDescriptorHandleIncrementSize(type)),
	m_view_count(num_views)
{
	const D3D12_DESCRIPTOR_HEAP_DESC desc =
	{
		.Type = type,
		.NumDescriptors = m_view_count,
		.Flags = flags,
	};
	DEBUG_ASSERT(device.CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)));
}

Resource_View_Heap::Iterator<D3D12_CPU_DESCRIPTOR_HANDLE> Resource_View_Heap::CPU_begin() const
{
	return Iterator(m_heap->GetCPUDescriptorHandleForHeapStart(), m_view_size);
}

Resource_View_Heap::Iterator<D3D12_GPU_DESCRIPTOR_HANDLE> Resource_View_Heap::GPU_begin() const
{
	return Iterator(m_heap->GetGPUDescriptorHandleForHeapStart(), m_view_size);
}

Resource_View_Heap::Iterator<D3D12_CPU_DESCRIPTOR_HANDLE> Resource_View_Heap::CPU_end() const
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += m_view_size * m_view_count;
	return Iterator(handle, m_view_size);
}

Resource_View_Heap::Iterator<D3D12_GPU_DESCRIPTOR_HANDLE> Resource_View_Heap::GPU_end() const
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = m_heap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += m_view_size * m_view_count;
	return Iterator(handle, m_view_size);
}

