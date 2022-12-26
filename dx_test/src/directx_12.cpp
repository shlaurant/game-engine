#include "directx_12.h"
#include "debug.h"
#include "dx_util.h"
#include "ReadData.h"

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

        init_global_buf();
        init_camera_buf();
        init_light_buf();
        init_resources();

        init_root_signature();
        init_shader();

        //window resize;
        RECT rect = {0, 0, info.width, info.height};
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
        SetWindowPos(info.hwnd, 0, 100, 100, info.width, info.height, 0);
    }

    void directx_12::init_geometries(std::vector<geometry> &geometries) {
        ThrowIfFailed(_cmd_alloc->Reset());
        ThrowIfFailed(_cmd_list->Reset(_cmd_alloc.Get(), nullptr));

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
                                               u_buffer_v, _device, _cmd_list);
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
                                              u_buffer_i, _device, _cmd_list);
        _index_buffer_view.BufferLocation = _index_buffer->GetGPUVirtualAddress();
        _index_buffer_view.Format = DXGI_FORMAT_R16_UINT;
        _index_buffer_view.SizeInBytes = sizeof(uint16_t) * indices.size();

        _cmd_list->Close();
        execute_cmd_list();
        wait_cmd_queue_sync();
        delete[] vert_arr;
        delete[] index_arr;
    }

    void directx_12::update_camera(const camera &cam) {
        update_const_buffer<camera>(_vp_buffer, &cam, 0);
    }

    void directx_12::update_lights(const light_info &info) {
        update_const_buffer<light_info>(_light_buffer, &info, 0);
    }

    void
    directx_12::update_obj_constants(const std::vector<object_constant> &vec) {
        for (auto i = 0; i < vec.size(); ++i) {
            update_const_buffer(_obj_const_buffer, &(vec[i]), i);
        }
    }

    int directx_12::load_texture(const std::wstring &path) {
        ThrowIfFailed(_cmd_alloc->Reset());
        ThrowIfFailed(_cmd_list->Reset(_cmd_alloc.Get(), nullptr));

        DirectX::ScratchImage image;
        auto ext = std::filesystem::path(path).extension();

        if (ext == L".dds" || ext == L".DDS")
            LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, nullptr,
                            image);
        else if (ext == L".tga" || ext == L".TGA")
            LoadFromTGAFile(path.c_str(), nullptr, image);
        else
            LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, nullptr,
                            image);

        ComPtr<ID3D12Resource> buf;
        ThrowIfFailed(
                DirectX::CreateTexture(_device.Get(), image.GetMetadata(),
                                       &buf));

        std::vector<D3D12_SUBRESOURCE_DATA> sub_reses;
        ThrowIfFailed(DirectX::PrepareUpload(_device.Get(), image.GetImages(),
                                             image.GetImageCount(),
                                             image.GetMetadata(), sub_reses));

        auto ub = create_upload_buffer(1,
                                       GetRequiredIntermediateSize(buf.Get(), 0,
                                                                   static_cast<uint32_t>(sub_reses.size())),
                                       _device);
        UpdateSubresources(_cmd_list.Get(), buf.Get(), ub.Get(),
                           0, 0, static_cast<uint32_t>(sub_reses.size()),
                           sub_reses.data());

        _cmd_list->Close();
        execute_cmd_list();
        wait_cmd_queue_sync();

        D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = image.GetMetadata().format;
        desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        desc.Texture2D.MipLevels = 1;

        _texture_buffers.emplace_back(std::make_pair(desc, buf));
        return _texture_buffers.size() - 1;
    }

    void directx_12::bind_texture(int obj, int texture) {
        auto handle = _res_desc_heap->GetCPUDescriptorHandleForHeapStart();
        auto handle_sz = _device->GetDescriptorHandleIncrementSize(
                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        handle.ptr += obj * group_size();
        handle.ptr += handle_sz;
        auto texture_res = _texture_buffers[texture].second;
        auto desc = _texture_buffers[texture].first;
        _device->CreateShaderResourceView(texture_res.Get(), &desc, handle);
    }

    void directx_12::render_begin() {
        ThrowIfFailed(_cmd_alloc->Reset())
        ThrowIfFailed(_cmd_list->Reset(_cmd_alloc.Get(), nullptr))
        _cmd_list->SetGraphicsRootSignature(_signature.Get());
        _cmd_list->SetGraphicsRootConstantBufferView(0,
                                                     _global_buffer->GetGPUVirtualAddress());
        _cmd_list->SetGraphicsRootConstantBufferView(1,
                                                     _vp_buffer->GetGPUVirtualAddress());
        _cmd_list->SetGraphicsRootConstantBufferView(2,
                                                     _light_buffer->GetGPUVirtualAddress());
        _cmd_list->RSSetViewports(1, &_view_port);
        _cmd_list->RSSetScissorRects(1, &_scissors_rect);
        ID3D12DescriptorHeap *heaps[] = {_res_desc_heap.Get()};
        _cmd_list->SetDescriptorHeaps(1, heaps);

        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                _rtv_buffer[_back_buffer].Get(), D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET);
        _cmd_list->ResourceBarrier(1, &barrier);
        _cmd_list->ClearRenderTargetView(_rtv_handle[_back_buffer],
                                         DirectX::Colors::Aqua, 0, nullptr);
        _cmd_list->ClearDepthStencilView(_dsv_handle, D3D12_CLEAR_FLAG_DEPTH |
                                                      D3D12_CLEAR_FLAG_STENCIL,
                                         1.f, 0, 0, nullptr);
        _cmd_list->OMSetRenderTargets(1, &_rtv_handle[_back_buffer], FALSE,
                                      &_dsv_handle);

        D3D12_VERTEX_BUFFER_VIEW tv[] = {_vertex_buffer_view};
        _cmd_list->IASetVertexBuffers(0, 1, tv);
        _cmd_list->IASetIndexBuffer(&_index_buffer_view);
        _cmd_list->IASetPrimitiveTopology(
                D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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

    void
    directx_12::render(directx_12::layer l,
                       const std::vector<render_info> &infos) {
        switch (l) {
            case layer::opaque:
                _cmd_list->SetPipelineState(
                        _pso_list[static_cast<uint8_t>(layer::opaque)].Get());
                for (const auto &e: infos) render(e);
                break;
            case layer::transparent:
                _cmd_list->SetPipelineState(
                        _pso_list[static_cast<uint8_t>(layer::transparent)].Get());
                for (const auto &e: infos) render(e);
                break;
            case layer::mirror:
                _cmd_list->OMSetStencilRef(1);
                _cmd_list->SetPipelineState(
                        _pso_list[static_cast<uint8_t>(layer::mirror)].Get());
                for (const auto &e: infos) render(e);
                _cmd_list->OMSetStencilRef(0);
                break;
            case layer::reflection:
                _cmd_list->OMSetStencilRef(1);
                _cmd_list->SetPipelineState(
                        _pso_list[static_cast<uint8_t>(layer::reflection)].Get());
                for (const auto &e: infos) render(e);
                _cmd_list->OMSetStencilRef(0);
                break;
            case layer::end:
                //do nothing;
                break;
        }

    }

    void directx_12::render(const std::vector<render_info> &infos) {
        std::vector<size_t> mirrors;
        std::vector<size_t> trans;
        std::vector<size_t> reflects;

        global g = {};
        auto index = 0;
        for (const auto &e: infos) {
            if (e.is_mirror) {
                g.reflection_matrix[index]
                        = Matrix::CreateReflection(e.mirror_plane);
                ++index;
            }
        }
        g.reflection_count = index;
        update_const_buffer(_global_buffer, &g, 0);

        _cmd_list->SetPipelineState(
                _pso_list[static_cast<uint8_t>(layer::opaque)].Get());
        for (auto i = 0; i < infos.size(); ++i) {
            if (infos[i].do_reflect) {
                reflects.push_back(i);
            }

            if (infos[i].is_mirror) {
                mirrors.push_back(i);
                trans.push_back(i);
            } else if (infos[i].is_transparent) {
                trans.push_back(i);
            } else {
                render(infos[i]);
            }
        }

        if (mirrors.size() > 0 && reflects.size() > 0) {
            _cmd_list->OMSetStencilRef(1);
            _cmd_list->SetPipelineState(
                    _pso_list[static_cast<uint8_t>(layer::mirror)].Get());

            for (auto i: mirrors) {
                render(infos[i]);
            }

            _cmd_list->SetPipelineState(
                    _pso_list[static_cast<uint8_t>(layer::reflection)].Get());

            for (auto j: reflects) {
                render(infos[j]);
            }

            _cmd_list->OMSetStencilRef(0);
        }

        if (trans.size() > 0) {
            _cmd_list->SetPipelineState(
                    _pso_list[static_cast<uint8_t>(layer::transparent)].Get());
            for (auto i: trans) render(infos[i]);
        }
    }

    void directx_12::render(const render_info &info) {
        auto handle = _res_desc_heap->GetGPUDescriptorHandleForHeapStart();
        handle.ptr += group_size() * info.object_index;
        _cmd_list->SetGraphicsRootDescriptorTable(3, handle);

        _cmd_list->DrawIndexedInstanced(info.index_count, 1, info.index_offset,
                                        info.vertex_offset, 0);
    }

    void directx_12::init_base(const window_info &info) {
        CreateDXGIFactory1(IID_PPV_ARGS(&_factory));
        D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0,
                          IID_PPV_ARGS(&_device));
        _view_port = {0, 0, static_cast<float>(info.width),
                      static_cast<float>(info.height), 0, 1};
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
        D3D12_DESCRIPTOR_HEAP_DESC dh_desc = {};
        dh_desc.NumDescriptors = 1;
        dh_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dh_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        dh_desc.NodeMask = 0;

        ThrowIfFailed(_device->CreateDescriptorHeap(&dh_desc, IID_PPV_ARGS(
                &_dsv_desc_heap)));
        _dsv_handle = _dsv_desc_heap->GetCPUDescriptorHandleForHeapStart();

        auto r_desc = CD3DX12_RESOURCE_DESC::Tex2D(DSV_FORMAT,
                                                   info.width, info.height);
        r_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto clear_value = CD3DX12_CLEAR_VALUE(DSV_FORMAT, 1.f, 0);

        ThrowIfFailed(_device->CreateCommittedResource(&heap_prop,
                                                       D3D12_HEAP_FLAG_NONE,
                                                       &r_desc,
                                                       D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                                       &clear_value,
                                                       IID_PPV_ARGS(
                                                               &_dsv_buffer)));


        _device->CreateDepthStencilView(_dsv_buffer.Get(), nullptr,
                                        _dsv_handle);
    }

    void directx_12::init_global_buf() {
        _global_buffer = create_const_buffer<global>(1, _device);
    }

    void directx_12::init_camera_buf() {
        _vp_buffer = create_const_buffer<camera>(1, _device);
    }

    void directx_12::init_light_buf() {
        _light_buffer = create_const_buffer<light_info>(1, _device);
    }

    void directx_12::init_resources() {
        _obj_const_buffer = create_const_buffer<object_constant>(OBJ_CNT,
                                                                 _device);

        D3D12_DESCRIPTOR_HEAP_DESC h_desc = {};
        h_desc.NodeMask = 0;
        h_desc.NumDescriptors = OBJ_CNT * TABLE_SIZE;
        h_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        h_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        _device->CreateDescriptorHeap(&h_desc, IID_PPV_ARGS(&_res_desc_heap));
        auto w_addr = _obj_const_buffer->GetGPUVirtualAddress();

        for (auto i = 0; i < OBJ_CNT; ++i) {
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
            cbv_desc.SizeInBytes = size_of_256<object_constant>();
            cbv_desc.BufferLocation =
                    w_addr + size_of_256<object_constant>() * i;
            auto handle = _res_desc_heap->GetCPUDescriptorHandleForHeapStart();
            handle.ptr += group_size() * i;
            _device->CreateConstantBufferView(&cbv_desc, handle);
        }
    }

    void directx_12::init_root_signature() {
        CD3DX12_DESCRIPTOR_RANGE ranges[] = {
                CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 3),
                CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0)
        };
        CD3DX12_ROOT_PARAMETER param[4];
        param[0].InitAsConstantBufferView(static_cast<uint32_t>(0));//global
        param[1].InitAsConstantBufferView(static_cast<uint32_t>(1));//camera
        param[2].InitAsConstantBufferView(static_cast<uint32_t>(2));//lights
        param[3].InitAsDescriptorTable(_countof(ranges), ranges);//object const

        auto sampler_arr = sampler::samplers();

        auto rs_desc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param, 9,
                                                   sampler_arr.data());
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

        _pso_list.resize(static_cast<uint8_t>(layer::end));

        auto vs_data = DX::ReadData(L"shader\\vs.cso");
        auto vs_ref_data = DX::ReadData(L"shader\\vs_ref.cso");
        auto ps_data = DX::ReadData(L"shader\\ps.cso");

        D3D12_INPUT_ELEMENT_DESC ie_desc[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC ps_desc = {};
        ps_desc.InputLayout = {ie_desc, _countof(ie_desc)};
        ps_desc.pRootSignature = _signature.Get();
        ps_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        ps_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        ps_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        ps_desc.DSVFormat = DSV_FORMAT;
        ps_desc.SampleMask = UINT_MAX;
        ps_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        ps_desc.NumRenderTargets = 1;
        ps_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        ps_desc.SampleDesc.Count = 1;
        ps_desc.SampleDesc.Quality = 0;
        ps_desc.VS = {vs_data.data(), vs_data.size()};
        ps_desc.PS = {ps_data.data(), ps_data.size()};

        ThrowIfFailed(_device->CreateGraphicsPipelineState
                (&ps_desc, IID_PPV_ARGS(
                        &_pso_list[static_cast<uint8_t>(layer::opaque)])));


        auto trans_pso = ps_desc;

        D3D12_RENDER_TARGET_BLEND_DESC transparent_blend_desc;
        transparent_blend_desc.BlendEnable = true;
        transparent_blend_desc.LogicOpEnable = false;
        transparent_blend_desc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        transparent_blend_desc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        transparent_blend_desc.BlendOp = D3D12_BLEND_OP_ADD;
        transparent_blend_desc.SrcBlendAlpha = D3D12_BLEND_ONE;
        transparent_blend_desc.DestBlendAlpha = D3D12_BLEND_ZERO;
        transparent_blend_desc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        transparent_blend_desc.LogicOp = D3D12_LOGIC_OP_NOOP;
        transparent_blend_desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

        trans_pso.BlendState.RenderTarget[0] = transparent_blend_desc;
        trans_pso.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

        ThrowIfFailed(_device->CreateGraphicsPipelineState
                (&trans_pso, IID_PPV_ARGS(
                        &_pso_list[static_cast<uint8_t>(layer::transparent)])));


        auto mirror_pso = ps_desc;
        mirror_pso.BlendState.RenderTarget[0].RenderTargetWriteMask = 0;
        D3D12_DEPTH_STENCIL_DESC ds_desc = {};
        ds_desc.DepthEnable = true;
        ds_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        ds_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        ds_desc.StencilEnable = true;
        ds_desc.StencilReadMask = 0xff;
        ds_desc.StencilWriteMask = 0xff;

        ds_desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        ds_desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
        ds_desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        ds_desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

        ds_desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        ds_desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
        ds_desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        ds_desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

        mirror_pso.DepthStencilState = ds_desc;
        ThrowIfFailed(_device->CreateGraphicsPipelineState
                (&mirror_pso, IID_PPV_ARGS(
                        &_pso_list[static_cast<uint8_t>(layer::mirror)])));


        auto ref_pso = ps_desc;
        D3D12_DEPTH_STENCIL_DESC ref_dss = {};
        ref_dss.DepthEnable = true;
        ref_dss.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        ref_dss.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        ref_dss.StencilEnable = true;
        ref_dss.StencilReadMask = 0xff;
        ref_dss.StencilWriteMask = 0xff;

        ref_dss.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
        ref_dss.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
        ref_dss.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        ref_dss.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

        ref_dss.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        ref_dss.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
        ref_dss.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        ref_dss.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

        ref_pso.DepthStencilState = ref_dss;
        ref_pso.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
        ref_pso.RasterizerState.FrontCounterClockwise = true;
        ref_pso.VS = {vs_ref_data.data(), vs_ref_data.size()};
        ThrowIfFailed(_device->CreateGraphicsPipelineState
                (&ref_pso, IID_PPV_ARGS(
                        &_pso_list[static_cast<uint8_t>(layer::reflection)])));
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

    UINT directx_12::group_size() {
        return _device->GetDescriptorHandleIncrementSize(
                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * TABLE_SIZE;
    }
}