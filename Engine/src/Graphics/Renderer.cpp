#include "Renderer.h"
#include "../Log/Error.hpp"
#include "../Window.h"
#include <d3dcompiler.h>
#include "DirectXTex.h"
#include "../Utils/Utils.h"

using namespace VE::GFX;
using namespace VE::GFX::Resources;
using namespace VE::Log;
using namespace VE::Error;
using namespace VE::Utils;
using namespace Microsoft::WRL;
using namespace DirectX;

Renderer::Renderer(Window* window) :
	m_window(window),
	m_fence_value(0),
	m_scissor_rect(0, 0, (LONG)window->get_width(), (LONG)window->get_height()),
	m_viewport(0.0f, 0.0f, float(m_window->get_width()), float(m_window->get_height()))
{

	//Initialize COM for DirectXTex
	DEBUG_ASSERT(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
	//Enable Debug
	{
		ComPtr<ID3D12Debug> debug_controller;
		DEBUG_ASSERT(
			D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)));
		debug_controller->EnableDebugLayer();
	}

	ComPtr<IDXGIFactory4> dxgi_factory;
	{
		DEBUG_ASSERT(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgi_factory)));

		ComPtr<IDXGIAdapter1> adapter;
		GetHardwareAdapter(dxgi_factory.Get(), &adapter, true);

		DEBUG_ASSERT(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)));
	}
	{
		D3D12_COMMAND_QUEUE_DESC description =
		{
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		};
		DEBUG_ASSERT(m_device->CreateCommandQueue(&description, IID_PPV_ARGS(&m_command_queue)));
	}
	m_swap_chain = new Swap_Chain(*m_window, *(dxgi_factory.Get()), *(m_command_queue.Get()));

	DEBUG_ASSERT(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_command_allocator)));
	DEBUG_ASSERT(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_command_allocator.Get(), m_pipeline_state.Get(), IID_PPV_ARGS(&m_command_list)));
	DEBUG_ASSERT(m_command_list->Close());

	DEBUG_ASSERT(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	setup_pipeline();
	load_data();



	//Upload buffers and stuff
	block();
}

