cbuffer wvp :register(b0) {
    float4x4 gwvp;
};

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

    output.pos = mul(float4(input.pos, 1.0f), gwvp);
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
     return input.color;
}