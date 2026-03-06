#include "Include\DefaultMaterialInclude.hlsli"
#include "ConstantBuffers\ObjectIDBuffer.hlsli"


uint main(DefaultMaterial_VStoPS input) : SV_Target
{
    return OIB_ID;
}