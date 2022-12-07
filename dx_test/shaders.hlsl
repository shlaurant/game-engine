cbuffer camera_vp :register(b0) {
    row_major float4x4 vp;
};

cbuffer object_w :register(b1) {
    row_major float4x4 w;
};

Texture2D tex : register(t0);

SamplerState sam : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;

    float4x4 wvp = mul(w, vp);
    output.pos = mul(float4(input.pos, 1.0f), wvp);
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = tex.Sample(sam, input.uv);
     return color;
}