void Renderer::setup_pipeline()
{
	{
		CD3DX12_ROOT_SIGNATURE_DESC root_sig_description;
		constexpr UINT root_param_count = 2;
		CD3DX12_ROOT_PARAMETER root_parameters[root_param_count] = {};
		{
			root_parameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
		}
		{
			const CD3DX12_DESCRIPTOR_RANGE descriptor_range = { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0 };
			root_parameters[1].InitAsDescriptorTable(1, &descriptor_range);
		}
		const CD3DX12_STATIC_SAMPLER_DESC static_sampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

		root_sig_description.Init(root_param_count, root_parameters, 1, &static_sampler , D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;

		DEBUG_ASSERT(D3D12SerializeRootSignature(&root_sig_description, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
		DEBUG_ASSERT(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_root_signature)));
	}

	ComPtr<ID3DBlob> vertex_shader;
	ComPtr<ID3DBlob> pixel_shader;
	ComPtr<ID3DBlob> vert_error;
	ComPtr<ID3DBlob> pixel_error;
	{
		DEBUG_ASSERT(D3DCompileFromFile(get_shader_path(L"vert.hlsl").c_str(), nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertex_shader, &vert_error), [](HRESULT result, Log_Builder& log_builder)
		{
			log_builder.add_log(L"vert shader error").print();//TODO: Reminder to disable line numbers and other metadata for consecutive log entries. Also add blob log entry
		});

		DEBUG_ASSERT(D3DCompileFromFile(get_shader_path(L"pixel.hlsl").c_str(), nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixel_shader, &pixel_error) , [](HRESULT result, Log_Builder& log_builder)
		{
			log_builder.add_log(L"pixel shader error").print(); 
		});
	}
	D3D12_INPUT_ELEMENT_DESC input_layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
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
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT m_dsv_format;
		};

		Pipeline_State_Stream pipeline_state_stream =
		{
			.m_root_sig = m_root_signature.Get(),
			.m_input_layout = CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT({ input_layout, std::size(input_layout) }),
			.m_primitive_type = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			.m_vs = CD3DX12_SHADER_BYTECODE(vertex_shader.Get()),
			.m_ps = CD3DX12_SHADER_BYTECODE(pixel_shader.Get()),
			.m_rtv_formats = CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS({{ DXGI_FORMAT_R8G8B8A8_UNORM }, 1 }),
			.m_dsv_format = DXGI_FORMAT_D32_FLOAT,
		};


		const D3D12_PIPELINE_STATE_STREAM_DESC pipeline_state_stream_desc = { sizeof(Pipeline_State_Stream), &pipeline_state_stream };
		DEBUG_ASSERT(m_device->CreatePipelineState(&pipeline_state_stream_desc, IID_PPV_ARGS(&m_pipeline_state)));
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
	Vertex vertex_buffer_data[] =
	{
		{ { -1.0f, -1.0f, -1.0f }, {0.0f, 0.0f} },
		{ { -1.0f, 1.0f, -1.0f }, {0.0f, 1.0f} },
		{ { 1.0f, 1.0f, -1.0f }, {1.0f, 1.0f} },
		{ { 1.0f, -1.0f, -1.0f }, {1.0f, 0.0f} },
		{ { -1.0f, -1.0f, 1.0f }, {0.0f, 1.0f} },
		{ { -1.0f, 1.0f, 1.0f }, {0.0f, 0.0f} },
		{ { 1.0f, 1.0f, 1.0f }, {1.0f, 0.0f} },
		{ { 1.0f, -1.0f, 1.0f }, {1.0f, 1.0f} },
	};
	constexpr UINT vertexBufferSize = sizeof(vertex_buffer_data);
	
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


	m_index_buffer_resource = new Buffer_Resource(*m_device.Get(), D3D12_HEAP_TYPE_UPLOAD, sizeof(index_buffer_data));
	m_ibv = new Index_Buffer_View(*m_index_buffer_resource, sizeof(index_buffer_data), DXGI_FORMAT_R16_UINT);

	m_vertex_buffer_resource = new Buffer_Resource(*m_device.Get(), D3D12_HEAP_TYPE_UPLOAD, sizeof(vertex_buffer_data));
	m_vbv = new Vertex_Buffer_View(*m_vertex_buffer_resource, sizeof(vertex_buffer_data), sizeof(Vertex));


	//----------------------------------------- Copying data from cpu to upload buffer
	{
		Vertex* gpu_vbuffer_handle;
		CD3DX12_RANGE read_range(0, 0);
		DEBUG_ASSERT(m_vertex_buffer_resource->Get()->Map(0, &read_range, reinterpret_cast<void**>(&gpu_vbuffer_handle)));
		memcpy(gpu_vbuffer_handle, vertex_buffer_data, sizeof(vertex_buffer_data));
		m_vertex_buffer_resource->Get()->Unmap(0, nullptr);
	}
	{
		unsigned int* gpu_ibuffer_handle;
		CD3DX12_RANGE read_range(0, 0);
		DEBUG_ASSERT(m_index_buffer_resource->Get()->Map(0, &read_range, (void**)(&gpu_ibuffer_handle)));
		memcpy(gpu_ibuffer_handle, index_buffer_data, sizeof(index_buffer_data));
		m_index_buffer_resource->Get()->Unmap(0, nullptr);
	}

	{
		m_depth_stencil_resource = new Texture_Resource(*m_device.Get(), m_window->get_width(), m_window->get_height());
		m_dsv_heap = new Resource_View_Heap(*m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
		m_dsv = new Depth_Stencil_View(*m_device.Get(), *m_depth_stencil_resource, *m_dsv_heap);
	}
	{
		m_rtv_heap = new Resource_View_Heap(*m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2);
		for (auto iterator = m_rtv_heap->CPU_begin(); iterator != m_rtv_heap->CPU_end(); ++iterator)
		{
			m_render_target_resources[iterator.index] = new Resource(*m_swap_chain, iterator.index);
			m_rtvs[iterator.index] = new Render_Target_View(*m_device.Get(), *m_render_target_resources[iterator.index], *iterator);
		}
	}
	//Create texture resource
	{
		ScratchImage mip_chain;
		ScratchImage image;
		DEBUG_ASSERT(LoadFromWICFile(get_shader_path(L"clay.png").c_str(), WIC_FLAGS_NONE, nullptr, image));
		DEBUG_ASSERT(GenerateMipMaps(*(image.GetImages()), TEX_FILTER_BOX, 0, mip_chain));
		const Image& base = *mip_chain.GetImages(); //This is the first image in the mipchain

		m_texture_resource = new Texture_Resource(*m_device.Get(), D3D12_HEAP_TYPE_DEFAULT, base.width, base.height, mip_chain.GetImageCount(), base.format);
		m_srv_heap = new Resource_View_Heap(*m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);



		std::vector<D3D12_SUBRESOURCE_DATA> sub_resources;
		for (UINT i = 0; i < mip_chain.GetImageCount(); i++)
		{
			const Image& img = *mip_chain.GetImage(i, 0, 0);
			D3D12_SUBRESOURCE_DATA subresource =
			{
				.pData = img.pixels,
				.RowPitch = (LONG_PTR)img.rowPitch,
				.SlicePitch = (LONG_PTR)img.slicePitch, 
			};
			sub_resources.push_back(subresource);
		}
		{
			const UINT64 buffer_size = GetRequiredIntermediateSize(m_texture_resource->Get(), 0, (UINT)sub_resources.size());
			Buffer_Resource image_upload_buffer(*m_device.Get(), D3D12_HEAP_TYPE_UPLOAD, buffer_size);

			DEBUG_ASSERT(m_command_allocator->Reset());
			DEBUG_ASSERT(m_command_list->Reset(m_command_allocator.Get(), nullptr));
			UpdateSubresources
			(
				m_command_list.Get(),
				m_texture_resource->Get(),
				image_upload_buffer.Get(),
				0, 0,
				(UINT)sub_resources.size(),
				sub_resources.data()
			);
			const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition
			(
				m_texture_resource->Get(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);
			m_command_list->ResourceBarrier(1, &barrier);
			DEBUG_ASSERT(m_command_list->Close());
			ID3D12CommandList* const command_lists[] = { m_command_list.Get() };
			m_command_queue->ExecuteCommandLists(1, command_lists);
			block();
		}

	}

	//-------------------------------------------- Fill descriptor heaps and ascociate with resources. 
	{
		const D3D12_SHADER_RESOURCE_VIEW_DESC desc =
		{
			.Format = m_texture_resource->Get()->GetDesc().Format,
			.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Texture2D =
			{
				.MipLevels = m_texture_resource->Get()->GetDesc().MipLevels,
			},
		};
		m_device->CreateShaderResourceView(m_texture_resource->Get(), &desc, *m_srv_heap->CPU_begin());
	}

}


void Renderer::render()
{
	DEBUG_ASSERT(m_command_allocator->Reset());
	DEBUG_ASSERT(m_command_list->Reset(m_command_allocator.Get(), m_pipeline_state.Get()));

	const int back_buffer_index = m_swap_chain->Get()->GetCurrentBackBufferIndex();

	m_command_list->SetGraphicsRootSignature(m_root_signature.Get());
	m_command_list->SetPipelineState(m_pipeline_state.Get());
	m_command_list->RSSetViewports(1, &m_viewport);
	m_command_list->RSSetScissorRects(1, &m_scissor_rect);

	const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(*m_rtv_heap->CPU_begin()[back_buffer_index]);

	const CD3DX12_GPU_DESCRIPTOR_HANDLE srv_handle(*m_srv_heap->GPU_begin());

	const CD3DX12_RESOURCE_BARRIER swap_to_write = CD3DX12_RESOURCE_BARRIER::Transition(m_rtvs[back_buffer_index]->get_resource().Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_command_list->ResourceBarrier(1, &swap_to_write);

	constexpr FLOAT clear_color[] = {0.4f, 0.6f, 0.9f, 1.0f};
	m_command_list->OMSetRenderTargets(1, &rtv_handle, FALSE, &(*m_dsv_heap->CPU_begin()));

	m_command_list->SetDescriptorHeaps(1, m_srv_heap->get_address());

	m_command_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);
	m_command_list->ClearDepthStencilView(*m_dsv_heap->CPU_begin(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	m_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_command_list->IASetVertexBuffers(0, 1, &m_vbv->get_view_description());
	m_command_list->IASetIndexBuffer(&m_ibv->get_view_description());

	m_command_list->SetGraphicsRoot32BitConstants(0, sizeof(m_world) / 4, &m_world, 0);
	m_command_list->SetGraphicsRootDescriptorTable(1, srv_handle);

	m_command_list->DrawIndexedInstanced(m_index_count, 1, 0, 0, 0);

	const CD3DX12_RESOURCE_BARRIER swap_to_present= CD3DX12_RESOURCE_BARRIER::Transition(m_rtvs[back_buffer_index]->get_resource().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_command_list->ResourceBarrier(1, &swap_to_present);

	DEBUG_ASSERT(m_command_list->Close());


	ID3D12CommandList* const command_lists[] = { m_command_list.Get() };
	m_command_queue->ExecuteCommandLists(std::size(command_lists), command_lists);
	DEBUG_ASSERT(m_swap_chain->Get()->Present(1, 0));
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

void Renderer::GetHardwareAdapter( //This is yoinked straight from the Microsoft example code. Should probably look at at some point
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

Renderer::~Renderer()
{
	block();

	delete(m_swap_chain);
	delete(m_vertex_buffer_resource);
	delete(m_vbv);
	delete(m_index_buffer_resource);
	delete(m_ibv);
	delete(m_depth_stencil_resource);
	delete(m_dsv);
	delete(m_dsv_heap);
}
