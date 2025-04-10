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

ConstantBuffer<ubos> global : register(b0, space0);
Texture2D texture : register(t0, space1);

[[vk::binding(1, 1)]]
SamplerState samplerNearest : register(s0, space0);

struct VSInput {
    [[vk::location(0)]] float4 position : POSITION;
    [[vk::location(1)]] float2 uv : TEXCOORD;
};

struct PSInput {
    [[vk::location(0)]] float4 position : SV_POSITION;
    [[vk::location(1)]] float2 uv : TEXCOORD;
};

PSInput VSMain(VSInput input) {
    PSInput result;

    result.position = input.position + global.ubo1.offset;
    result.uv = input.uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET {
    return texture.Sample(samplerNearest, input.uv) + float4(global.ubo2.color, 1.0f);
}