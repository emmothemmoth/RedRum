#include "../ConstantBuffers/LightBuffer.hlsli"
#include "SamplerInclude.hlsli"
#include "TextureInclude.hlsli"

#include "IBLBRDF.hlsli"

float GetDirLightShadow(float4 aWorldPosition)
{
    
    float4 lightSpacePos = mul(DirLight.LightViewInv, float4(aWorldPosition.xyz, 1));
    lightSpacePos = mul(DirLight.LightProj, lightSpacePos);
    
    float3 lightSpaceUV = lightSpacePos.xyz / lightSpacePos.w;
    float shadowBias = 0.005f;
    float D = lightSpaceUV.z - shadowBias;
    
    float2 shadowUV = lightSpaceUV.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y;
    
    return dirLightShadowMap.SampleCmpLevelZero(shadowCmpSampler, shadowUV, D).r;
}

float GetPointLightShadow(float4 aWorldPosition, int aLightIndex)
{
    float result;
    
    float4 lightSpacePos = mul(DirLight.LightViewInv, aWorldPosition);
    lightSpacePos = mul(DirLight.LightProj, lightSpacePos);
    
    float3 lightSpaceUV = lightSpacePos.xyz / lightSpacePos.w;
    lightSpaceUV.y *= -1.0f;
    float shadowBias = 0.0015f;
    float D = lightSpaceUV.z - shadowBias;
    
    lightSpaceUV.xy = (lightSpaceUV.xy * 0.5f) + 0.5f;
    
    result = PointLightShadowMaps[aLightIndex].SampleCmpLevelZero(shadowCmpSampler, lightSpaceUV.xy, D).r;
    
    return result;
}

float GetSpotLightShadow(float4 aWorldPosition, int aLightIndex)
{
    float result;
    
    float4 lightSpacePos = mul(DirLight.LightViewInv, aWorldPosition);
    lightSpacePos = mul(DirLight.LightProj, lightSpacePos);
    
    float3 lightSpaceUV = lightSpacePos.xyz / lightSpacePos.w;
    lightSpaceUV.y *= -1.0f;
    float shadowBias = 0.0015f;
    float D = lightSpaceUV.z - shadowBias;
    
    lightSpaceUV.xy = (lightSpaceUV.xy * 0.5f) + 0.5f;
    
    result = SpotLightShadowMaps[aLightIndex].SampleCmpLevelZero(shadowCmpSampler, lightSpaceUV.xy, D).r;
    
    return result;
}

float3 LinearToGamma(float3 aColor)
{
    return pow(abs(aColor), 1.0f / 2.2);
}

int GetNumMips(TextureCube cubeTex)
{
    int iWidth = 0;
    int iHeight = 0;
    int numMips = 0;
    cubeTex.GetDimensions(0, iWidth, iHeight, numMips);
    return numMips;
}

//Assumes shininess lies between 0-1000
int GetMipLevel(float aShininess, int aLevelOfMips)
{
    float mipRange = 1000 / (float) aLevelOfMips;
    float actualMipLevel = aShininess / mipRange;
    int finalMip = aLevelOfMips - (int) round(actualMipLevel);
    return finalMip;
}

float3 SpecularBRDF(float aRoughness, float3 aNormal, float3 aHalfAngle, float3 aToView, float3 aToLight, float3 aSpecularColor)
{
    float normalDotLight = saturate(dot(aNormal, aToLight));
    float normalDotView = saturate(dot(aNormal, aToView));
    float normalDistribution = NormalDistributionFunction_GGX(aRoughness, saturate(dot(aNormal, aHalfAngle)));
    float3 fresnel = Fresnel_SphericalGaussianSchlick(aToView, aHalfAngle, aSpecularColor);
    float geometricAttenuationSchlick = GeometricAttenuation_Schlick(normalDotView, normalDotLight, aRoughness);
    return (normalDistribution * fresnel * geometricAttenuationSchlick) / (max(4.0f * normalDotLight * normalDotView, 0.01));
    
    //return (NormalDistributionFunction_GGX(aRoughness, saturate(dot(aNormal, aHalfAngle))) *
	//		Fresnel_SphericalGaussianSchlick(aToView, aHalfAngle, aSpecularColor) *
	//		GeometricAttenuation_Schlick(normalDotView, normalDotLight, aRoughness)) /
	//		max(4.f * normalDotLight * normalDotView, 0.01);
    //
    //const float PI = 3.14159265f;
    ////Direct specular D
    //const float a = aRoughness * aRoughness;
    //const float a2 = a * a;
    //const float specD = a2 / PI * pow(pow(saturate(dot(aNormal, aHalfAngle)), 2) * (a2 - 1) + 1, 2);
    //    
    ////Direct specular F
    //const float p = (-5.55473f * saturate(dot(aToView, aHalfAngle)) - 6.98316) * saturate(dot(aToView, aHalfAngle));
    //const float3 specF = aSpecularColor + (1 - aSpecularColor) * pow(2, p);
    //        
    ////Direct specular G
    //const float k = pow(aRoughness + 1, 2) / 8.0f;
    //const float3 specG = (saturate(dot(aNormal, aToView)) / saturate((dot(aNormal, aToView)) * (1 - k) + k)) * saturate((dot(aNormal, aToLight)) / saturate((dot(aNormal, aToLight)) * (1 - k) + k));
    //
    //const float3 totalSpecular = specD * specF * specG / 4 * saturate((dot(aNormal, aToLight)) * saturate(dot(aNormal, aToView)));
    //return totalSpecular;
}

float3 DiffuseBRDF(float3 aDiffuseColor)
{
    const float PI = 3.14159265f;
    return aDiffuseColor / PI;
}

float3 DiffuseIBL(float3 aNormal)
{
    const int numMips = GetNumMips(environmentCube) - 1;
        
    const float3 iblDiffuse = environmentCube.SampleLevel(defaultSampler, aNormal, numMips).rgb;
    return iblDiffuse;
}

float3 SpecularIBL(float3 aNormal, float3 aToView, float aRoughness, float3 aSpecularColor)
{
    const int numMips = GetNumMips(environmentCube) - 1;
    
    //Figure out where to read cube map
    const float3 R = reflect(-aToView, aNormal);
    
    //Get a pixel from a specific mip
    const float3 envColor = environmentCube.SampleLevel(defaultSampler, R, aRoughness * numMips).rgb;
    
    //Fetch integrad result from the LUT
    const float NdotV = saturate(dot(aNormal, aToView));
    const float2 brdfLUT = BRDG_LUT_Texture.Sample(BRDF_LUT_Sampler, float2(NdotV, aRoughness)).rg;
    const float3 iblSpecular = envColor * (aSpecularColor * brdfLUT.x + brdfLUT.y);

    return iblSpecular;
}