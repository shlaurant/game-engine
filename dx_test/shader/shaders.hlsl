#include "light.hlsl"

cbuffer globals :register(b0) {
    row_major float4x4 reflection_matrix[10];
    int reflection_count;
    float3 pad0;
}

cbuffer camera :register(b1) {
    row_major float4x4 vp;
    float3 camera_pos;
    float camera_pad0;
};

cbuffer light_info : register(b2) {
    light lights[LIGHT_COUNT];
    int active_light_counts;
    float3 light_info_pad0;
}

cbuffer object_const :register(b3) {
    row_major float4x4 w;
    material mat;
};

Texture2D tex : register(t0);

SamplerState sam_pw : register(s0);
SamplerState sam_pc : register(s1);
SamplerState sam_pm : register(s2);
SamplerState sam_lw : register(s3);
SamplerState sam_lc : register(s4);
SamplerState sam_lm : register(s5);
SamplerState sam_aw : register(s6);
SamplerState sam_ac : register(s7);
SamplerState sam_am : register(s8);

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

#ifdef REFLECTION
    float4x4 world = mul(w, reflection_matrix[0]);
#else
    float4x4 world = w;
#endif

    float4x4 wvp = mul(world, vp);
    output.pos = mul(float4(input.pos, 1.0f), wvp);
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3)world);

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    input.normal = normalize(input.normal);
    float4 color = tex.Sample(sam_aw, input.uv);

    clip(color.w * mat.diffuse_albedo.w - 0.1f);

    float3 to_eye = normalize(camera_pos - input.pos.xyz);
    float4 light_color = calc_light(lights, active_light_counts, mat, input.pos.xyz, input.normal, to_eye);
    color *= light_color;
    color.w *= mat.diffuse_albedo.w;

    return color;
}