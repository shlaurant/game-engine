#pragma once

#include <windows.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

#include "d3dx12.h"
#include "SimpleMath.h"

#include "common.h"

using namespace Microsoft::WRL;

namespace fuse {
    struct window_info {
        HWND hwnd;
        int32_t width;
        int32_t height;
        bool windowed;
    };

    struct light_color {
        DirectX::SimpleMath::Vector4 diffuse;
        DirectX::SimpleMath::Vector4 ambient;
        DirectX::SimpleMath::Vector4 specular;
    };

    struct light_info {
        light_color color;
        DirectX::SimpleMath::Vector4 position;
        DirectX::SimpleMath::Vector4 direction;
        int32_t type;
        float range;
        float angle;
        int32_t padding;
    };

    struct light_param {
        uint32_t count;
        DirectX::SimpleMath::Vector3 padding;
        light_info infos[50];
    };

    class directx_12 {
    public:
        enum class CBV_REGISTER : uint8_t {
            b0, b1, b2, b3, b4, END
        };

        enum class SRV_REGISTER : uint8_t {
            t0 = static_cast<uint8_t>(CBV_REGISTER::END), t1, t2, t3, t4, END
        };

        const static int SWAP_CHAIN_BUFFER_COUNT = 2;
        const static int CBV_REGISTER_COUNT = static_cast<uint8_t>(CBV_REGISTER::END);
        const static int SRV_REGISTER_COUNT =
                static_cast<uint8_t>(SRV_REGISTER::END) - CBV_REGISTER_COUNT;
        const static int REGISTER_COUNT =
                CBV_REGISTER_COUNT + SRV_REGISTER_COUNT;

        void init(const window_info &);
        void init_geometries(std::vector<geometry> &);

        void render_begin();
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
        ComPtr<ID3D12Resource> _dsv_buffer;
        ComPtr<ID3D12DescriptorHeap> _dsv_desc_heap;
        D3D12_CPU_DESCRIPTOR_HANDLE _dsv_handle;

        //root sig
        ComPtr <ID3D12RootSignature> _signature;
        D3D12_STATIC_SAMPLER_DESC _sampler_desc;
        ComPtr <ID3D12DescriptorHeap> _root_desc_table;

        //resource buffers
        ComPtr <ID3D12Resource> _t0;
        ComPtr <ID3D12DescriptorHeap> _t0_desc_heap;

        //shader
        ComPtr <ID3DBlob> _vertex_shader;
        ComPtr <ID3DBlob> _pixel_shader;
        ComPtr <ID3D12PipelineState> _pipeline_state;

        //vertex & index buffer
        ComPtr <ID3D12Resource> _vertex_buffer;
        ComPtr <ID3D12Resource> _index_buffer;

        void init_base(const window_info &info);
        void init_cmds();
        void init_swap_chain(const window_info &info);
        void init_rtv();
        void init_dsv(const window_info &);
        void init_root_signature();
        void init_shader();

        void execute_cmd_list();
        void wait_cmd_queue_sync();

        ComPtr <ID3D12Resource>

        //Reset cmd list beforehand, and close after calling this
        create_default_buffer(void *, UINT64, ComPtr <ID3D12Resource>);
    };
}

#define TRY(x) {auto hr = x; if(FAILED(hr)) {auto line = __LINE__; \
auto message = "line " + std::to_string(line); FUSE_ERROR(message.c_str())}}
