struct LightColor
{
    float4      diffuse;
    float4      ambient;
    float4      specular;
};

struct LightInfo
{
    LightColor  color;
    float4	    position;
    float4	    direction; 
    int		    lightType;
    float	    range;
    float	    angle;
    int  	    padding;
};

cbuffer GLOBAL_PARAMS : register(b0)
{
    int         g_lightCount;
    float3      g_lightPadding;
    LightInfo   g_light[50];
}

cbuffer TRANSFORM_PARAMS : register(b1)
{
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProjection;
    row_major matrix g_matWV;
    row_major matrix g_matWVP;
};

cbuffer MATERIAL_PARAMS : register(b2)
{
    int     g_int_0;
    int     g_int_1;
    int     g_int_2;
    int     g_int_3;
    int     g_int_4;
    float   g_float_0;
    float   g_float_1;
    float   g_float_2;
    float   g_float_3;
    float   g_float_4;
};

Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);

SamplerState g_sam_0 : register(s0);

LightColor CalculateLightColor(int lightIndex, float3 viewNormal, float3 viewPos)
{
    LightColor color = (LightColor)0.f;

    float3 viewLightDir = (float3)0.f;

    float diffuseRatio = 0.f;
    float specularRatio = 0.f;
    float distanceRatio = 1.f;

    if (g_light[lightIndex].lightType == 0)
    {
        // Directional Light
        viewLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));
    }
    else if (g_light[lightIndex].lightType == 1)
    {
        // Point Light
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
        viewLightDir = normalize(viewPos - viewLightPos);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        float dist = distance(viewPos, viewLightPos);
        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
            distanceRatio = saturate(1.f - pow(dist / g_light[lightIndex].range, 2));
    }
    else
    {
        // Spot Light
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
        viewLightDir = normalize(viewPos - viewLightPos);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
        {
            float halfAngle = g_light[lightIndex].angle / 2;

            float3 viewLightVec = viewPos - viewLightPos;
            float3 viewCenterLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);

            float centerDist = dot(viewLightVec, viewCenterLightDir);

            float lightAngle = acos(dot(normalize(viewLightVec), viewCenterLightDir));

            if (centerDist < 0.f || centerDist > g_light[lightIndex].range) // 최대 거리를 벗어났는지
                distanceRatio = 0.f;
            else if (lightAngle > halfAngle) // 최대 시야각을 벗어났는지
                distanceRatio = 0.f;
            else // 거리에 따라 적절히 세기를 조절
                distanceRatio = saturate(1.f - pow(centerDist / g_light[lightIndex].range, 2));
        }
    }

    float3 reflectionDir = normalize(viewLightDir + 2 * (saturate(dot(-viewLightDir, viewNormal)) * viewNormal));
    float3 eyeDir = normalize(viewPos);
    specularRatio = saturate(dot(-eyeDir, reflectionDir));
    specularRatio = pow(specularRatio, 2);

    color.diffuse = g_light[lightIndex].color.diffuse * diffuseRatio * distanceRatio;
    color.ambient = g_light[lightIndex].color.ambient * distanceRatio;
    color.specular = g_light[lightIndex].color.specular * specularRatio * distanceRatio;

    return color;
}

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
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    //float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    float4 color = float4(1.f, 1.f, 1.f, 1.f);

    LightColor totalColor = (LightColor)0.f;

    for (int i = 0; i < g_lightCount; ++i)
    {
         LightColor color = CalculateLightColor(i, input.viewNormal, input.viewPos);
         totalColor.diffuse += color.diffuse;
         totalColor.ambient += color.ambient;
         totalColor.specular += color.specular;
    }

    color.xyz = (totalColor.diffuse.xyz * color.xyz)
        + totalColor.ambient.xyz * color.xyz
        + totalColor.specular.xyz;

     return color;
}