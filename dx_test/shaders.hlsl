cbuffer camera_vp :register(b0) {
    row_major float4x4 vp;
};

cbuffer object_w :register(b1) {
    row_major float4x4 w;
}

struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;

    output.pos = mul(float4(input.pos, 1.0f), vp);
    output.color = input.color;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
     return input.color;
}