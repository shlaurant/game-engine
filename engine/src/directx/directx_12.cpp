#include "directx_12.h"

namespace fuse {
    void directx_12::init(const WindowInfo &info) {
        init_base(info);
        init_cmds();
        init_swap_chain(info);
        init_rtv();
        init_root_signature();
        init_shader();


        //constant buffer
        auto buffer_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto res_desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(light_param));
        _device->CreateCommittedResource(&buffer_heap_prop,
                                         D3D12_HEAP_FLAG_NONE, &res_desc,
                                         D3D12_RESOURCE_STATE_GENERIC_READ,
                                         nullptr,
                                         IID_PPV_ARGS(&_t0));


        D3D12_DESCRIPTOR_HEAP_DESC cbv_dh_desc;
        cbv_dh_desc.NumDescriptors = 1;
        cbv_dh_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbv_dh_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        cbv_dh_desc.NodeMask = 0;
        _device->CreateDescriptorHeap(&cbv_dh_desc,
                                      IID_PPV_ARGS(&_t0_desc_heap));

        //buffers. more implementation needed
//        ComPtr<ID3D12DescriptorHeap> cbv_dh;
//        _device->CreateDescriptorHeap(&cbv_dh_desc, IID_PPV_ARGS(&cbv_dh));


        //window resize;
        RECT rect = {0, 0, info.width, info.height};
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
        SetWindowPos(info.hwnd, 0, 100, 100, info.width, info.height, 0);
    }

    void directx_12::init_root_signature() {
        _sampler_desc = CD3DX12_STATIC_SAMPLER_DESC(0);
        CD3DX12_DESCRIPTOR_RANGE ranges[] = {
                CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
                                         CBV_REGISTER_COUNT - 1, 1),
                CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                                         SRV_REGISTER_COUNT, 0),
        };
        CD3DX12_ROOT_PARAMETER param[2];
        param[0].InitAsConstantBufferView(
                static_cast<uint32_t>(CBV_REGISTER::b0));
        param[1].InitAsDescriptorTable(_countof(ranges), ranges);

        auto rs_desc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param, 1,
                                                   &_sampler_desc);
        rs_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ComPtr<ID3DBlob> blob_signature;
        ComPtr<ID3DBlob> blob_error;
        D3D12SerializeRootSignature(&rs_desc, D3D_ROOT_SIGNATURE_VERSION_1,
                                    &blob_signature, &blob_error);
        _device->CreateRootSignature(0, blob_signature->GetBufferPointer(),
                                     blob_signature->GetBufferSize(),
                                     IID_PPV_ARGS(&_signature));


        //descriptor table
        D3D12_DESCRIPTOR_HEAP_DESC regi_dh_desc = {};
        regi_dh_desc.NumDescriptors = REGISTER_COUNT - 1;
        regi_dh_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        regi_dh_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        _device->CreateDescriptorHeap(&regi_dh_desc,
                                      IID_PPV_ARGS(&_root_desc_table));
    }

    void directx_12::init_swap_chain(const WindowInfo &info) {
        _swap_chain.Reset();
        DXGI_SWAP_CHAIN_DESC swap_desc;
        swap_desc.BufferDesc.Width = static_cast<uint32_t>(info.width);
        swap_desc.BufferDesc.Height = static_cast<uint32_t>(info.height);
        swap_desc.BufferDesc.RefreshRate.Numerator = 60;
        swap_desc.BufferDesc.RefreshRate.Denominator = 1;
        swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swap_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swap_desc.SampleDesc.Count = 1;
        swap_desc.SampleDesc.Quality = 0;
        swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_desc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
        swap_desc.OutputWindow = info.hwnd;
        swap_desc.Windowed = info.windowed;
        swap_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swap_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        if(FAILED(_factory->CreateSwapChain(_cmd_queue.Get(), &swap_desc, &_swap_chain))){
            FUSE_ERROR("swap chain creation failed")
        }
    }

    void directx_12::init_rtv() {
        auto rtv_heap_size = _device->GetDescriptorHandleIncrementSize(
                D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_DESCRIPTOR_HEAP_DESC rtv_dh_desc;
        rtv_dh_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtv_dh_desc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
        rtv_dh_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtv_dh_desc.NodeMask = 0;
        _device->CreateDescriptorHeap(&rtv_dh_desc, IID_PPV_ARGS(&_rtv_heap));
        auto rtv_dh_begin = _rtv_heap->GetCPUDescriptorHandleForHeapStart();
        for (auto i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i) {
            _rtv_handle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtv_dh_begin,
                                                           rtv_heap_size * i);
            _swap_chain->GetBuffer(i, IID_PPV_ARGS(&_rtv_buffer[i]));
            _device->CreateRenderTargetView(_rtv_buffer[i].Get(), nullptr,
                                            _rtv_handle[i]);
        }
    }

    void directx_12::init_cmds() {
        D3D12_COMMAND_QUEUE_DESC queue_desc = {D3D12_COMMAND_LIST_TYPE_DIRECT,
                                               0, D3D12_COMMAND_QUEUE_FLAG_NONE,
                                               0};
        _device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&_cmd_queue));
        _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                        IID_PPV_ARGS(&_cmd_alloc));
        _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                   _cmd_alloc.Get(), nullptr,
                                   IID_PPV_ARGS(&_cmd_list));
        _cmd_list->Close();
        _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    }

    void directx_12::init_base(const WindowInfo &info) {
        CreateDXGIFactory(IID_PPV_ARGS(&_factory));
        D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0,
                          IID_PPV_ARGS(&_device));
        _view_port = {0, 0, static_cast<float>(info.width),
                      static_cast<float>(info.height), 0, 0};
        _scissors_rect = CD3DX12_RECT{0, 0, info.width, info.height};
    }

    void directx_12::render_begin() {
        _cmd_alloc->Reset();
        _cmd_list->Reset(_cmd_alloc.Get(), nullptr);
        _cmd_list->SetGraphicsRootSignature(_signature.Get());

        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                _rtv_buffer[_back_buffer].Get(), D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET);
        _cmd_list->ResourceBarrier(1, &barrier);
        _cmd_list->RSSetViewports(1, &_view_port);
        _cmd_list->RSSetScissorRects(1, &_scissors_rect);
        _cmd_list->ClearRenderTargetView(_rtv_handle[_back_buffer],
                                         DirectX::Colors::Aqua, 0, nullptr);
        _cmd_list->OMSetRenderTargets(1, &_rtv_handle[_back_buffer], FALSE,
                                      nullptr);
    }

    void directx_12::render_end() {
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                _rtv_buffer[_back_buffer].Get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT);
        _cmd_list->ResourceBarrier(1, &barrier);
        _cmd_list->Close();

        ID3D12CommandList *cmd_list_arr[] = {_cmd_list.Get()};
        _cmd_queue->ExecuteCommandLists(_countof(cmd_list_arr), cmd_list_arr);
        _swap_chain->Present(0, 0);

        ++_fence_value;
        _cmd_queue->Signal(_fence.Get(), _fence_value);
        if (_fence->GetCompletedValue() < _fence_value) {
            auto fence_event = CreateEventEx(nullptr, false, false,
                                             EVENT_ALL_ACCESS);
            _fence->SetEventOnCompletion(_fence_value, fence_event);
            WaitForSingleObject(fence_event, INFINITE);
            CloseHandle(fence_event);
        }

        _back_buffer = (_back_buffer + 1) % SWAP_CHAIN_BUFFER_COUNT;
    }

    void directx_12::init_shader() {
#if defined(_DEBUG)
        UINT compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compile_flags = 0;
#endif

        if(FAILED(D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VS_Main", "vs_5_0", compile_flags, 0, &_vertex_shader, nullptr))){
            FUSE_ERROR("failed to compile shader")
        }
        if(FAILED(D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PS_Main", "ps_5_0", compile_flags, 0, &_pixel_shader, nullptr))){
            FUSE_ERROR("failed to compile shader")
        }

        D3D12_INPUT_ELEMENT_DESC ie_desc[] = {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC ps_desc = {};
        ps_desc.InputLayout = {ie_desc, _countof(ie_desc)};
        ps_desc.pRootSignature = _signature.Get();
        ps_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        ps_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        ps_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        ps_desc.SampleMask = UINT_MAX;
        ps_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        ps_desc.NumRenderTargets = 1;
        ps_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        ps_desc.SampleDesc.Count = 1;
        ps_desc.VS = {reinterpret_cast<UINT8*>(_vertex_shader->GetBufferPointer()), _vertex_shader->GetBufferSize()};
        ps_desc.PS = {reinterpret_cast<UINT8*>(_pixel_shader->GetBufferPointer()), _pixel_shader->GetBufferSize()};

        _device->CreateGraphicsPipelineState(&ps_desc, IID_PPV_ARGS(&_pipeline_state));
    }
}