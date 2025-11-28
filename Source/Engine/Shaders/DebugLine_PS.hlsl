#include "Include/DefaultMaterialInclude.hlsli"


float4 main(DefaultMaterial_VStoPS input) : SV_TARGET
{
    float4 result;
    result.rgba = input.VertexColor;
    return result;
}