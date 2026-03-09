#include"Include\DefaultMaterialInclude.hlsli"
#include "Include\TextureInclude.hlsli"
#include "Include\SamplerInclude.hlsli"

DefaultMaterial_Result main(DefaultMaterial_VStoPS input)
{
    DefaultMaterial_Result result;

    //Sample albedoTexture
    result.Color = albedoTexture.Sample(defaultSampler, input.UV0);
    
    return result;
}
