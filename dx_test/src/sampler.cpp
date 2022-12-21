#include "sampler.h"

namespace fuse::directx {
    const CD3DX12_STATIC_SAMPLER_DESC
            sampler::POINT_WRAP(0, D3D12_FILTER_MIN_MAG_MIP_POINT,
                                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                                D3D12_TEXTURE_ADDRESS_MODE_WRAP);
    const CD3DX12_STATIC_SAMPLER_DESC
            sampler::POINT_CLAMP(1, D3D12_FILTER_MIN_MAG_MIP_POINT,
                                 D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                                 D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                                 D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
    const CD3DX12_STATIC_SAMPLER_DESC
            sampler::POINT_MIRROR(2, D3D12_FILTER_MIN_MAG_MIP_POINT,
                                  D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                                  D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                                  D3D12_TEXTURE_ADDRESS_MODE_MIRROR);

    const CD3DX12_STATIC_SAMPLER_DESC
            sampler::LINEAR_WRAP(3, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                                 D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                                 D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                                 D3D12_TEXTURE_ADDRESS_MODE_WRAP);
    const CD3DX12_STATIC_SAMPLER_DESC
            sampler::LINEAR_CLAMP(4, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                                  D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                                  D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                                  D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
    const CD3DX12_STATIC_SAMPLER_DESC
            sampler::LINEAR_MIRROR(5, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                                   D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                                   D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                                   D3D12_TEXTURE_ADDRESS_MODE_MIRROR);

    const CD3DX12_STATIC_SAMPLER_DESC
            sampler::ANISOTROPIC_WRAP(6, D3D12_FILTER_ANISOTROPIC,
                                      D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                                      D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                                      D3D12_TEXTURE_ADDRESS_MODE_WRAP);
    const CD3DX12_STATIC_SAMPLER_DESC
            sampler::ANISOTROPIC_CLAMP(7, D3D12_FILTER_ANISOTROPIC,
                                       D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                                       D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                                       D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
    const CD3DX12_STATIC_SAMPLER_DESC
            sampler::ANISOTROPIC_MIRROR(8, D3D12_FILTER_ANISOTROPIC,
                                        D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                                        D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
                                        D3D12_TEXTURE_ADDRESS_MODE_MIRROR);

    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 9> sampler::samplers() {
        return {POINT_WRAP, POINT_CLAMP, POINT_MIRROR, LINEAR_WRAP,
                LINEAR_CLAMP, LINEAR_MIRROR, ANISOTROPIC_WRAP,
                ANISOTROPIC_CLAMP, ANISOTROPIC_MIRROR};
    }
}