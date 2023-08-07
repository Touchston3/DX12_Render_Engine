#include "Renderer.h"
#include "../Log/Error.hpp"
#include "../Window.h"
#include <d3dcompiler.h>

using namespace void_renderer;
using namespace Microsoft::WRL;
using namespace DirectX;

Renderer::Renderer(Window* window) :
	m_window(window),
	m_fence_value(0),
	m_command_allocator(),
	m_command_list(),
	m_rtv_descriptor_size(0),
	m_scissor_rect(0, 0, (LONG)window->get_width(), (LONG)window->get_height()),
	m_viewport(0.0f, 0.0f, float(m_window->get_width()), float(m_window->get_height()))
{

	const UINT buffer_count = 2;
	//Enable Debug
	{
		ComPtr<ID3D12Debug> debug_controller;
		DEBUG_ASSERT(
			D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)));
		debug_controller->EnableDebugLayer();
	}

	ComPtr<IDXGIFactory4> dxgi_factory;
	DEBUG_ASSERT(
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgi_factory)));

	ComPtr<IDXGIAdapter1> adapter;
	GetHardwareAdapter(dxgi_factory.Get(), &adapter, true);

	DEBUG_ASSERT(
		D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)));
	{
		D3D12_COMMAND_QUEUE_DESC description =
		{
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		};
		DEBUG_ASSERT(
			m_device->CreateCommandQueue(&description, IID_PPV_ARGS(&m_command_queue)));
	}

	{
		ComPtr<IDXGISwapChain> tmp_swap_chain;
		DXGI_SWAP_CHAIN_DESC description =
		{
			.BufferDesc =
			{
				.Width = (UINT)m_window->get_width(),
				.Height = (UINT)m_window->get_height(),
				.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
			},
			.SampleDesc =
			{
				.Count = 1,
			},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = buffer_count,
			.OutputWindow = m_window->get_window_handler(),
			.Windowed = TRUE,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags = 0,
		};
		DEBUG_ASSERT(
			dxgi_factory->CreateSwapChain(m_command_queue.Get(), &description, &tmp_swap_chain));
		DEBUG_ASSERT(
			tmp_swap_chain.As(&m_swap_chain));
	}
	{
		D3D12_DESCRIPTOR_HEAP_DESC description =
		{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = buffer_count,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		};

		DEBUG_ASSERT(
			m_device->CreateDescriptorHeap(&description, IID_PPV_ARGS(&m_rtv_descriptor_heap)));
		m_rtv_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < buffer_count; i++)
		{
			DEBUG_ASSERT(
				m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&m_rtvs[i])));
			m_device->CreateRenderTargetView(m_rtvs[i].Get(), nullptr, handle);
			handle.Offset(1, m_rtv_descriptor_size);
		}
	}
	setup_pipeline();
	load_data();

	// Create the command list.

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.

	// Create the vertex buffer.


	DEBUG_ASSERT(
		m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_command_allocator)));
	DEBUG_ASSERT(
		m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_command_allocator.Get(), m_pipeline_state.Get(), IID_PPV_ARGS(&m_command_list)));
	DEBUG_ASSERT(
		m_command_list->Close());

	DEBUG_ASSERT(
		m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);


	//Upload buffers and stuff
	block();
}

