#include "directx_12.h"

namespace fuse {
    void directx_12::init(const WindowInfo &info) {
        CreateDXGIFactory(IID_PPV_ARGS(&_factory));
        D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0,
                          IID_PPV_ARGS(&_device));

        //create cmd brothers
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
        _fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        //create swap chain
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
        _factory->CreateSwapChain(_cmd_queue.Get(), &swap_desc, &_swap_chain);

        for (auto i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i) {
            _swap_chain->GetBuffer(i, IID_PPV_ARGS(&_rtv_buffer[i]));
        }

        //create desc heap and bind desc cpu handle with member
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
            _device->CreateRenderTargetView(_rtv_buffer[i].Get(), nullptr,
                                            _rtv_handle[i]);
        }

        //root signature
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
    }
}