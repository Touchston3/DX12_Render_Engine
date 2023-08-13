#pragma once
#include "IResource.hpp"
#include <wrl.h>
class Buffer_Resource : public IResource
{
public:
	Buffer_Resource(Heap_Type heap_type, UINT buffer_size, ID3D12Device& device);
	ID3D12Resource* Get() const { return m_buffer.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer;


};

