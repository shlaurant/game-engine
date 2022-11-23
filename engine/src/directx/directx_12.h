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

using namespace Microsoft::WRL;

namespace fuse {
    struct WindowInfo {
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
        const static int REGISTER_COUNT = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT;

        void init(const WindowInfo &);
    private:
        ComPtr <IDXGIFactory> _factory;
        ComPtr <ID3D12Device> _device;

        //cmd queue
        ComPtr <ID3D12CommandQueue> _cmd_queue;
        ComPtr <ID3D12CommandAllocator> _cmd_alloc;
        ComPtr <ID3D12GraphicsCommandList> _cmd_list;
        ComPtr <ID3D12Fence> _fence;
        uint32_t _fence_value = 0;
        HANDLE _fence_event = INVALID_HANDLE_VALUE;

        //swap chain & rtv
        ComPtr <IDXGISwapChain> _swap_chain;
        ComPtr <ID3D12Resource> _rtv_buffer[SWAP_CHAIN_BUFFER_COUNT];
        ComPtr <ID3D12DescriptorHeap> _rtv_heap;
        D3D12_CPU_DESCRIPTOR_HANDLE _rtv_handle[SWAP_CHAIN_BUFFER_COUNT];

        //root sig
        ComPtr <ID3D12RootSignature> _signature;
        D3D12_STATIC_SAMPLER_DESC _sampler_desc;
        ComPtr<ID3D12DescriptorHeap> _regi_heap;


    };
}
