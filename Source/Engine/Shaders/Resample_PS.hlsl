#include "Include/QuadInclude.hlsli"

Texture2D inputTexture : register(t20);
SamplerState defaultClampSampler : register(s1);

float4 main(QuadVStoPS pixel) : SV_TARGET
{
    return inputTexture.Sample(defaultClampSampler, pixel.UV);
}