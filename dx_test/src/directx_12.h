#pragma once

#include "d3dx12.h"
#include "SimpleMath.h"

#include "common.h"
#include "resource.h"
#include "constants.h"

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

        enum class layer : uint8_t {
            opaque, transparent, fog, end
        };

        void init(const window_info &);
        void init_geometries(std::vector<geometry> &);
        int load_texture(const std::wstring &path);
        void bind_texture(int obj, int texture);

        void update_camera(const camera &);
        void update_lights(const light_info &);
        void update_obj_constants(const std::vector<object_constant> &);

        void render_begin();
        void render(layer, const std::vector<render_info> &);
        void render_end();

    private:
        ComPtr <IDXGIFactory> _factory;
        ComPtr <ID3D12Device> _device;
        D3D12_VIEWPORT _view_port;
        D3D12_RECT _scissors_rect;

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

        const DXGI_FORMAT DSV_FORMAT = DXGI_FORMAT_D32_FLOAT;
        ComPtr <ID3D12Resource> _dsv_buffer;
        ComPtr <ID3D12DescriptorHeap> _dsv_desc_heap;
        D3D12_CPU_DESCRIPTOR_HANDLE _dsv_handle;

        //root sig
        ComPtr <ID3D12RootSignature> _signature;
        ComPtr <ID3D12DescriptorHeap> _root_desc_table;

        //resource
        static const int TABLE_SIZE = 2;
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
        ComPtr <ID3D12Resource> _vertex_buffer;
        ComPtr <ID3D12Resource> _index_buffer;
        D3D12_VERTEX_BUFFER_VIEW _vertex_buffer_view;
        D3D12_INDEX_BUFFER_VIEW _index_buffer_view;

        void init_base(const window_info &info);
        void init_cmds();
        void init_swap_chain(const window_info &info);
        void init_rtv();
        void init_dsv(const window_info &);

        void init_camera_buf();
        void init_light_buf();
        void init_resources();

        void init_root_signature();
        void init_shader();

        void execute_cmd_list();
        void wait_cmd_queue_sync();

        void render(const render_info &);

        UINT group_size();
    };
}
