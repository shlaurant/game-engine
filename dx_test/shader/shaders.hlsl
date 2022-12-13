#include "light.hlsl"

cbuffer camera :register(b0) {
    row_major float4x4 vp;
    float3 camera_pos;
    float camera_pad0;
};

cbuffer light_info : register(b1) {
    light lights[LIGHT_COUNT];
    int active_light_counts;
    float3 light_info_pad0;
}

cbuffer object_const :register(b2) {
    row_major float4x4 w;
    material mat;
};

Texture2D tex : register(t0);

SamplerState sam : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;

    float4x4 wvp = mul(w, vp);
    output.pos = mul(float4(input.pos, 1.0f), wvp);
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3)w);

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    input.normal = normalize(input.normal);
    float4 color = tex.Sample(sam, input.uv);

    float3 to_eye = normalize(camera_pos - input.pos.xyz);
    float4 light_color = calc_light(lights, active_light_counts, mat, input.pos.xyz, input.normal, to_eye);
    light_color = float4(light_color.rgb, mat.diffuse_albedo.w);
    color *= light_color;

    return color;
}