void Renderer::setup_pipeline()
{
	{
		CD3DX12_ROOT_SIGNATURE_DESC root_sig_description;
		constexpr UINT root_param_count = 1;
		CD3DX12_ROOT_PARAMETER root_parameters[root_param_count] = {};
		root_parameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		root_sig_description.Init(root_param_count, root_parameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;

		DEBUG_ASSERT(
			D3D12SerializeRootSignature(&root_sig_description, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
		DEBUG_ASSERT(
			m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_root_signature)));
	}

	ComPtr<ID3DBlob> vertex_shader;
	ComPtr<ID3DBlob> pixel_shader;
	{
		DEBUG_ASSERT(D3DCompileFromFile(get_shader_path(L"vert.hlsl").c_str(), nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertex_shader, nullptr));
		DEBUG_ASSERT(D3DCompileFromFile(get_shader_path(L"pixel.hlsl").c_str(), nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixel_shader, nullptr));
	}
	D3D12_INPUT_ELEMENT_DESC input_layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	{
		struct Pipeline_State_Stream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE m_root_sig;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT m_input_layout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY m_primitive_type;
			CD3DX12_PIPELINE_STATE_STREAM_VS m_vs;
			CD3DX12_PIPELINE_STATE_STREAM_PS m_ps;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS m_rtv_formats;
		};

		Pipeline_State_Stream pipeline_state_stream =
		{
			.m_root_sig = m_root_signature.Get(),
			.m_input_layout = CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT({ input_layout, std::size(input_layout) }),
			.m_primitive_type = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			.m_vs = CD3DX12_SHADER_BYTECODE(vertex_shader.Get()),
			.m_ps = CD3DX12_SHADER_BYTECODE(pixel_shader.Get()),
			.m_rtv_formats = CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS({{ DXGI_FORMAT_R8G8B8A8_UNORM }, 1 })
		};

		const D3D12_PIPELINE_STATE_STREAM_DESC pipeline_state_stream_desc = { sizeof(Pipeline_State_Stream), &pipeline_state_stream };
		m_device->CreatePipelineState(&pipeline_state_stream_desc, IID_PPV_ARGS(&m_pipeline_state));
	}
}

