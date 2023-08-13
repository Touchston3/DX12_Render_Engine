#include "Swap_Chain.hpp"
#include "../Log/Error.hpp"
#include "../Window.h"
using namespace void_renderer;
Swap_Chain::Swap_Chain(Window& window, IDXGIFactory4& factory, ID3D12CommandQueue& command_queue) :
	m_buffer_count(2) //temp
{
	this->m_description =
	{
		.BufferDesc =
		{
			.Width = (UINT)window.get_width(),
			.Height = (UINT)window.get_height(),
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		},
		.SampleDesc =
		{
			.Count = 1,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = m_buffer_count,
		.OutputWindow = window.get_window_handler(),
		.Windowed = TRUE,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.Flags = 0,
	};
	Microsoft::WRL::ComPtr<IDXGISwapChain> tmp_swap_chain;
	DEBUG_ASSERT(factory.CreateSwapChain(&command_queue, &m_description, &tmp_swap_chain));
	DEBUG_ASSERT(tmp_swap_chain.As(&m_swap_chain));
}

