/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

struct UBO1{
    float4 offset;
    float scale;
};
struct UBO2{
    float3 color;
};
struct ubos {
    UBO1 ubo1;
    UBO2 ubo2;
};

[[vk::binding(0, 1)]]
ConstantBuffer<ubos> global : register(b0, space0);

struct VSInput {
    [[vk::location(0)]] float4 position : POSITION;
    [[vk::location(1)]] float4 color : COLOR;
};

struct PSInput {
    [[vk::location(0)]] float4 position : SV_POSITION;
    [[vk::location(1)]] float4 color : COLOR;
};

PSInput VSMain(VSInput input) {
    PSInput result;

    result.position = input.position + global.ubo1.offset;
    result.color = input.color + float4(global.ubo2.color, 1.0f);

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET {
    return input.color;
}