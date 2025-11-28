#include "Include/QuadInclude.hlsli"
#include "Include/LightsInclude.hlsli"
#include "ConstantBuffers/FrameBuffer.hlsli"
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
    
    
    float3 pointLightTotalRadiance = 0.0f;
    [unroll]
    for (uint pointIndex = 0; pointIndex < 4; ++pointIndex)
    {
        float pointDistance = length(PointLights[pointIndex].Lightpos - worldPos);
        [flatten]
        if (PointLights[pointIndex].Active == true && pointDistance <= PointLights[pointIndex].Range)
        {
            float3 toPointLight = normalize(PointLights[pointIndex].Lightpos - worldPos);
            float3 halfAnglePoint = normalize((toPointLight + toView));
        
            //DirectLighting
            const float3 kS = SpecularBRDF(roughMap, pixelNormal, halfAnglePoint, toView, toPointLight, specularColor);
        
            float3 kD = DiffuseBRDF(diffuseColor);
        
            //kD *= (1.0f - kS);
            const float3 lightColorAndIntensity = PointLights[pointIndex].Color * PointLights[pointIndex].Intensity;
        
            const float3 pointlightDirectRadiance = (kD + kS) * lightColorAndIntensity * saturate(dot(pixelNormal, toPointLight));
        
            //Indirect lighting
            float2 integratedBRDF = IntegrateBRDF(pixel.UV.x, pixel.UV.y);
        
            const float3 diffuseIBL = DiffuseIBL(pixelNormal);
            const float3 specularIBL = SpecularIBL(pixelNormal, toView, roughMap, specularColor);
        
            const float3 kA = (diffuseColor * diffuseIBL + specularIBL) * occlusionMap;
        
            //Attenuation
            float pointLightRange = max(PointLights[pointIndex].Range, 0.001f);
            float pointAttenuation = 1 - pow(pointDistance * (1.0f / pointLightRange), 2);
            
            
            //float shadow = GetPointLightShadow(float4(worldPos, 1), pointIndex);
            //
            //if (shadow > 0)
            //{
            //    pointLightTotalRadiance += pointlightDirectRadiance * pointAttenuation;
            //}
            //TEMP
           pointLightTotalRadiance += pointlightDirectRadiance * pointAttenuation;
        }
        
        
    }
        
    float3 albedoColor = GBufferAlbedo.Sample(defaultSampler, pixel.UV).rgb;
    float emissionMask = GBufferFXTexture.Sample(defaultSampler, pixel.UV).r;
    float3 emission = albedoColor * emissionMask * M_EmissionStrength;

    result.rgb = pointLightTotalRadiance + emission;
    result.a = 1;
    
    return result;
    
}