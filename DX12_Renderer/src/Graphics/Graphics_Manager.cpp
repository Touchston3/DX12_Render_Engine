#include "Graphics_Manager.h"
#include <dxgi1_6.h>
#include "../Window.h"
#include "../Input_Manager.h"

//TODO: Investigate the different numbered D3D and DXGI methods and structs
#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "dxgi.lib")

void Graphics_Manager::init(HWND* const window_handle, HINSTANCE hInstance)
{
    
    void_renderer::Window window(hInstance);
    window.attach_event_handler(void_renderer::Input_Manager::handle_window_input); //I feel like I shouldn't need this to be a static method.
    
    
    const UINT buffer_count = 2;
    //Enable Debug
    ComPtr<ID3D12Debug> debug_controller;
    D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller));
    debug_controller->EnableDebugLayer();
    
    //Create Device (Check notes as to what this is)
    //This is probably not the correct way to do this. We should actually use ID3D12Adapter instead of this generic device. Just requires more work
    ComPtr<IDXGIFactory4> dxgi_factory;
    CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgi_factory));

    ComPtr<ID3D12Device2> device;
    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
    
    //Create command queue
    ComPtr<ID3D12CommandQueue> command_queue;
    const D3D12_COMMAND_QUEUE_DESC  command_queue_desc =
    {
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
    };
    device->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(&command_queue));
    
    //Create Swap Chain
    //Interesting how DXGI descriptions cannot be const. Also the factory works a little different than device
    ComPtr<IDXGISwapChain3> swap_chain;
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
        .OutputWindow = window.get_window_handler(),
        .Windowed = TRUE,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
    };
    dxgi_factory->CreateSwapChain(command_queue.Get(), &swap_chain_desc, &tmp_swap_chain); //Investigate SwapChainAs() method for switching b/w implimentations
    tmp_swap_chain.As(&swap_chain); //Why the fuck can I not call tmp_swap_chain.Get()? Why do I need to also free this com object as well? Is that it?
    //Create Render Target View/descriptor array (heap)
    ComPtr<ID3D12DescriptorHeap> descriptor_heap;
    const D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc =
    {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        .NumDescriptors = buffer_count, //This is the number of frame buffers you have
    };
    device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&descriptor_heap));
    
    const UINT descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); //This is based on individual hardware
    
    //Create RTV for each frame (frame buffer)
    ComPtr<ID3D12Resource> frame_buffer_descriptors[buffer_count];
    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptor_handle(descriptor_heap->GetCPUDescriptorHandleForHeapStart());
    for( UINT i = 0; i < buffer_count; i++ )
    {
        swap_chain->GetBuffer(i, IID_PPV_ARGS(&frame_buffer_descriptors[i]));
        device->CreateRenderTargetView(frame_buffer_descriptors[i].Get(), nullptr, descriptor_handle);
        descriptor_handle.Offset(1, descriptor_size);
    }
    
    //Create command allacator and command list
    ComPtr<ID3D12CommandAllocator> command_allocator;
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator));

    ComPtr<ID3D12GraphicsCommandList> command_list;
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator.Get(), nullptr, IID_PPV_ARGS(&command_list));
    command_list->Close(); //Beginning of rendering loop requires command loop closed state
    
    //Create fence ( syncs CPU and GPU with flags) and Fence event (that we wait for to sync)
    ComPtr<ID3D12Fence> fence;
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

    HANDLE fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if( fence_event == nullptr )
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
    ShowWindow(window.get_window_handler(), SW_SHOW);
    UINT64 fence_count = 0;
    while( true )
    {
        ComPtr<ID3D12Resource> back_frame_buffer_descriptor = frame_buffer_descriptors[swap_chain->GetCurrentBackBufferIndex()];
        command_allocator->Reset();
        command_list->Reset(command_allocator.Get(), nullptr);

        //Clear Render Target
        const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
        (
            back_frame_buffer_descriptor.Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );
        command_list->ResourceBarrier(1, &barrier);

        FLOAT clear_color[] = { 0.4f, 0.6f, 0.9f, 1.0f };

        
        CD3DX12_CPU_DESCRIPTOR_HANDLE tmp_descriptor_handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptor_heap->GetCPUDescriptorHandleForHeapStart(), swap_chain->GetCurrentBackBufferIndex(), descriptor_size);
        command_list->ClearRenderTargetView(tmp_descriptor_handle, clear_color, 0, nullptr);

        const CD3DX12_RESOURCE_BARRIER second_barrier = CD3DX12_RESOURCE_BARRIER::Transition
        (
            back_frame_buffer_descriptor.Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT
        );
        command_list->ResourceBarrier(1, &second_barrier);

        command_list->Close();
        ID3D12CommandList* const command_lists[] = { command_list.Get() };
        command_queue->ExecuteCommandLists(1, command_lists);
        command_queue->Signal(fence.Get(), fence_count++);

        swap_chain->Present(0, 0);

        fence->SetEventOnCompletion(fence_count - 1, fence_event);
        if( WaitForSingleObject(fence_event, INFINITE) == WAIT_FAILED)
        {
            //ERROR
        }
    }
    
}


void Graphics_Manager::update()
{
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

Graphics_Manager::Graphics_Manager()
{
}

Graphics_Manager::~Graphics_Manager()
{
}


