#include "dx_util.h"

namespace fuse::directx {
    Microsoft::WRL::ComPtr<ID3D12Resource>
    create_upload_buffer(int elem_cnt, UINT64 elem_size,
                         Microsoft::WRL::ComPtr<ID3D12Device> device) {
        auto sz = elem_size * elem_cnt;
        Microsoft::WRL::ComPtr<ID3D12Resource> ret;
        auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto res_desc = CD3DX12_RESOURCE_DESC::Buffer(sz);
        device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE,
                                        &res_desc,
                                        D3D12_RESOURCE_STATE_GENERIC_READ,
                                        nullptr, IID_PPV_ARGS(&ret));
        return ret;
    }
}