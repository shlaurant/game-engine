#pragma once

#include "d3dx12.h"
#include "SimpleMath.h"

#include "common.h"
#include "resource.h"
#include "constants.h"
#include "dx_util.h"
#include "debug.h"
#include "typeid.h"
#include "blur.h"

using namespace Microsoft::WRL;

namespace fuse::directx {
    struct window_info {
        HWND hwnd;
        int32_t width;
        int32_t height;
        bool windowed;
    };

    class directx_12 {
    public:
        const static int SWAP_CHAIN_BUFFER_COUNT = 2;
        const static int OBJ_CNT = 10;
        const static DXGI_FORMAT RTV_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

        enum class layer : uint8_t {
            opaque, transparent, mirror, reflection, shadow, billboard, blur_h, blur_v, end
        };

        void init(const window_info &);

        template<typename T>
        void init_geometries(std::vector<geometry<T>> &geometries) {
            ThrowIfFailed(_cmd_alloc->Reset());
            ThrowIfFailed(_cmd_list->Reset(_cmd_alloc.Get(), nullptr));

            ComPtr<ID3D12Resource> u_buffer_v;
            std::vector<T> vertices;
            size_t index = 0;
            for (auto &e: geometries) {
                e.vertex_offset = index;
                std::copy(e.vertices.begin(), e.vertices.end(),
                          std::back_inserter(vertices));
                index += e.vertices.size();
            }
            auto vert_byte_size = sizeof(T) * vertices.size();
            std::copy(vertices.begin(), vertices.end(), vertices.data());

            auto vb = create_default_buffer(vertices.data(), vert_byte_size,
                                            u_buffer_v, _device, _cmd_list);
            D3D12_VERTEX_BUFFER_VIEW vbv;
            vbv.BufferLocation = vb->GetGPUVirtualAddress();
            vbv.StrideInBytes = sizeof(T);
            vbv.SizeInBytes = vert_byte_size;
            _vertex_buffers[type_id<T>()] = std::make_pair(vb, vbv);

            ComPtr<ID3D12Resource> u_buffer_i;
            std::vector<uint16_t> indices;
            size_t index_i = 0;
            for (auto &e: geometries) {
                e.index_offset = index_i;
                std::copy(e.indices.begin(), e.indices.end(),
                          std::back_inserter(indices));
                index_i += e.indices.size();
            }

            auto ib = create_default_buffer(indices.data(),
                                            sizeof(uint16_t) *
                                            indices.size(),
                                            u_buffer_i, _device,
                                            _cmd_list);

            D3D12_INDEX_BUFFER_VIEW ibv;
            ibv.BufferLocation = ib->GetGPUVirtualAddress();
            ibv.Format = DXGI_FORMAT_R16_UINT;
            ibv.SizeInBytes = sizeof(uint16_t) * indices.size();

            _index_buffers[type_id<T>()] = std::make_pair(ib, ibv);

            _cmd_list->Close();
            execute_cmd_list();
            wait_cmd_queue_sync();
        }

        int load_texture(const std::wstring &path);
        void bind_texture(int obj, int texture);

        void update_camera(const camera &);
        void update_lights(const light_info &);
        void update_obj_constants(const std::vector<object_constant> &);

        void render_begin();
        void render(const std::vector<render_info> &);
        void render_end();

    private:
        ComPtr <IDXGIFactory> _factory;
        ComPtr <ID3D12Device> _device;
        D3D12_VIEWPORT _view_port;
        D3D12_RECT _scissors_rect;
        UINT msaa_sample_count = 4;
        UINT msaa_quality_levels;


        //cmd queue
        ComPtr <ID3D12CommandQueue> _cmd_queue;
        ComPtr <ID3D12CommandAllocator> _cmd_alloc;
        ComPtr <ID3D12GraphicsCommandList> _cmd_list;
        ComPtr <ID3D12Fence> _fence;
        uint32_t _fence_value = 0;

        //swap chain & rtv & dsv
        ComPtr <IDXGISwapChain> _swap_chain;
        uint32_t _back_buffer = 0;
        ComPtr <ID3D12Resource> _rtv_buffer[SWAP_CHAIN_BUFFER_COUNT];
        ComPtr <ID3D12DescriptorHeap> _rtv_heap;
        D3D12_CPU_DESCRIPTOR_HANDLE _rtv_handle[SWAP_CHAIN_BUFFER_COUNT];
        ComPtr<ID3D12Resource> _msaa_render_buffer;
        ComPtr<ID3D12DescriptorHeap> _msaa_render_buffer_heap;

        const DXGI_FORMAT DSV_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;
        ComPtr <ID3D12Resource> _dsv_buffer;
        ComPtr <ID3D12DescriptorHeap> _dsv_desc_heap;
        D3D12_CPU_DESCRIPTOR_HANDLE _dsv_handle;

        //root sig
        ComPtr <ID3D12RootSignature> _signature;
        ComPtr <ID3D12DescriptorHeap> _root_desc_table;

        //resource
        static const int TABLE_SIZE = 2;
        global _global;
        ComPtr <ID3D12Resource> _global_buffer;//globals set automatically.
        ComPtr <ID3D12Resource> _vp_buffer;
        ComPtr <ID3D12Resource> _light_buffer;
        ComPtr <ID3D12Resource> _obj_const_buffer;
        std::vector<std::pair<D3D12_SHADER_RESOURCE_VIEW_DESC,
                ComPtr < ID3D12Resource>>>
        _texture_buffers;
        ComPtr <ID3D12DescriptorHeap> _res_desc_heap;

        //shader
        std::vector<ComPtr < ID3D12PipelineState>> _pso_list;

        //vertex & index buffer
        std::unordered_map<uint32_t, std::pair<ComPtr <
                                               ID3D12Resource>, D3D12_VERTEX_BUFFER_VIEW>> _vertex_buffers;
        std::unordered_map<uint32_t, std::pair<ComPtr <
                                               ID3D12Resource>, D3D12_INDEX_BUFFER_VIEW>> _index_buffers;

        //cs
        blur _blur;
        ComPtr<ID3D12RootSignature> _blur_rs;

        void init_base(const window_info &info);
        void init_cmds();
        void init_swap_chain(const window_info &info);
        void init_rtv(const window_info &);
        void init_dsv(const window_info &);

        void init_global_buf();
        void init_camera_buf();
        void init_light_buf();
        void init_resources();

        void init_root_signature();
        void init_shader();

        void execute_cmd_list();
        void wait_cmd_queue_sync();

        void render(const render_info &);

        UINT group_size();

        ComPtr <ID3D12Resource>
        load_texture(const std::wstring &path, DirectX::ScratchImage &image,
                     ComPtr <ID3D12Resource> &upload_buffer);
        void init_default_signature();
        void init_blur_signature();
    };
}
