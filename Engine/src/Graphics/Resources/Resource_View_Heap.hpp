#pragma once
#include "Resource_View.hpp"
#include <wrl.h>
namespace VE::GFX::Resources
{
	class Resource_View_Heap 
	{

		template<typename Descriptor_Handle>
		class Iterator
		{
		public:
			Iterator(Descriptor_Handle handle, UINT stride);

			Iterator& operator++() { m_handle.ptr += m_stride; index++; return *this; }
			Iterator& operator++(int) { Iterator tmp(m_handle, m_stride); m_handle.ptr += m_stride; index++; return tmp; }

			Iterator& operator--() { m_handle.ptr -= m_stride; index--; return *this; }
			Iterator& operator--(int) { Iterator tmp(m_handle, m_stride); m_handle.ptr -= m_stride; index--; return tmp; }

			Iterator& operator[](int i) { m_handle.ptr += i * m_stride; return *this; }
			bool operator==(Iterator& iterator) const { return iterator.m_handle.ptr == m_handle.ptr; }
			bool operator!=(Iterator& iterator) const { return iterator.m_handle.ptr != m_handle.ptr; }

			Descriptor_Handle& operator*() { return m_handle; }
			UINT index = 0;
		private:
			Descriptor_Handle m_handle;
			UINT m_stride;
			

		};

	public:
		Resource_View_Heap(ID3D12Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT num_views);
		Resource_View_Heap(ID3D12Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT num_views, D3D12_DESCRIPTOR_HEAP_FLAGS flags);

		Iterator<D3D12_CPU_DESCRIPTOR_HANDLE> CPU_begin() const;
		Iterator<D3D12_GPU_DESCRIPTOR_HANDLE> GPU_begin() const;
		Iterator<D3D12_CPU_DESCRIPTOR_HANDLE> CPU_end() const;
		Iterator<D3D12_GPU_DESCRIPTOR_HANDLE> GPU_end() const;

		ID3D12DescriptorHeap* Get() const { return m_heap.Get(); }
		ID3D12DescriptorHeap* const* get_address() const { return m_heap.GetAddressOf(); }
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
		const UINT m_view_size;
		const UINT m_view_count;
	};
}
