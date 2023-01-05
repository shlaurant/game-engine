#include "light.hlsl"
#include "register.hlsl"

Texture2D tex : register(t0);

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 pos_w : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;

#ifdef REFLECTION
    float4x4 world = mul(w, reflection_matrix[0]);
#elif SHADOW
    float4x4 world = mul(w, shadow_matrix);
#else
    float4x4 world = w;
#endif

    float4x4 wvp = mul(world, vp);
    output.pos = mul(float4(input.pos, 1.0f), wvp);
    output.pos_w = output.pos;
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3)world);

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = tex.Sample(sam_aw, input.uv);
    clip(color.w * mat.diffuse_albedo.w - 0.1f);

#ifdef SHADOW
    color = float4(0.f, 0.f, 0.f, 0.5f);
#else
    input.normal = normalize(input.normal);

    float3 to_eye = normalize(camera_pos - input.pos_w.xyz);
    float4 light_color = calc_light(lights, active_light_counts, mat, input.pos_w.xyz, input.normal, to_eye);
    color *= light_color;
    color.w *= mat.diffuse_albedo.w;
#endif

    return color;
}