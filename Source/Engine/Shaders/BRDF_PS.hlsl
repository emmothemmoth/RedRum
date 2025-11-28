#include "Include/BRDFInclude.hlsli"
#include "Include/IBLBRDF.hlsli"
#include "Include/QuadInclude.hlsli"

float2 main(QuadVStoPS input) : SV_TARGET
{
    float2 integratedBRDF = IntegrateBRDF(input.UV.x, input.UV.y);
    return integratedBRDF;
}