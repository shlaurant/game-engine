#include "directx_12.h"
#include "debug.h"
#include "dx_util.h"

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace fuse::directx {
    void directx_12::init(const window_info &info) {
        {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(
                    D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
                debugController->EnableDebugLayer();
            }
        }

        init_base(info);
        init_cmds();
        init_swap_chain(info);
        init_rtv();
        init_dsv(info);

        init_vp();
        init_resources();

        init_root_signature();
        init_shader();


        //window resize;
        RECT rect = {0, 0, info.width, info.height};
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
        SetWindowPos(info.hwnd, 0, 100, 100, info.width, info.height, 0);
    }

    ComPtr<ID3D12Resource>
    directx_12::create_default_buffer(const void *data, UINT64 byte_size,
                                      ComPtr<ID3D12Resource> &upload_buffer) {
        auto tmph_desc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto tmp_desc = CD3DX12_RESOURCE_DESC::Buffer(byte_size);
        _device->CreateCommittedResource(&tmph_desc, D3D12_HEAP_FLAG_NONE,
                                         &tmp_desc,
                                         D3D12_RESOURCE_STATE_GENERIC_READ,
                                         nullptr,
                                         IID_PPV_ARGS(&upload_buffer));

        ComPtr<ID3D12Resource> default_buffer;
        auto dbh_desc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto db_desc = CD3DX12_RESOURCE_DESC::Buffer(byte_size);
        _device->CreateCommittedResource(&dbh_desc, D3D12_HEAP_FLAG_NONE,
                                         &db_desc, D3D12_RESOURCE_STATE_COMMON,
                                         nullptr,
                                         IID_PPV_ARGS(&default_buffer));

        D3D12_SUBRESOURCE_DATA sub_data = {};
        sub_data.pData = data;
        sub_data.RowPitch = byte_size;
        sub_data.SlicePitch = sub_data.RowPitch;

        auto b0 = CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.Get(),
                                                       D3D12_RESOURCE_STATE_COMMON,
                                                       D3D12_RESOURCE_STATE_COPY_DEST);
        _cmd_list->ResourceBarrier(1, &b0);
        UpdateSubresources<1>(_cmd_list.Get(), default_buffer.Get(),
                              upload_buffer.Get(),
                              0, 0, 1, &sub_data);
        auto b1 = CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.Get(),
                                                       D3D12_RESOURCE_STATE_COPY_DEST,
                                                       D3D12_RESOURCE_STATE_GENERIC_READ);
        _cmd_list->ResourceBarrier(1, &b1);

        return default_buffer;
    }

    void directx_12::init_geometries(std::vector<geometry> &geometries) {
        _cmd_list->Reset(_cmd_alloc.Get(), nullptr);

        ComPtr<ID3D12Resource> u_buffer_v;
        std::vector<vertex> vertices;
        size_t index = 0;
        for (auto &e: geometries) {
            e.vertex_offset = index;
            std::copy(e.vertices.begin(), e.vertices.end(),
                      std::back_inserter(vertices));
            index += e.vertices.size();
        }
        auto vert_arr = new vertex[vertices.size()];
        auto vert_byte_size = sizeof(vertex) * vertices.size();
        std::copy(vertices.begin(), vertices.end(), vert_arr);

        _vertex_buffer = create_default_buffer(vert_arr, vert_byte_size,
                                               u_buffer_v);
        _vertex_buffer_view.BufferLocation = _vertex_buffer->GetGPUVirtualAddress();
        _vertex_buffer_view.StrideInBytes = sizeof(vertex);
        _vertex_buffer_view.SizeInBytes = vert_byte_size;

        ComPtr<ID3D12Resource> u_buffer_i;
        std::vector<uint16_t> indices;
        size_t index_i = 0;
        for (auto &e: geometries) {
            e.index_offset = index_i;
            std::copy(e.indices.begin(), e.indices.end(),
                      std::back_inserter(indices));
            index_i += e.indices.size();
        }
        auto index_arr = new uint16_t[indices.size()];
        std::copy(indices.begin(), indices.end(), index_arr);
        _index_buffer = create_default_buffer(index_arr,
                                              sizeof(uint16_t) * indices.size(),
                                              u_buffer_i);
        _index_buffer_view.BufferLocation = _index_buffer->GetGPUVirtualAddress();
        _index_buffer_view.Format = DXGI_FORMAT_R16_UINT;
        _index_buffer_view.SizeInBytes = sizeof(uint16_t) * indices.size();

        _cmd_list->Close();
        execute_cmd_list();
        wait_cmd_queue_sync();
        delete[] vert_arr;
        delete[] index_arr;
    }

    void directx_12::set_vp(const DirectX::SimpleMath::Matrix &vp) {
        Matrix t[] = {vp};
        update_const_buffer(_vp_buffer, t, _countof(t));
    }

    void
    directx_12::set_ojb_w(const std::vector<DirectX::SimpleMath::Matrix> &ws) {
        auto tmp = new Matrix[ws.size()];
        for (size_t i = 0; i < ws.size(); ++i) {
            tmp[i] = ws[i];
        }
        update_const_buffer(_w_buffer, tmp, 10);
        delete[] tmp;
    }

    void directx_12::render_begin() {
        ThrowIfFailed(_cmd_alloc->Reset())
        ThrowIfFailed(_cmd_list->Reset(_cmd_alloc.Get(), _pipeline_state.Get()))
        _cmd_list->SetGraphicsRootSignature(_signature.Get());
        _cmd_list->SetGraphicsRootConstantBufferView(0,
                                                     _vp_buffer->GetGPUVirtualAddress());
        ID3D12DescriptorHeap *heaps[] = {_w_desc_heap.Get()};
        _cmd_list->SetDescriptorHeaps(1, heaps);

        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                _rtv_buffer[_back_buffer].Get(), D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET);
        _cmd_list->ResourceBarrier(1, &barrier);
        _cmd_list->RSSetViewports(1, &_view_port);
        _cmd_list->RSSetScissorRects(1, &_scissors_rect);
        _cmd_list->ClearRenderTargetView(_rtv_handle[_back_buffer],
                                         DirectX::Colors::Aqua, 0, nullptr);
        _cmd_list->ClearDepthStencilView(_dsv_handle, D3D12_CLEAR_FLAG_DEPTH,
                                         1.f, 0, 0, nullptr);
        _cmd_list->OMSetRenderTargets(1, &_rtv_handle[_back_buffer], FALSE,
                                      &_dsv_handle);
    }

    void directx_12::render_end() {
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                _rtv_buffer[_back_buffer].Get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT);
        _cmd_list->ResourceBarrier(1, &barrier);
        ThrowIfFailed(_cmd_list->Close())

        execute_cmd_list();
        _swap_chain->Present(0, 0);
        _back_buffer = (_back_buffer + 1) % SWAP_CHAIN_BUFFER_COUNT;

        wait_cmd_queue_sync();
    }

    void directx_12::render(const geometry &geo) {
        D3D12_VERTEX_BUFFER_VIEW tv[] = {_vertex_buffer_view};
        _cmd_list->IASetVertexBuffers(0, 1, tv);
        _cmd_list->IASetIndexBuffer(&_index_buffer_view);
        _cmd_list->IASetPrimitiveTopology(
                D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        auto handle = _w_desc_heap->GetGPUDescriptorHandleForHeapStart();
        handle.ptr += _device->GetDescriptorHandleIncrementSize(
                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * geo.w_offset;
        _cmd_list->SetGraphicsRootDescriptorTable(1, handle);

        _cmd_list->DrawIndexedInstanced(geo.indices.size(), 1, geo.index_offset,
                                        geo.vertex_offset, 0);
    }

    void directx_12::init_base(const window_info &info) {
        CreateDXGIFactory1(IID_PPV_ARGS(&_factory));
        D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0,
                          IID_PPV_ARGS(&_device));
        _view_port = {0, 0, static_cast<float>(info.width),
                      static_cast<float>(info.height), 0, 0};
        _scissors_rect = CD3DX12_RECT{0, 0, info.width, info.height};
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

    void directx_12::init_swap_chain(const window_info &info) {
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
        ThrowIfFailed(_factory->CreateSwapChain(_cmd_queue.Get(), &swap_desc,
                                                &_swap_chain))
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

    void directx_12::init_dsv(const window_info &info) {
        auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
                                                 info.width, info.height);
        desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        auto clear_value = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.f, 0);
        _device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE,
                                         &desc,
                                         D3D12_RESOURCE_STATE_COMMON,
                                         &clear_value,
                                         IID_PPV_ARGS(&_dsv_buffer));

        D3D12_DESCRIPTOR_HEAP_DESC dh_desc = {};
        dh_desc.NumDescriptors = 1;
        dh_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dh_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        dh_desc.NodeMask = 0;

        _device->CreateDescriptorHeap(&dh_desc, IID_PPV_ARGS(&_dsv_desc_heap));
        _dsv_handle = _dsv_desc_heap->GetCPUDescriptorHandleForHeapStart();
        _device->CreateDepthStencilView(_dsv_buffer.Get(), nullptr,
                                        _dsv_handle);

        _cmd_list->Reset(_cmd_alloc.Get(), nullptr);
        auto b = CD3DX12_RESOURCE_BARRIER::Transition(_dsv_buffer.Get(),
                                                      D3D12_RESOURCE_STATE_COMMON,
                                                      D3D12_RESOURCE_STATE_DEPTH_WRITE);
        _cmd_list->ResourceBarrier(1, &b);
        _cmd_list->Close();
        execute_cmd_list();
        wait_cmd_queue_sync();
    }

    void directx_12::init_vp() {
        _vp_buffer = create_const_buffer<Matrix>(1,
                                                 _device);
        set_vp(Matrix::Identity);
    }

    void directx_12::init_resources() {
        _w_buffer = create_const_buffer<Matrix>(OBJ_CNT, _device);

        D3D12_DESCRIPTOR_HEAP_DESC h_desc = {};
        h_desc.NodeMask = 0;
        h_desc.NumDescriptors = OBJ_CNT;
        h_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        h_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        _device->CreateDescriptorHeap(&h_desc, IID_PPV_ARGS(&_w_desc_heap));

        auto w_addr = _w_buffer->GetGPUVirtualAddress();

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
        cbv_desc.SizeInBytes = size_of_256<Matrix>();
        cbv_desc.BufferLocation = w_addr;

        _device->CreateConstantBufferView(&cbv_desc, _w_desc_heap->GetCPUDescriptorHandleForHeapStart());
    }

    void directx_12::init_root_signature() {
        _sampler_desc = CD3DX12_STATIC_SAMPLER_DESC(0);
        CD3DX12_DESCRIPTOR_RANGE ranges[] = {
                CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1),
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

    void directx_12::init_shader() {
#if defined(_DEBUG)
        UINT compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compile_flags = 0;
#endif
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(
                D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VS_Main",
                                   "vs_5_0", compile_flags, 0, &_vertex_shader,
                                   &error))

        ThrowIfFailed(
                D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PS_Main",
                                   "ps_5_0", compile_flags, 0, &_pixel_shader,
                                   &error))


        D3D12_INPUT_ELEMENT_DESC ie_desc[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                {"COLOR",    0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC ps_desc = {};
        ps_desc.InputLayout = {ie_desc, _countof(ie_desc)};
        ps_desc.pRootSignature = _signature.Get();
        ps_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        ps_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        ps_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        ps_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        ps_desc.SampleMask = UINT_MAX;
        ps_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        ps_desc.NumRenderTargets = 1;
        ps_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        ps_desc.SampleDesc.Count = 1;
        ps_desc.VS = {
                reinterpret_cast<UINT8 *>(_vertex_shader->GetBufferPointer()),
                _vertex_shader->GetBufferSize()};
        ps_desc.PS = {
                reinterpret_cast<UINT8 *>(_pixel_shader->GetBufferPointer()),
                _pixel_shader->GetBufferSize()};

        _device->CreateGraphicsPipelineState(&ps_desc,
                                             IID_PPV_ARGS(&_pipeline_state));
    }

    void directx_12::execute_cmd_list() {
        ID3D12CommandList *cmd_list_arr[] = {_cmd_list.Get()};
        _cmd_queue->ExecuteCommandLists(_countof(cmd_list_arr), cmd_list_arr);
    }

    void directx_12::wait_cmd_queue_sync() {
        ++_fence_value;
        _cmd_queue->Signal(_fence.Get(), _fence_value);
        if (_fence->GetCompletedValue() < _fence_value) {
            auto fence_event = CreateEventEx(nullptr, nullptr, false,
                                             EVENT_ALL_ACCESS);
            _fence->SetEventOnCompletion(_fence_value, fence_event);
            WaitForSingleObject(fence_event, INFINITE);
            CloseHandle(fence_event);
        }
    }
}