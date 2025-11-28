#include"Include\DefaultMaterialInclude.hlsli"
#include "Include/LightsInclude.hlsli"
#include "ConstantBuffers\FrameBuffer.hlsli"
#include "ConstantBuffers\MaterialBuffer.hlsli"


DefaultMaterial_Result main(DefaultMaterial_VStoPS input)
{
    DefaultMaterial_Result result;
    result.Color.r = 0;
    result.Color.g = 0;
    result.Color.b = 0;
    result.Color.a = 0;
    
    float3 kA = 0.0f;
    
    //MY OLD CODE
    const float PI = 3.14159265f;
    
    //Create TBN Matrix
    const float3x3 TBN = float3x3
    (
    normalize(input.Tangent),
    normalize(input.BiNormal),
    normalize(input.Normal)
    );
    
    //scale UV
    const float2 scaledUV = input.UV0 * M_UVTiling;
    
    //Sample pixelNormal from normalMap
    const float2 normalSample = normalTexture.Sample(defaultSampler, scaledUV).rg;
    float3 pixelNormal = float3(normalSample, 0);
    pixelNormal = 2.0f * pixelNormal - 1.0f;
    pixelNormal.z = sqrt(1 - saturate(dot(pixelNormal.xy, pixelNormal.xy)));
    pixelNormal = normalize(pixelNormal);
    pixelNormal.xy *= M_NormalStrength;
    pixelNormal = normalize(mul(pixelNormal, TBN));

    
    //Sample materialTexture
    const float3 materialMap = materialTexture.Sample(defaultSampler, scaledUV).rgb;
    const float metalMap = materialMap.b;
    const float roughMap = materialMap.g;
    const float occlusionMap = materialMap.r;

    //Sample albedoTexture
    const float4 albedoMap = albedoTexture.Sample(defaultSampler, scaledUV);
    
    //Perfect Specular surface color
    const float3 specularColor = lerp((float3) 0.04f, albedoMap.rgb, metalMap);
    //Perfect Diffuse surface color
    const float3 diffuseColor = lerp((float3) 0.00f, albedoMap.rgb, 1 - metalMap);
 
    float3 toView = normalize(FB_CameraPosition.xyz - input.WorldPosition.xyz);
    
    //DIRECTIONAL LIGHT
    float3 directlightRadiance = 0.0f;
        float3 toDirLight = normalize(DirLight.LightPos.xyz - input.WorldPosition.xyz);
        float3 halfAngle = normalize((toDirLight + toView));
        
        //Direct lighting
        float3 kS = SpecularBRDF(roughMap, pixelNormal, halfAngle, toView, toDirLight, specularColor);
        
        float3 kD = DiffuseBRDF(diffuseColor);
        
        kD *= (1.0f - kS);
        const float3 lightColorAndIntensity = DirLight.Color * DirLight.Intensity;
        
        
        float3 directlightDirectRadiance = (kD + kS) * lightColorAndIntensity * saturate(dot(pixelNormal, toDirLight));
        float dirLightAttenuation = saturate(dot(pixelNormal, normalize(-DirLight.LightDir.rgb)));
        
        //Indirect lighting
        float2 integratedBRDF = IntegrateBRDF(input.UV0.x, input.UV0.y);
        
        const float3 diffuseIBL = DiffuseIBL(pixelNormal);
        const float3 specularIBL = SpecularIBL(pixelNormal, toView, roughMap, specularColor);
        
        
        kA = (diffuseColor * diffuseIBL + specularIBL) * occlusionMap;
        

        float shadow = GetDirLightShadow(input.WorldPosition);
        [flatten]
    if (DirLight.Active == false)
    {
        directlightDirectRadiance = 0.0f;
        
    }
    directlightRadiance = directlightDirectRadiance * shadow + kA;
    
    //POINTLIGHTS
    float3 pointLightTotalRadiance = 0.0f;
    [unroll]
    for (uint pointIndex = 0; pointIndex < 4; ++pointIndex)
    {
        float pointDistance = length(PointLights[pointIndex].Lightpos - input.WorldPosition.xyz);
        [flatten]
        if (PointLights[pointIndex].Active == true && pointDistance < PointLights[pointIndex].Range)
        {
            float3 toPointLight = normalize(PointLights[pointIndex].Lightpos - input.WorldPosition.xyz);
            float3 halfAnglePoint = normalize((toPointLight + toView));
        
            //DirectLighting
            const float3 kS = SpecularBRDF(roughMap, pixelNormal, halfAnglePoint, toView, toPointLight, specularColor);
        
            float3 kD = DiffuseBRDF(diffuseColor);
        
            //kD *= (1.0f - kS);
            const float3 lightColorAndIntensity = PointLights[pointIndex].Color * PointLights[pointIndex].Intensity;
        
            const float3 pointlightDirectRadiance = (kD + kS) * lightColorAndIntensity * saturate(dot(pixelNormal, toPointLight));
        
            //Indirect lighting
            float2 integratedBRDF = IntegrateBRDF(input.UV0.x, input.UV0.y);
        
            const float3 diffuseIBL = DiffuseIBL(pixelNormal);
            const float3 specularIBL = SpecularIBL(pixelNormal, toView, roughMap, specularColor);
        
            const float3 kA = (diffuseColor * diffuseIBL + specularIBL) * occlusionMap;
        
            //Attenuation
            float pointLightRange = max(PointLights[pointIndex].Range, 0.001f);
            float pointAttenuation = 1 - pow(pointDistance * (1.0f / pointLightRange), 2);
            
            pointLightTotalRadiance += pointlightDirectRadiance * pointAttenuation;

        }
        
    }
     //SPOTLIGHTS
    float3 spotLightTotalRadiance = 0.0f;
    [unroll]
    for (uint spotIndex = 0; spotIndex < 4; ++spotIndex)
    {
        float spotDistance = length(SpotLights[spotIndex].Lightpos - input.WorldPosition.xyz);
        [flatten]
        if (SpotLights[spotIndex].Active == true && spotDistance < SpotLights[spotIndex].Range)
        {
            float3 toSpotLight = normalize(SpotLights[spotIndex].Lightpos - input.WorldPosition.xyz);
            float3 halfAngleSpot = normalize((toSpotLight + toView));
        
            //DirectLighting
            const float3 kS = SpecularBRDF(roughMap, pixelNormal, halfAngleSpot, toView, toSpotLight, specularColor);
        
            float3 kD = DiffuseBRDF(diffuseColor);
        
            //kD = (1.0f - kS);
            const float3 lightColorAndIntensity = SpotLights[spotIndex].Color * SpotLights[spotIndex].Intensity;
        
            const float3 spotlightDirectRadiance = (kD + kS) * lightColorAndIntensity * saturate(dot(pixelNormal, toSpotLight));
        
            //Indirect lighting
            float2 integratedBRDF = IntegrateBRDF(input.UV0.x, input.UV0.y);
        
            const float3 diffuseIBL = DiffuseIBL(pixelNormal);
            const float3 specularIBL = SpecularIBL(pixelNormal, toView, roughMap, specularColor);
        
            const float3 kA = (diffuseColor * diffuseIBL + specularIBL) * occlusionMap;
            
            
            //Attenuation
            float3 spotLightReverseDir = SpotLights[spotIndex].LightDir * -1;
            float spotLightRange = max(SpotLights[spotIndex].Range, 0.001f);
            float distanceAttenuationSpotLight = 1 - pow(spotDistance * (1.0f / spotLightRange), 2);
            float angleAttenuation = pow(dot(spotLightReverseDir, toSpotLight) - sin(SpotLights[spotIndex].OuterConeAngle) / (cos(SpotLights[spotIndex].InnerConeAngle) - cos(SpotLights[spotIndex].OuterConeAngle)), 2);
            float spotAttenuation = mul(distanceAttenuationSpotLight, angleAttenuation);
            
            
            // //Shadow
            //float shadow = GetSpotLightShadow(input.WorldPosition, spotIndex);
            //
            //if (shadow > 0)
            //{
            //    spotLightTotalRadiance += spotlightDirectRadiance * kA * spotAttenuation;
            //}
            
            spotLightTotalRadiance += spotlightDirectRadiance * spotAttenuation;

        }
       
    
    }
    
    //Assemble total light
    float3 radiance = (directlightRadiance + pointLightTotalRadiance + spotLightTotalRadiance);
    float emissionMask = fxTexture.Sample(defaultSampler, scaledUV).r;
    float3 emission = albedoMap.rgb * emissionMask * M_EmissionStrength;
    
    result.Color.rgb = radiance + emission;
    result.Color.a = M_AlbedoColor.a;
    return result;
}
