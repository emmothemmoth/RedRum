#include "Include/QuadInclude.hlsli"
#include "Include/LightsInclude.hlsli"
#include "ConstantBuffers/FrameBuffer.hlsli"
#include "Include/DefaultMaterialInclude.hlsli"
#include "ConstantBuffers/GBuffer.hlsli"
#include "ConstantBuffers/MaterialBuffer.hlsli"

float4 main(QuadVStoPS pixel) : SV_TARGET
{
    float4 result = float4(0, 0, 0, 0);
    
    float3 albedo = GBufferAlbedo.Sample(defaultSampler, pixel.UV).rgb;
    float3 material = GBufferMaterial.Sample(defaultSampler, pixel.UV).rgb;
    float3 pixelNormal = GBufferNormal.Sample(defaultSampler, pixel.UV).rgb;
    float3 worldPos = GBufferWorldPos.Sample(defaultSampler, pixel.UV).rgb;
        
    const float occlusionMap = material.r;
    const float roughMap = material.g;
    const float metalMap = material.b;
        
        //Perfect Specular surface color
    const float3 specularColor = lerp((float3) 0.04f, albedo, metalMap);
        //Perfect Diffuse surface color
    const float3 diffuseColor = lerp((float3) 0.00f, albedo, 1 - metalMap);
        
    float3 toView = normalize(FB_CameraPosition.xyz - worldPos);
        
    float3 toDirLight = normalize(DirLight.LightPos.xyz - worldPos);
    float3 halfAngle = normalize((toDirLight + toView));
    
    
      //SPOTLIGHTS
    float3 spotLightTotalRadiance = 0.0f;
    [unroll]
    for (uint spotIndex = 0; spotIndex < 4; ++spotIndex)
    {
        float spotDistance = length(SpotLights[spotIndex].Lightpos - worldPos);
        [flatten]
        if (SpotLights[spotIndex].Active == true && spotDistance <= SpotLights[spotIndex].Range)
        {
            float3 toSpotLight = normalize(SpotLights[spotIndex].Lightpos - worldPos);
            float3 halfAngleSpot = normalize((toSpotLight + toView));
        
            //DirectLighting
            const float3 kS = SpecularBRDF(roughMap, pixelNormal, halfAngleSpot, toView, toSpotLight, specularColor);
        
            float3 kD = DiffuseBRDF(diffuseColor);
        
            //kD = (1.0f - kS);
            const float3 lightColorAndIntensity = SpotLights[spotIndex].Color * SpotLights[spotIndex].Intensity;
        
            const float3 spotlightDirectRadiance = (kD + kS) * lightColorAndIntensity * saturate(dot(pixelNormal, toSpotLight));
        
            //Indirect lighting
            float2 integratedBRDF = IntegrateBRDF(pixel.UV.x, pixel.UV.y);
        
            const float3 diffuseIBL = DiffuseIBL(pixelNormal);
            const float3 specularIBL = SpecularIBL(pixelNormal, toView, roughMap, specularColor);
        
            const float3 kA = (diffuseColor * diffuseIBL + specularIBL) * occlusionMap;
            
    
            float3 spotLightReverseDir = SpotLights[spotIndex].LightDir * -1;
            float spotLightRange = max(SpotLights[spotIndex].Range, 0.001f);
            float distanceAttenuationSpotLight = 1 - pow(spotDistance * (1.0f / spotLightRange), 2);
            float angleAttenuation = pow(dot(spotLightReverseDir, toSpotLight) - sin(SpotLights[spotIndex].OuterConeAngle) / (cos(SpotLights[spotIndex].InnerConeAngle) - cos(SpotLights[spotIndex].OuterConeAngle)), 2);
            float spotAttenuation = mul(distanceAttenuationSpotLight, angleAttenuation);
            
            ////Shadow
            //float shadow = GetSpotLightShadow(float4(worldPos, 1), spotIndex);
            //
            //if (shadow > 0)
            //{
            //    spotLightTotalRadiance += spotlightDirectRadiance * kA *  spotAttenuation;
            //}
            spotLightTotalRadiance += spotlightDirectRadiance * spotAttenuation;
        }
        //float3 emission = GBufferAlbedo.Sample(defaultSampler, pixel.UV).rgb * GBufferFXTexture.Sample(defaultSampler, pixel.UV).r;
        //emission *= 0.0f;
        //spotLightTotalRadiance.rgb += emission;
        //
        //result.rgb = saturate(LinearToGamma(spotLightTotalRadiance));
        float3 albedoColor = GBufferAlbedo.Sample(defaultSampler, pixel.UV).rgb;
        float emissionMask = GBufferFXTexture.Sample(defaultSampler, pixel.UV).r;
        float3 emission = albedoColor * emissionMask * M_EmissionStrength;
        
        result.rgb = spotLightTotalRadiance + emission;
        result.a = 1;
        return result;
    }
    
}