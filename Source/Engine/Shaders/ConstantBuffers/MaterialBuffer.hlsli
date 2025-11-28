cbuffer MaterialBuffer : register(b3)
{
    float4 M_AlbedoColor;
    float2 M_UVTiling;
    float M_NormalStrength;
    float M_Shininess;
    float M_EmissionStrength;
    float3 M_Padding;
};