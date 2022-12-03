#pragma once

namespace fuse::directx {
    template<typename T>
    Microsoft::WRL::ComPtr<ID3D12Resource> create_upload_buffer(int elem_cnt,
                                                                Microsoft::WRL::ComPtr<ID3D12Device> device) {
        auto sz = ((sizeof(T) + 255) & ~255) * elem_cnt;
        Microsoft::WRL::ComPtr<ID3D12Resource> ret;
        auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto res_desc = CD3DX12_RESOURCE_DESC::Buffer(sz);
        device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE,
                                        &res_desc,
                                        D3D12_RESOURCE_STATE_GENERIC_READ,
                                        nullptr, IID_PPV_ARGS(&ret));
        return ret;
    }

    template<typename T>
    void update_upload_buffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer, T *data, int elem_cnt) {
        auto sz = ((sizeof(T) + 255) & ~255) * elem_cnt;
        BYTE *mapped_data;
        buffer->Map(0, nullptr, reinterpret_cast<void **>(&mapped_data));
        memcpy(mapped_data, data, sz);
        buffer->Unmap(0, nullptr);
        mapped_data = nullptr;
    }
}