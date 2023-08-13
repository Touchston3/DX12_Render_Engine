#include "IResource.hpp"

D3D12_HEAP_TYPE IResource::get_heap_type(Heap_Type type)
{
	switch(type)
	{
	case Heap_Type::Default:
		return D3D12_HEAP_TYPE_DEFAULT;
	case Heap_Type::Upload:
		return D3D12_HEAP_TYPE_UPLOAD;
	}
}

D3D12_RESOURCE_STATES IResource::get_resource_state(Heap_Type type)
{
	switch(type)
	{
	case Heap_Type::Default:
		return D3D12_RESOURCE_STATE_DEPTH_WRITE;
	case Heap_Type::Upload:
		return D3D12_RESOURCE_STATE_GENERIC_READ;
	}
}


