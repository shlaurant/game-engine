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
}