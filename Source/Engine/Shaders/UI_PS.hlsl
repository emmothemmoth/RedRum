#include "Include/TextureInclude.hlsli"
#include "Include/SamplerInclude.hlsli"

#include "Include/DefaultMaterialInclude.hlsli"


float4 main(UI_VS_to_PS input) : SV_TARGET
{
    const float4 color = albedoTexture.Sample(defaultSampler, input.uv);
    float3 color2 = pow(abs(color.xyz), 1.0 / 2.2);
    return float4(color2.xyz, color.a) * input.tint;
}