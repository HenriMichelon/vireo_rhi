/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
struct VertexInput {
    float4 position : POSITION;
    float2 uv       : TEXCOORD;
};

struct VertexOutput {
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};


VertexOutput vertexMain(VertexInput input) {
    VertexOutput output;
    output.position = input.position;
    output.uv = input.uv;
    return output;
}

float4 fragmentMain(VertexOutput input) : SV_TARGET {
    return float4(input.uv.x, input.uv.y, 1.0, 1.0);
}
