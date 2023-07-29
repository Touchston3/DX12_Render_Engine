#include "Graphics_Manager.h"
#include "../Window.h"
#include "../Input_Manager.h"
#include "../Log/Log_Manager.h"
#include <d3dcompiler.h>
#include <filesystem>
#include <dxgidebug.h>

//TODO: Investigate the different numbered D3D and DXGI methods and structs

void Graphics_Manager::init(HWND window_handle)
{
    //Enable Debug
    {
        ComPtr<ID3D12Debug> debug_controller;
        GRAPHICS_ASSERT(
            D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)));
        debug_controller->EnableDebugLayer();
    }
    //Create Device (Check notes as to what this is) and dxgi factory
    //This is probably not the correct way to do this. We should actually use ID3D12Adapter instead of this generic device. Just requires more work
    GRAPHICS_ASSERT(
        CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgi_factory)));

    ComPtr<IDXGIAdapter1> hardware_adapter;
    get_hardware_adapter(m_dxgi_factory.Get(), &hardware_adapter, false);
    GRAPHICS_ASSERT(D3D12CreateDevice(
        hardware_adapter.Get(),
        D3D_FEATURE_LEVEL_12_0,
        IID_PPV_ARGS(&m_device)
    ));

    //Create command queue
    {
        D3D12_COMMAND_QUEUE_DESC command_queue_desc = {};
        command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        GRAPHICS_ASSERT(
            m_device->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(&m_command_queue)));
    }
    //Create Swap Chain
    //Interesting how DXGI descriptions cannot be const. Also the factory works a little different than device
    {
        ComPtr<IDXGISwapChain1> tmp_swap_chain;
        DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
        swap_chain_desc.BufferCount = m_buffer_count;
        swap_chain_desc.Width = m_width;
        swap_chain_desc.Height = m_height;
        swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swap_chain_desc.SampleDesc.Count = 1;

        GRAPHICS_ASSERT(
            m_dxgi_factory->CreateSwapChainForHwnd(
                m_command_queue.Get(),
                window_handle,
                &swap_chain_desc,
                nullptr,
                nullptr,
                &tmp_swap_chain));

        GRAPHICS_ASSERT(
            m_dxgi_factory->MakeWindowAssociation(window_handle, DXGI_MWA_NO_ALT_ENTER));
        GRAPHICS_ASSERT(
            tmp_swap_chain.As(&m_swap_chain));
    }

    //Create Render Target View/descriptor array (heap)
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_descriptor = {};
        rtv_heap_descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtv_heap_descriptor.NumDescriptors = m_buffer_count; //This is the number of frame buffers you have
        rtv_heap_descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        GRAPHICS_ASSERT(
            m_device->CreateDescriptorHeap(&rtv_heap_descriptor, IID_PPV_ARGS(&m_rtv_heap)));

        m_rtv_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        //This is based on individual hardware
    }
    //Create RTV for each frame buffer
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart());
        for (UINT i = 0; i < m_buffer_count; i++)
        {
            GRAPHICS_ASSERT(
                m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&m_rtvs[i])));

            m_device->CreateRenderTargetView(m_rtvs[i].Get(), nullptr, rtv_handle);
            rtv_handle.Offset(1, m_rtv_size);
        }
    }
    //Create command allacator and command list
    GRAPHICS_ASSERT(
        m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_command_allocator)));


    {
        CD3DX12_ROOT_SIGNATURE_DESC root_signature_desc;
        //Man I hate how different all of these different libraries and apis handle initiation. Like, an Init function? In this economy?
        root_signature_desc.Init
        (
            0,
            nullptr,
            0,
            nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        );
        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        GRAPHICS_ASSERT(
            D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        //Okay. This turns the signature into some intermediary data. Stored in the signature blob. 
        GRAPHICS_ASSERT(
            m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&
                m_root_signature)));
        //Okay. So this root signature is a pointer to gpu memory. I guess that makes sense. Is the signature tho?
    }
    ComPtr<ID3DBlob> vshader_blob;
    ComPtr<ID3DBlob> fshader_blob;
    GRAPHICS_ASSERT(
        D3DCompileFromFile(get_shader_path(L"vert.hlsl").c_str(), nullptr, nullptr, "main", "vs_5_0",D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG,
            0, &vshader_blob, nullptr));
    GRAPHICS_ASSERT(
        D3DCompileFromFile(get_shader_path(L"frag.hlsl").c_str(), nullptr, nullptr, "main", "ps_5_0", D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG,
            0, &fshader_blob, nullptr));


    //Create Pipeline state object
    {
        constexpr D3D12_INPUT_ELEMENT_DESC input_layout[] =
        {
            {
                "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
            },
            {
                "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
            },
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_stream_description =
        {
            .pRootSignature = m_root_signature.Get(),
            .VS = CD3DX12_SHADER_BYTECODE(vshader_blob.Get()),
            .PS = CD3DX12_SHADER_BYTECODE(fshader_blob.Get()),
            .BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            .SampleMask = UINT_MAX,
            .RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            .DepthStencilState =
            {
                .DepthEnable = FALSE,
                .StencilEnable = FALSE,
            },
            .InputLayout =
            {
                .pInputElementDescs = input_layout,
                .NumElements = _countof(input_layout),
            },
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .SampleDesc =
            {
                .Count = 1,
            },
        };

        GRAPHICS_ASSERT(
            m_device->CreateGraphicsPipelineState(&pipeline_state_stream_description, IID_PPV_ARGS(&
                m_pipeline_state_object)));
    }
    GRAPHICS_ASSERT(
        m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_command_allocator.Get(),
            m_pipeline_state_object.Get() , IID_PPV_ARGS(&m_command_list)));
    GRAPHICS_ASSERT(
        m_command_list->Close()); //Beginning of rendering loop requires command loop closed state

    //Create, fill, and configure Vertex Buffer
    {
        constexpr UINT vert_count = 3;
        constexpr Vertex vert_data[vert_count] =
        {
            {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
            {{-0.5f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        };

        CD3DX12_HEAP_PROPERTIES buffer_heap_props(D3D12_HEAP_TYPE_UPLOAD);
        //The default one is the efficient one to use. However, you cannot upload data to this heap from the cpu. Thus we are using this for now. 
        CD3DX12_RESOURCE_DESC buffer_description = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vert_data));
        GRAPHICS_ASSERT(
            m_device->CreateCommittedResource
            //This creates a heap on the gpu behind the scenes and automatically manages it. Means you cannot share this memory with other resources
            (
                &buffer_heap_props,
                D3D12_HEAP_FLAG_NONE,
                &buffer_description,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_vertex_buffer)
            ));

        //Map vertex data on the cpu to the vertex buffer on the gpu.
        Vertex* v_buffer_handle = nullptr;
        //I think I need to delete this right? I guess not. That causes it to crash. Let's defnitely not do this in the render loop then.
        D3D12_RANGE read_range(0, 0);
        GRAPHICS_ASSERT(
            m_vertex_buffer->Map(0, &read_range, (void**)&v_buffer_handle)); //You can define ranges of data to be copied. 
        memcpy(v_buffer_handle, vert_data, sizeof(vert_data));
        m_vertex_buffer->Unmap(0, nullptr);

        //Init vertex buffer view
        m_vbuffer_view =
        {
            .BufferLocation = m_vertex_buffer->GetGPUVirtualAddress(),
            .SizeInBytes = sizeof(vert_data),
            .StrideInBytes = sizeof(Vertex),
        };
    }
    m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fence_event == nullptr)
    {
        DEBUG_LOG_FATAL(L"No fence event {}", "");
        //Error
    }
    //Create fence ( syncs CPU and GPU with flags) and Fence event (that we wait for to sync)
    GRAPHICS_ASSERT(
        m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

    GRAPHICS_ASSERT(m_command_queue->Signal(m_fence.Get(), ++m_fence_count));
    if (m_fence->GetCompletedValue() < m_fence_count)
    {
        GRAPHICS_ASSERT(m_fence->SetEventOnCompletion(m_fence_count, m_fence_event));
        WaitForSingleObject(m_fence_event, INFINITE);
    }
}


