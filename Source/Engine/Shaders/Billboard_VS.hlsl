#include "Include\BillboardInclude.hlsli"
#include "ConstantBuffers\FrameBuffer.hlsli"
#include "ConstantBuffers\ObjectBuffer.hlsli"

BillboardVStoPS main(BillboardVS_INPUT input)
{
    BillboardVStoPS output;

    float size = 1.0f;

    float4 centerWorld = mul(OB_Transform, float4(input.Center, 1));

    float3 worldPos =
        centerWorld.xyz +
        FB_CameraRight.xyz * input.Corner.x * size +
        FB_CameraUp.xyz * input.Corner.y * size;

    float4 world = float4(worldPos, 1);

    float4 view = mul(FB_InvView, world);
    float4 clip = mul(FB_Projection, view);

    output.Position = clip;
    output.UV = input.UV;

    return output;
}