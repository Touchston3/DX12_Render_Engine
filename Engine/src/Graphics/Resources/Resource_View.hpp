#pragma once
#include "Resource.hpp"
#include <d3d12.h>
namespace VE::GFX::Resources
{
	class Resource_View
	{
	public:
		Resource_View(Resource& resource) : m_resource(resource) {}
		virtual ~Resource_View() = default;

		const Resource& get_resource() const { return m_resource; }
	protected:
		const Resource& m_resource; //Do we manage the lifetime of this? We shouldn't right?


		//Some kind of description
	};
}
/*
	Types of Views:
	//The view is only a struct/description. Accessing it should only return the description/address. 
		//Shader visible -- binding is controlled by application via root sig
		-Constant Buffer CBV
		-Unordered Access UAV
		-Shader Resource SRV
		-Sampler

		//Shader invisible -- binding is controlled by driver via command list recording
		-Render Target RTV
		-Depth Stencil DSV
		-Index Buffer IBV
		-Vertex Buffer VBV
		-Stream Output SOV

*/