void Graphics_Manager::update()
{
    //Create root signature. This will contain constants and samplers and such. Why doesn't it also contain shader binaries? Why is it different from a PSO? I am not sure. I suppose maybe that will become apparent later. Maybe u want same constants for different shaders?
}


void Graphics_Manager::render()
{
    //Populate command list

    //Reset command list allocator (basically just clear that memory and resuse it)

    //Reset command list

    //set graphics root signature

    //set viewport and scissor rectangles (wtf does this mean?)

    //Set resource barrier stating backbuffer is current render target (how does a resource barrier do this?)

    //Record commands to command list

    //Indicate back buffer will be used upon command list execution

    //close command list to recording

    //Execute command list

    //Present frame (what does this mean?)

    //Wait for GPU to finish (use fence)
}

void Graphics_Manager::close()
{
    //Wait for GPU to finish (Once again using fence)

    //close event handle. WTF is this event handle. 
}

void Graphics_Manager::populate_command_list()
{
    const ComPtr<ID3D12Resource> back_buffer_rtv = m_rtvs[m_swap_chain->GetCurrentBackBufferIndex()];

    GRAPHICS_ASSERT(
        m_command_allocator->Reset());
    GRAPHICS_ASSERT(
        m_command_list->Reset(m_command_allocator.Get(), m_pipeline_state_object.Get()));

    m_command_list->SetGraphicsRootSignature(m_root_signature.Get());
    m_command_list->RSSetViewports(1, &m_viewport);
    m_command_list->RSSetScissorRects(1, &m_scissor_rect);

    // Create resource barrier to write to back buffer rtv
    {
        const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
        (
            back_buffer_rtv.Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );
        m_command_list->ResourceBarrier(1, &barrier);
    }
    const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart(),
                                                   m_swap_chain->GetCurrentBackBufferIndex(), m_rtv_size);
    m_command_list->OMSetRenderTargets(1, &rtv_handle, FALSE, nullptr);

    constexpr FLOAT clear_color[] = {0.4f, 0.6f, 0.0f, 0.5f};
    m_command_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);

    m_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_command_list->IASetVertexBuffers(0, 1, &m_vbuffer_view);
    m_command_list->DrawInstanced(3, 1, 0, 0);

    {
        const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
        (
            back_buffer_rtv.Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT
        );
        m_command_list->ResourceBarrier(1, &barrier);
    }
    GRAPHICS_ASSERT(
        m_command_list->Close());

    ID3D12CommandList* const command_lists[] = {m_command_list.Get()};
    m_command_queue->ExecuteCommandLists(1, command_lists);

    GRAPHICS_ASSERT(
        m_swap_chain->Present(1, 0));

    GRAPHICS_ASSERT(
        m_command_queue->Signal(m_fence.Get(), ++m_fence_count));

    if (m_fence->GetCompletedValue() < m_fence_count)
    {
        GRAPHICS_ASSERT(
            m_fence->SetEventOnCompletion(m_fence_count, m_fence_event));
        WaitForSingleObject(m_fence_event, INFINITE);
    }
}


Graphics_Manager::Graphics_Manager() :
    m_rtv_size(0),
    m_scissor_rect(CD3DX12_RECT(0, 0, 1920, 1080)),
    m_viewport(CD3DX12_VIEWPORT(0.0f, 0.0f, 1920.0f, 1080.0f))
{
}

std::wstring Graphics_Manager::get_shader_path(const std::wstring& filename)
{
    //current_dir += L"\\resources\\shaders\\";
    //current_dir += filename;
    //DEBUG_LOG_INFO(m_current_dir, "");
    return m_current_dir + L"\\resources\\shaders\\" + filename;
}

Graphics_Manager::~Graphics_Manager()
{
}

void Graphics_Manager::get_hardware_adapter(
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
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
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
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
}
