/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

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

    result.position = input.position;
    result.color = input.color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET {
    return input.color;
}