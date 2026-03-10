#include "Include\BillboardInclude.hlsli"
#include "Include\TextureInclude.hlsli"
#include "Include\SamplerInclude.hlsli"

float4 main(BillboardVStoPS input) : SV_TARGET
{
    float4 color = albedoTexture.Sample(defaultSampler, input.UV);
    if (color.a < 0.1)
    {
        discard;
    }
    return color;
}