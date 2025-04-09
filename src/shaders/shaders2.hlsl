/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

cbuffer SceneConstantBuffer : register(b0)
{
    float4 offset;
    float4 padding[15];
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float4 uv : TEXCOORD)
{
    PSInput result;

    result.position = position + offset;
    result.uv = uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    //return float4(input.uv.x, input.uv.y, 1.0, 1.0);
    return g_texture.Sample(g_sampler, input.uv);
}
