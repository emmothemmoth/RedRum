#include "Include/QuadInclude.hlsli"
#include "Include/TextureInclude.hlsli"
#include "Include/SamplerInclude.hlsli"
#include "ConstantBuffers/GBuffer.hlsli"
#include "ConstantBuffers/FrameBuffer.hlsli"
#include "ConstantBuffers/PostProcessBuffer.hlsli"
#include "ConstantBuffers/DebugBuffer.hlsli"


float4 GetViewSpacePosition(float2 aUV)
{
    float4 position = GBufferWorldPos.SampleLevel(PointClampSampler, aUV, 0);
    return mul(FB_InvView, position);
}

float4 GetViewSpaceNormal(float2 aUV)
{
    float4 normal = GBufferNormal.SampleLevel(PointClampSampler, aUV, 0);
    return mul(FB_InvView, normal);
}

float2 GetNoiseScale()
{
    uint w, h, m;
    BlueNoiseTexture.GetDimensions(0, w, h, m);
    float2 noiseScale = float2(w, h);
    return FB_Resolution / noiseScale;
}

float3 GetRandomNoise(float2 aUV, float2 aNoiseScale)
{
    return BlueNoiseTexture.SampleLevel(PointWrapSampler, aUV * aNoiseScale, 0).rgb;
}

float4 main(QuadVStoPS pixel) : SV_TARGET
{
    float4 occlusion = float4(0, 0, 0, 0);
    
    const float2 noiseScale = GetNoiseScale();
    ////Debugging
    //return float4((GetViewSpacePosition(pixel.UV).rgb), 1);
    //return float4((GetViewSpaceNormal(pixel.UV).rgb + 1.0f) / 2.0f, 1);
    
    const float4 pos = GetViewSpacePosition(pixel.UV);
    const float4 normal = GetViewSpaceNormal(pixel.UV);
    const float noisePower = 0.25f;
    const float3 r = GetRandomNoise(pixel.UV, noiseScale) * noisePower;
    
    const float3 T = normalize(r - normal.xyz * saturate(dot(r, normal.xyz)));
    const float3 B = cross(normal.rgb, T);
    const float3x3 TBN = float3x3(T, B, normal.rgb);
    
    const float radius = 0.5f;
    const float bias = 0.025f;
    
    for (int index = 0; index < PostProcessData.PB_KernelSize; index++)
    {
        float3 kernelPos = mul(TBN, PostProcessData.PB_Kernel[index].xyz);
        kernelPos = pos.xyz + kernelPos * radius;
        
        float4 offset = float4(kernelPos, 1);
        offset = mul(FB_Projection, offset);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5f + 0.5f;
        offset.y = 1 - offset.y;
        
        const float stepDepth = GetViewSpacePosition(offset.xy).z;
        const float range = smoothstep(0.0, 1.0, radius / abs(pos.z - stepDepth));
        occlusion += (stepDepth >= kernelPos.z + bias ? 1.0 : 0.0) * range;
    }
    occlusion = 1.0f - (occlusion / PostProcessData.PB_KernelSize);
    
    return occlusion;

}