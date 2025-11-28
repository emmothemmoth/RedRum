#include "Include/ParticleInclude.hlsli"
#include "Include/SamplerInclude.hlsli"
#include "Include/TextureInclude.hlsli"


float4 main(ParticleGStoPS pixel) : SV_TARGET
{
    if (pixel.LifeTime < 0.0f)
    {
        discard;
        return 0;
    }
    const float4 textureColor = ParticleTexture.Sample(defaultSampler, pixel.UV) * pixel.ChannelMask;
    const float color = textureColor.r + textureColor.g + textureColor.b + textureColor.a;
    return color * pixel.Color;
    
    //Debugging
    //return textureColor;
    //return float4(1, 0, 0, 1);
}