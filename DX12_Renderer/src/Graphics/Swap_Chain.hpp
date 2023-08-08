#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace void_renderer
{
	class Window;
	class Swap_Chain
	{
	public:
		Swap_Chain(Window& window, IDXGIFactory4& factory, ID3D12CommandQueue& command_queue); //None of these should be r values
		inline IDXGISwapChain4* Get() const { return m_swap_chain.Get(); }
		inline USHORT get_buffer_count() const { return m_buffer_count; }
	private:
		USHORT m_buffer_count;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swap_chain;
		DXGI_SWAP_CHAIN_DESC m_description;
	};
}
