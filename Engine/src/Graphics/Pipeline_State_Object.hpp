#pragma once
#include "Root_Signature.hpp"
#include <d3d12.h>

namespace VE::GFX
{
	class Input_Layout;
	class Vertex_Shader;
	class Pixel_Shader;

	class Pipeline_State_Object
	{
	public:

	private:
		Root_Signature m_root_signature;
		//Input_Layout m_input_layout;
		//Vertex_Shader m_vertex_shader;
		//Pixel_Shader m_pixel_shader;

	};
}
