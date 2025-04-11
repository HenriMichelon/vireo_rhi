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

ConstantBuffer<UBO1> ubo1 : register(b0);
ConstantBuffer<UBO2> ubo2 : register(b1);
[[vk::binding(2)]]
Texture2D texture : register(t0);
[[vk::binding(3)]]
SamplerState samplerNearest : register(s0);

struct VSInput {
    [[vk::location(0)]] float4 position : POSITION;
    [[vk::location(1)]] float2 uv : TEXCOORD;
    [[vk::location(2)]] float3 color : COLOR;
};

struct PSInput {
    [[vk::location(0)]] float4 position : SV_POSITION;
    [[vk::location(1)]] float2 uv : TEXCOORD;
    [[vk::location(2)]] float3 color : COLOR;
};

PSInput VSMain(VSInput input) {
    PSInput result;

    result.position = input.position + ubo1.offset;
    result.uv = input.uv;
    result.color = input.color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET {
    return texture.Sample(samplerNearest, input.uv) + float4(ubo2.color + input.color, 1.0f);
}