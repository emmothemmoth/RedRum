#include "Include/TextureInclude.hlsli"
#include "Include/SamplerInclude.hlsli"

#include "Include/DefaultMaterialInclude.hlsli"


float4 main(UI_VS_to_PS input) : SV_TARGET
{
    const float4 color = albedoTexture.Sample(defaultSampler, input.uv);
    return float4(color * input.tint);
}