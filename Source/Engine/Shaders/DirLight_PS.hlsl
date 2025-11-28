#include "Include/QuadInclude.hlsli"
#include "Include/LightsInclude.hlsli"
#include "ConstantBuffers/FrameBuffer.hlsli"
#include "Include/DefaultMaterialInclude.hlsli"
#include "ConstantBuffers/GBuffer.hlsli"
#include "ConstantBuffers/MaterialBuffer.hlsli"
#include "ConstantBuffers/DebugBuffer.hlsli"


float4 main(QuadVStoPS pixel) : SV_TARGET
{
    float4 result = float4(0, 0, 0, 0);
    float3 directlightRadiance = float3(0, 0, 0);
    float shadow = 0.0f;
    float3 kA = 0.0f;
 
        float3 albedo = GBufferAlbedo.Sample(defaultSampler, pixel.UV).rgb;
        float3 material = GBufferMaterial.Sample(defaultSampler, pixel.UV).rgb;
        float3 pixelNormal = GBufferNormal.Sample(defaultSampler, pixel.UV).rgb;
        float3 worldPos = GBufferWorldPos.Sample(defaultSampler, pixel.UV).rgb;
        
        
        const float occlusionMap = material.r;
        const float roughMap = material.g;
        const float metalMap = material.b;
        
        const float ssaoMap = SSAOTexture.Sample(defaultClampSampler, pixel.UV).r;
        float ao = min(occlusionMap, ssaoMap);
        
        //Perfect Specular surface color
        const float3 specularColor = lerp(float3(0.04f, 0.04, 0.04), albedo, metalMap);
        //Perfect Diffuse surface color
        const float3 diffuseColor = lerp(float3(0.00f, 0.0, 0.0), albedo, 1 - metalMap);
        
        float3 toView = normalize(FB_CameraPosition.xyz - worldPos);
        
        float3 toDirLight = normalize(DirLight.LightPos.xyz - worldPos);
        float3 halfAngle = normalize((toDirLight + toView));
        
        //Direct lighting
        float3 kS = SpecularBRDF(roughMap, pixelNormal, halfAngle, toView, toDirLight, specularColor);
        
        float3 kD = DiffuseBRDF(diffuseColor);
        
        kD *= (1.0f - kS);
        const float3 lightColorAndIntensity = DirLight.Color * DirLight.Intensity;
        
        
        float3 directlightDirectRadiance = (kD + kS) * lightColorAndIntensity * saturate(dot(pixelNormal, toDirLight));
        float dirLightAttenuation = saturate(dot(pixelNormal, normalize(-DirLight.LightDir.rgb)));
        
        //Indirect lighting
        float2 integratedBRDF = IntegrateBRDF(pixel.UV.x, pixel.UV.y);
        
        const float3 diffuseIBL = DiffuseIBL(pixelNormal);
        const float3 specularIBL = SpecularIBL(pixelNormal, toView, roughMap, specularColor);
        
        
       kA = (diffuseColor * diffuseIBL + specularIBL);
        if (DB_SSAOActive == true)
        {
            kA *= ao;
        }
        else
        {
            kA *= occlusionMap;
        }
        
        shadow = GetDirLightShadow(float4(worldPos, 1));
        
        directlightRadiance = directlightDirectRadiance;
    
    [flatten]
    if (DirLight.Active == false)
    {
        directlightRadiance = 0.0f;
    }
    float3 albedoColor = GBufferAlbedo.Sample(defaultSampler, pixel.UV).rgb;
    float4 emissionMask = GBufferFXTexture.Sample(defaultSampler, pixel.UV).r;
    float3 emission = albedoColor * emissionMask.r * emissionMask.a;
    result.rgb = kA + (directlightRadiance * shadow) + emission;
    //result.r = emission;
    result.a = 1;
    return result;
}
