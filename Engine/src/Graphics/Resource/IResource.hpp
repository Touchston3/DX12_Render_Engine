#pragma once

#include <d3d12.h>
#include "../../d3dx12.h"
enum class Heap_Type
{
	Default = 0,
	Upload,
};
class IResource
{
public:
	IResource() = default;
	virtual ~IResource() = default;

	D3D12_HEAP_TYPE get_heap_type(Heap_Type type);
	D3D12_RESOURCE_STATES get_resource_state(Heap_Type type);
};
