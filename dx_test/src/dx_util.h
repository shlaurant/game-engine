#pragma once

namespace fuse::directx {
    template<typename T>
    Microsoft::WRL::ComPtr<ID3D12Resource> create_upload_buffer(int elem_cnt,
                                                                Microsoft::WRL::ComPtr<ID3D12Device> device) {
        auto sz = size_of_256<T>() * elem_cnt;
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
    void
    update_upload_buffer(Microsoft::WRL::ComPtr<ID3D12Resource> buffer, T *data,
                         int elem_cnt) {
        auto sz = size_of_256<T>() * elem_cnt;
        BYTE *mapped_data;
        buffer->Map(0, nullptr, reinterpret_cast<void **>(&mapped_data));
        memcpy(mapped_data, data, sz);
        buffer->Unmap(0, nullptr);
        mapped_data = nullptr;
    }

    template<typename T>
    UINT size_of_256() { return (sizeof(T) + 255) & ~255; }
}