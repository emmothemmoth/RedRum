#include "Include/QuadInclude.hlsli"

Texture2D IntermediateLDR : register(t125);
Texture2D inputTexture : register(t20);
SamplerState DefaultSampler : register(s0);

float4 main(QuadVStoPS pixel) : SV_TARGET
{
    const float3 sceneColor = IntermediateLDR.Sample(DefaultSampler, pixel.UV).rgb;
    const float3 bloomColor = inputTexture.Sample(DefaultSampler, pixel.UV).rgb;
    float4 blend;
    float bias = 0.15f;

    blend.r = lerp(sceneColor.r, bloomColor.r, bias);
    blend.g = lerp(sceneColor.g, bloomColor.g, bias);
    blend.b = lerp(sceneColor.b, bloomColor.b, bias);
    blend.a = 1;
    return blend;
}