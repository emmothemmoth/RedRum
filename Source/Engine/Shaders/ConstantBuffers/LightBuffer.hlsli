cbuffer LightBuffer : register(b4)
{
    struct DirLightData
    {
        float3 Color;
        float Intensity;
        float4 LightPos;
        float4 LightDir;
        float4x4 LightViewInv;
        float4x4 LightProj;
        bool Active;
        float3 Padding;
    } DirLight;
    struct PointLightData
    {
        float3 Color;
        float Intensity;
        float3 Lightpos;
        bool CastShadows;
        float4x4 LightViewInv;
        float4x4 LightProj;
        bool Active;
        float Range;
        float2 Padding;
    } PointLights[4];
    
    struct SpotLightData
    {
        float3 Color;
        float Intensity;
        float3 Lightpos;
        bool CastShadows;
        float3 LightDir;
        float InnerConeAngle;
        float4x4 LightViewInv;
        float4x4 LightProj;
        bool Active;
        float Range;
        float OuterConeAngle;
        float Padding;
    } SpotLights[4];
    
    int PointLightCount;
    int SpotLightCount;
    float2 Padding;
};