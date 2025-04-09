//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once
#include "Libraries.h"
#include "../../Tools.h"

// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
using Microsoft::WRL::ComPtr;

inline std::string HrToString(const HRESULT hr) {
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
    return std::string(s_str);
}

#define SAFE_RELEASE(p) if (p) (p)->Release()

// void PrintD3D12DebugMessages(ID3D12Device* device) {
//     ComPtr<ID3D12InfoQueue> infoQueue;
//     if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
//         const UINT64 numMessages = infoQueue->GetNumStoredMessages();
//         for (UINT64 i = 0; i < numMessages; ++i) {
//             SIZE_T messageLength = 0;
//             infoQueue->GetMessage(i, nullptr, &messageLength);
//             auto message = reinterpret_cast<D3D12_MESSAGE*>(malloc(messageLength));
//             infoQueue->GetMessage(i, message, &messageLength);
//
//             std::cerr << "D3D12 Debug: " << message->pDescription << std::endl;
//
//             free(message);
//         }
//
//         // Optional: clear the stored messages after printing
//         infoQueue->ClearStoredMessages();
//     }
// }

inline void DieIfFailed(const HRESULT hr) {
    if (FAILED(hr)) {
        die(HrToString(hr));
    }
}


// Resets all elements in a ComPtr array.
template<class T>
void ResetComPtrArray(T* comPtrArray)
{
    for (auto &i : *comPtrArray)
    {
        i.Reset();
    }
}


// Resets all elements in a unique_ptr array.
template<class T>
void ResetUniquePtrArray(T* uniquePtrArray)
{
    for (auto &i : *uniquePtrArray)
    {
        i.reset();
    }
}