void Renderer::load_data()
{
	const XMVECTOR eye_pos = XMVectorSet(0, 5, -5, 1);
	const XMVECTOR focus_pos = XMVectorSet(0, 0, 0, 1);
	const XMVECTOR up_vec = XMVectorSet(0, 1, 0, 1);
	const XMMATRIX view = XMMatrixLookAtLH(eye_pos, focus_pos, up_vec);

	const float aspect_ratio = (float)m_window->get_width() / (float)m_window->get_height();
	const XMMATRIX projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(65.0f), aspect_ratio, 0.1f, 100.0f);

	m_world = XMMatrixRotationZ(90.0f) * view * projection;

	// Define the geometry for a triangle.
	Vertex triangleVertices[] =
	{
		{ { -1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f} },
		{ { -1.0f, 1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { 1.0f, 1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { 1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { -1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { -1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { 1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
	};
	const UINT vertexBufferSize = sizeof(triangleVertices);
	
	const unsigned short index_buffer_data[] =
	{
			0, 1, 2,
			0, 2, 3,
			4, 6, 5,
			4, 7, 6,
			5, 5, 1,
			4, 1, 0,
			3, 2, 6,
			3, 6, 7,
			1, 5, 6, 
			1, 6, 2,
			4, 0, 3, 
			4, 3, 7,
		};
	const UINT index_buffer_size = sizeof(index_buffer_data);

	// Note: using upload heaps to transfer static data like vert buffers is not 
	// recommended. Every time the GPU needs it, the upload heap will be marshalled 
	// over. Please read up on Default Heap usage. An upload heap is used here for 
	// code simplicity and because there are very few verts to actually transfer.
	{
		const auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		m_device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&resource_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_vertex_buffer)
		);
	}
	{
		const auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(index_buffer_size);
		m_device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&resource_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_index_buffer)
		);
	}


	// Copy the triangle data to the vertex buffer.
	{
		Vertex* gpu_vbuffer_handle;
		CD3DX12_RANGE read_range(0, 0);
		m_vertex_buffer->Map(0, &read_range, reinterpret_cast<void**>(&gpu_vbuffer_handle));
		memcpy(gpu_vbuffer_handle, triangleVertices, vertexBufferSize);
		m_vertex_buffer->Unmap(0, nullptr);
	}
	{
		unsigned int* gpu_ibuffer_handle;
		CD3DX12_RANGE read_range(0, 0);
		m_index_buffer->Map(0, &read_range, (void**)(&gpu_ibuffer_handle));
		memcpy(gpu_ibuffer_handle, index_buffer_data, index_buffer_size);
		m_index_buffer->Unmap(0, nullptr);
	}
	m_vbuffer_description =
	{
		.BufferLocation = m_vertex_buffer->GetGPUVirtualAddress(),
		.SizeInBytes = vertexBufferSize,
		.StrideInBytes = sizeof(Vertex),
	};
	m_ibuffer_description =
	{
		.BufferLocation = m_index_buffer->GetGPUVirtualAddress(),
		.SizeInBytes = index_buffer_size,
		.Format = DXGI_FORMAT_R16_UINT,
	};
}

void Renderer::render()
{
	DEBUG_ASSERT(m_command_allocator->Reset());
	DEBUG_ASSERT(m_command_list->Reset(m_command_allocator.Get(), m_pipeline_state.Get()));

	const int back_buffer_index = m_swap_chain->GetCurrentBackBufferIndex();

	m_command_list->SetGraphicsRootSignature(m_root_signature.Get());
	m_command_list->SetPipelineState(m_pipeline_state.Get());
	m_command_list->RSSetViewports(1, &m_viewport);
	m_command_list->RSSetScissorRects(1, &m_scissor_rect);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), back_buffer_index, m_rtv_descriptor_size);

	const CD3DX12_RESOURCE_BARRIER swap_to_write = CD3DX12_RESOURCE_BARRIER::Transition(m_rtvs[back_buffer_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_command_list->ResourceBarrier(1, &swap_to_write);

	constexpr FLOAT clear_color[] = {0.4f, 0.6f, 0.9f, 1.0f};
	m_command_list->OMSetRenderTargets(1, &rtv, FALSE, nullptr);


	m_command_list->ClearRenderTargetView(rtv, clear_color, 0, nullptr);
	m_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_command_list->IASetVertexBuffers(0, 1, &m_vbuffer_description);
	m_command_list->IASetIndexBuffer(&m_ibuffer_description);

	m_command_list->SetGraphicsRoot32BitConstants(0, sizeof(m_world) / 4, &m_world, 0);

	m_command_list->DrawIndexedInstanced(m_index_count, 1, 0, 0, 0);

	const CD3DX12_RESOURCE_BARRIER swap_to_present= CD3DX12_RESOURCE_BARRIER::Transition(m_rtvs[back_buffer_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_command_list->ResourceBarrier(1, &swap_to_present);

	DEBUG_ASSERT(m_command_list->Close());


	ID3D12CommandList* const command_lists[] = { m_command_list.Get() };
	m_command_queue->ExecuteCommandLists(std::size(command_lists), command_lists);
	DEBUG_ASSERT(m_swap_chain->Present(1, 0));
	block();
}

void Renderer::block()
{
	m_command_queue->Signal(m_fence.Get(), ++m_fence_value);
	if (m_fence->GetCompletedValue() < m_fence_value)
	{
		DEBUG_ASSERT(m_fence->SetEventOnCompletion(m_fence_value, m_fence_event));
		WaitForSingleObject(m_fence_event, INFINITE);
	}
}

std::wstring Renderer::get_shader_path(const std::wstring& name)
{
	std::wstring module_path = std::filesystem::current_path();
	return module_path + L"\\resources\\shaders\\" + name;
}

void Renderer::GetHardwareAdapter(
	IDXGIFactory1* pFactory,
	IDXGIAdapter1** ppAdapter,
	bool requestHighPerformanceAdapter)
{
	*ppAdapter = nullptr;

	ComPtr<IDXGIAdapter1> adapter;

	ComPtr<IDXGIFactory6> factory6;
	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
	{
		for (
			UINT adapterIndex = 0;
			SUCCEEDED(factory6->EnumAdapterByGpuPreference(
				adapterIndex,
				requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&adapter)));
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	if (adapter.Get() == nullptr)
	{
		for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	*ppAdapter = adapter.Detach();
}