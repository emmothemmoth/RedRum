#include "Include/DefaultMaterialInclude.hlsli"
#include "Include/GBufferInclude.hlsli"
#include "ConstantBuffers/MaterialBuffer.hlsli"
#include "Include/TextureInclude.hlsli"
#include "Include/SamplerInclude.hlsli"


float4 SampleAlbedo(DefaultMaterial_VStoPS pixel)
{
    const float2 scaledUV = pixel.UV0 * M_UVTiling;
    
    const float4 albedoMap = albedoTexture.Sample(defaultSampler, scaledUV);
    
    return albedoMap;
    
    //float averageVertexColor = pixel.VertexColor.r + pixel.VertexColor.g + pixel.VertexColor.b;
    //averageVertexColor /= 3;
    //float3 albedoColor = M_AlbedoColor.rgb;
    //float4 albedoTint = float4(averageVertexColor * albedoColor, 1);
    //float4 albedoTex = albedoTexture.Sample(defaultSampler, pixel.UV0);
    //if (albedoTex.a < 0.05f)
    //{
    //    discard;
    //}
    //return albedoTex;
}

float4 CalculateNormals(DefaultMaterial_VStoPS input)
{
    const float3x3 TBN = float3x3
    (
    normalize(input.Tangent),
    normalize(input.BiNormal),
    normalize(input.Normal)
    );
    
    const float2 scaledUV = input.UV0 * M_UVTiling;
    
    //Sample pixelNormal from normalMap
    const float2 normalSample = normalTexture.Sample(defaultSampler, scaledUV).rg;
    float3 pixelNormal = float3(normalSample, 0);
    pixelNormal = 2.0f * pixelNormal - 1.0f;
    pixelNormal.z = sqrt(1 - saturate(dot(pixelNormal.xy, pixelNormal.xy)));
    pixelNormal = normalize(pixelNormal);
    pixelNormal.xy *= M_NormalStrength;
    pixelNormal = normalize(mul(pixelNormal, TBN));

    return float4(pixelNormal, 1);
    
}

float4 SampleMaterials(DefaultMaterial_VStoPS pixel)
{
    return float4(materialTexture.Sample(defaultSampler, pixel.UV0).rgb, 1);
}

float4 SampleFX(DefaultMaterial_VStoPS pixel)
{
    return float4(fxTexture.Sample(defaultSampler, pixel.UV0));
}

GBufferOutput main(DefaultMaterial_VStoPS pixel)
{
    GBufferOutput GBuffer;
    GBuffer.Albedo = SampleAlbedo(pixel);
    GBuffer.WorldNormal = CalculateNormals(pixel);
    GBuffer.Material = SampleMaterials(pixel);
    GBuffer.WorldPosition = float4(pixel.WorldPosition);
    GBuffer.FX = SampleFX(pixel);
    GBuffer.FX.a = M_EmissionStrength;
    return GBuffer;
}