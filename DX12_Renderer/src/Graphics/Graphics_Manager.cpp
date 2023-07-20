#include "Graphics_Manager.h"
#include "../Window.h"
#include "../Input_Manager.h"

//TODO: Investigate the different numbered D3D and DXGI methods and structs
#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "dxgi.lib")

void Graphics_Manager::init(HWND window_handle)
{
    //Enable Debug
    const UINT buffer_count = 2;
    {
        ComPtr<ID3D12Debug> debug_controller;
        D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller));
        debug_controller->EnableDebugLayer();
    }
    //Create Device (Check notes as to what this is) and dxgi factory
    //This is probably not the correct way to do this. We should actually use ID3D12Adapter instead of this generic device. Just requires more work
    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
    CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgi_factory));
    
    //Create command queue
    {
        const D3D12_COMMAND_QUEUE_DESC command_queue_desc =
        {
            .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
            .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
            .NodeMask = 0,
        };
        m_device->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(&m_command_queue));
    }
    //Create Swap Chain
    //Interesting how DXGI descriptions cannot be const. Also the factory works a little different than device
    {
        ComPtr<IDXGISwapChain> tmp_swap_chain;
        DXGI_SWAP_CHAIN_DESC swap_chain_desc =
        {
            .BufferDesc =
            {
                .Width = 1920,
                .Height = 1080,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            },
            .SampleDesc =
            {
                .Count = 1,
            },
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = buffer_count,
            .OutputWindow = window_handle,
            .Windowed = TRUE,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        };
        m_dxgi_factory->CreateSwapChain(m_command_queue.Get(), &swap_chain_desc, &tmp_swap_chain); //Investigate SwapChainAs() method for switching b/w implimentations
        tmp_swap_chain.As(&m_swap_chain); //Why the fuck can I not call tmp_swap_chain.Get()? Why do I need to also free this com object as well? Is that it?
    }
    
    //Create Render Target View/descriptor array (heap)
    {
        const D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_descriptor =
        {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = buffer_count, //This is the number of frame buffers you have
        };
    
        m_device->CreateDescriptorHeap(&rtv_heap_descriptor, IID_PPV_ARGS(&m_rtv_heap));
        m_rtv_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); //This is based on individual hardware
    }    
    //Create RTV for each frame (frame buffer)
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart());
        for( UINT i = 0; i < buffer_count; i++ )
        {
            m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&m_rtvs[i]));
            m_device->CreateRenderTargetView(m_rtvs[i].Get(), nullptr, rtv_handle);
            rtv_handle.Offset(1, m_rtv_size);
        }
    }    
    //Create command allacator and command list
    m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_command_allocator));

    m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_command_allocator.Get(),nullptr, IID_PPV_ARGS(&m_command_list));
    m_command_list->Close(); //Beginning of rendering loop requires command loop closed state
    
    //Create fence ( syncs CPU and GPU with flags) and Fence event (that we wait for to sync)
    m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));

    m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if( m_fence_event == nullptr )
    {
        //Error
    }

        //Empty root signature - defines resources bound to pipeline

        //Compile shaders

        //Vertex input layout (is this just a vertex layout?)

        //Pipeline state object description, then create pipeline object

        //Create vbuffers and load

        //Create vbuffer views
    
        //Wait for GPU to finish using fence


    //----------------------------------------------------
    //Temp render loop
    //----------------------------------------------------
    
}


void Graphics_Manager::update()
{
        ComPtr<ID3D12Resource> back_frame_buffer_descriptor = m_rtvs[m_swap_chain->GetCurrentBackBufferIndex()];
        m_command_allocator->Reset();
        m_command_list->Reset(m_command_allocator.Get(), nullptr);

        //Clear Render Target
        const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
        (
            back_frame_buffer_descriptor.Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );
        m_command_list->ResourceBarrier(1, &barrier);

        FLOAT clear_color[] = { 0.4f, 0.6f, 0.9f, 1.0f };

        
        CD3DX12_CPU_DESCRIPTOR_HANDLE tmp_descriptor_handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtv_heap->GetCPUDescriptorHandleForHeapStart(), m_swap_chain->GetCurrentBackBufferIndex(), m_rtv_size);
        m_command_list->ClearRenderTargetView(tmp_descriptor_handle, clear_color, 0, nullptr);

        const CD3DX12_RESOURCE_BARRIER second_barrier = CD3DX12_RESOURCE_BARRIER::Transition
        (
            back_frame_buffer_descriptor.Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT
        );
        m_command_list->ResourceBarrier(1, &second_barrier);

        m_command_list->Close();
        ID3D12CommandList* const command_lists[] = { m_command_list.Get() };
        m_command_queue->ExecuteCommandLists(1, command_lists);
        m_command_queue->Signal(m_fence.Get(), m_fence_count++);

        m_swap_chain->Present(0, 0);

        m_fence->SetEventOnCompletion(m_fence_count - 1, m_fence_event);
        if( WaitForSingleObject(m_fence_event, INFINITE) == WAIT_FAILED)
        {
            //ERROR
        }
    //Modify Constants (I think these are opengl uniforms)

    //Modify vbuffers, ibuffers etc...
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

Graphics_Manager::Graphics_Manager() :
    m_rtv_size(0)
{
}

Graphics_Manager::~Graphics_Manager()
{
}


