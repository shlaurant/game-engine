#pragma once

#include "d3dx12.h"

namespace fuse::directx {
    class sampler {
    public:
        static const CD3DX12_STATIC_SAMPLER_DESC POINT_WRAP;
        static const CD3DX12_STATIC_SAMPLER_DESC POINT_CLAMP;
        static const CD3DX12_STATIC_SAMPLER_DESC POINT_MIRROR;

        static const CD3DX12_STATIC_SAMPLER_DESC LINEAR_WRAP;
        static const CD3DX12_STATIC_SAMPLER_DESC LINEAR_CLAMP;
        static const CD3DX12_STATIC_SAMPLER_DESC LINEAR_MIRROR;

        static const CD3DX12_STATIC_SAMPLER_DESC ANISOTROPIC_WRAP;
        static const CD3DX12_STATIC_SAMPLER_DESC ANISOTROPIC_CLAMP;
        static const CD3DX12_STATIC_SAMPLER_DESC ANISOTROPIC_MIRROR;

        static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 9> samplers();
    };

    class pipeline_state {
    public:
        static const DXGI_FORMAT DSV_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;
        static const DXGI_FORMAT RTV_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

        static D3D12_GRAPHICS_PIPELINE_STATE_DESC
        default_desc(D3D12_INPUT_ELEMENT_DESC *ed, UINT ed_cnt,
                     ID3D12RootSignature *rs,
                     const std::vector<uint8_t> &vs,
                     const std::vector<uint8_t> &ps);

        static D3D12_GRAPHICS_PIPELINE_STATE_DESC
        billboard_desc(D3D12_INPUT_ELEMENT_DESC *ed, UINT ed_cnt,
                       ID3D12RootSignature *rs,
                       const std::vector<uint8_t> &vs,
                       const std::vector<uint8_t> &ps,
                       const std::vector<uint8_t> &gs);

        static D3D12_GRAPHICS_PIPELINE_STATE_DESC
        transparent_desc(D3D12_INPUT_ELEMENT_DESC *ed, UINT ed_cnt,
                         ID3D12RootSignature *rs,
                         const std::vector<uint8_t> &vs,
                         const std::vector<uint8_t> &ps);

        static D3D12_GRAPHICS_PIPELINE_STATE_DESC
        mirror_desc(D3D12_INPUT_ELEMENT_DESC *ed, UINT ed_cnt,
                    ID3D12RootSignature *rs,
                    const std::vector<uint8_t> &vs,
                    const std::vector<uint8_t> &ps);

        static D3D12_GRAPHICS_PIPELINE_STATE_DESC
        reflection_desc(D3D12_INPUT_ELEMENT_DESC *ed, UINT ed_cnt,
                        ID3D12RootSignature *rs,
                        const std::vector<uint8_t> &vs,
                        const std::vector<uint8_t> &ps);

        static D3D12_GRAPHICS_PIPELINE_STATE_DESC
        shadow_desc(D3D12_INPUT_ELEMENT_DESC *ed, UINT ed_cnt,
                        ID3D12RootSignature *rs,
                        const std::vector<uint8_t> &vs,
                        const std::vector<uint8_t> &ps);

        static D3D12_COMPUTE_PIPELINE_STATE_DESC blur_desc(ID3D12RootSignature *rs, const std::vector<uint8_t> &cs);
    };
}