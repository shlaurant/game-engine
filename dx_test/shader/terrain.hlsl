#include "light.hlsl"
#include "register.hlsl"

#define DIST_MAX 50.f
#define DIST_MIN 5.f
#define TESS_MAX 3.f
#define TESS_MIN 1.f
#define DELTA_FOR_NORMAL 0.01f
#define HEIGHT_CONST 80.f

Texture2D tex_diffuse : register(t0);
Texture2D tex_height : register(t1);

struct VS_IN {
    float3 pos:POSITION;
    float2 uv:TEXCOORD;
    float3 normal:NORMAL;
};

struct VS_OUT {
    float4 pos:SV_Position;
    float2 uv:TEXCOORD;
};

VS_OUT VS(VS_IN input) {
    VS_OUT output;
    output.pos = float4(input.pos, 1.f);
    output.uv = input.uv;

    return output;
}

struct CHS_OUT {
    float edge[3] : SV_TessFactor;
    float inside[1]: SV_InsideTessFactor;
};

float get_tess(float4 pos)
{
    float dist = length(pos.xyz - camera_pos);
    float factor = saturate((dist - DIST_MIN) / (DIST_MAX - DIST_MIN));
    return lerp(TESS_MAX, TESS_MIN, factor);
}

CHS_OUT CHS(InputPatch<VS_OUT, 3> patch, uint patch_id : SV_PrimitiveID) {
    CHS_OUT output;
    float4 center = (patch[0].pos + patch[1].pos + patch[2].pos) / 3.f;
    center = mul(center, w);

    float4 ecs[] = {(patch[1].pos + patch[2].pos) / 2.f, (patch[0].pos + patch[2].pos) / 2.f, (patch[1].pos + patch[0].pos) / 2.f};

    [unroll]
    for(int i = 0; i < 3; ++i){
        output.edge[i] = get_tess(length(mul(ecs[i], w) - float4(camera_pos, 1.f)));
    }

    output.inside[0] = get_tess(length(center.xyz - camera_pos));

    return output;
}


struct CPHS_OUT {
    float4 pos:SV_Position;
    float2 uv:TEXCOORD;
};

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CHS")]
CPHS_OUT CPHS(InputPatch<VS_OUT, 3> input, uint i : SV_OutputControlPointID, uint patch_id : SV_PrimitiveID) {
    CPHS_OUT output;
    output.pos = input[i].pos;
    output.uv = input[i].uv;
    return output;
}


struct DS_OUT {
    float4 pos:SV_Position;
    float4 pos_w:POSITION;
    float2 uv:TEXCOORD;
    float3 normal:NORMAL;
};

float4 pos(const OutputPatch<CPHS_OUT, 3> input, float r, float s, float t)
{
    return input[0].pos * r + input[1].pos * s + input[2].pos * t;
}

float2 get_uv(const OutputPatch<CPHS_OUT, 3> input, float r, float s, float t)
{
    return input[0].uv * r + input[1].uv * s + input[2].uv * t;
}

float height(const OutputPatch<CPHS_OUT, 3> input, float r, float s, float t)
{
    float2 uv = get_uv(input, r, s, t);
    return tex_height.SampleLevel(sam_lw, uv,0).x * HEIGHT_CONST;
}

[domain("tri")]
DS_OUT DS(CHS_OUT patch, float3 location : SV_DomainLocation, const OutputPatch<CPHS_OUT, 3> input){
    DS_OUT output;

    float4 position = input[0].pos * location.x + input[1].pos * location.y + input[2].pos * location.z;
    float2 uv = input[0].uv * location.x + input[1].uv * location.y + input[2].uv * location.z;

    position.y = height(input, location.x, location.y, location.z);

    // float4 delta0 = pos(input, location.x + DELTA_FOR_NORMAL, location.y - DELTA_FOR_NORMAL, location.z);
    // delta0 = mul(delta0, w);
    // delta0.y = height(input, location.x + DELTA_FOR_NORMAL, location.y - DELTA_FOR_NORMAL, location.z);
    // float4 delta1 = pos(input, location.x - DELTA_FOR_NORMAL, location.y + DELTA_FOR_NORMAL, location.z);
    // delta1 = mul(delta1, w);
    // delta1.y = height(input, location.x - DELTA_FOR_NORMAL, location.y + DELTA_FOR_NORMAL, location.z);
    // float3 v0 = (float3)(delta1 - delta0);
    //
    // float4 delta2 = pos(input, location.x + DELTA_FOR_NORMAL, location.y, location.z - DELTA_FOR_NORMAL);
    // delta2 = mul(delta2, w);
    // delta2.y = height(input, location.x + DELTA_FOR_NORMAL, location.y, location.z - DELTA_FOR_NORMAL);
    // float4 delta3 = pos(input, location.x - DELTA_FOR_NORMAL, location.y, location.z + DELTA_FOR_NORMAL);
    // delta3 = mul(delta3, w);
    // delta3.y = height(input, location.x - DELTA_FOR_NORMAL, location.y, location.z + DELTA_FOR_NORMAL);
    // float3 v1 = (float3)(delta3 - delta2);
    float3 v1 = float3(0.f, 0.f, DELTA_FOR_NORMAL * 2);
    v1.y = tex_height.SampleLevel(sam_lw, uv - float2(0.f,DELTA_FOR_NORMAL), 0).x - tex_height.SampleLevel(sam_lw, uv + float2(0.f,DELTA_FOR_NORMAL), 0).x;

    float3 v2 = float3(DELTA_FOR_NORMAL * 2, 0.f, 0.f);
    v2.y = tex_height.SampleLevel(sam_lw, uv + float2(DELTA_FOR_NORMAL, 0.f), 0).x - tex_height.SampleLevel(sam_lw, uv - float2(DELTA_FOR_NORMAL, 0.f), 0).x;
    
    float3 normal = cross(v1, v2);
    normal = normalize(normal);

    float4x4 wvp = mul(w, vp);
    output.pos = mul(position, wvp);
    output.pos_w = output.pos;
    output.uv = uv;
    output.normal = normal;
    
    return output;
}

float4 PS(DS_OUT input) :SV_Target
{
    float4 diffuse_albedo = tex_diffuse.Sample(sam_aw, input.uv) * mat.diffuse_albedo;
    input.normal = normalize(input.normal);
    
    material new_mat = {diffuse_albedo, mat.fresnel_r0, mat.roughness};
    float3 to_eye = normalize(camera_pos - input.pos_w.xyz);
    float4 light_color = calc_light(lights, active_light_counts, new_mat, input.pos_w.xyz, input.normal, to_eye);
    float4 color = light_color;
    color.w = diffuse_albedo.w;

    return color;
}