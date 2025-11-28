#include "Include\DefaultMaterialInclude.hlsli"
#include "Include\DefaultVertexInput.hlsli"
#include "ConstantBuffers\FrameBuffer.hlsli"
#include "ConstantBuffers\ObjectBuffer.hlsli"

DefaultMaterial_VStoPS main(DefaultVertexInput input)
{

    DefaultMaterial_VStoPS result;
    result.LocalPosition = input.Position;
    input.Normal = normalize(input.Normal);
    input.Tangent = normalize(input.Tangent);
    
    if (OB_HasBone)
    {
        float4x4 skinMatrix = GetSkinMatrix(input, OB_BoneTransForms);
        input.Position = mul(input.Position, skinMatrix);
        
        const float3x3 skinNormalRotation = (float3x3) transpose(skinMatrix);
        input.Normal = mul(input.Normal, skinNormalRotation);
        input.Tangent = mul(input.Tangent, skinNormalRotation);
    }
    float3x3 worldInverseTranspose = (float3x3) OB_WorldInvTranspose;
    input.Normal = mul(worldInverseTranspose, input.Normal);
    input.Tangent = mul(worldInverseTranspose, input.Tangent);
    float3 biNormal = cross(input.Normal, input.Tangent);

    
    if (OB_IsInstanced)
    {
        input.Position = mul(input.RelativeTransform, input.Position);
        input.Normal = mul((float3x3) input.RelativeTransform, input.Normal);
    }
	//From local to world
    float4 world = mul(OB_Transform, input.Position);
	
	//From world to view
    float4 view = mul(FB_InvView, world);
	
	//from view to clip
    float4 clip = mul(FB_Projection, view);
	
    result.WorldPosition = world;
    result.Position = clip;
    result.BiNormal = biNormal;
    result.Normal = input.Normal;
    result.Tangent = input.Tangent;
    result.UV0 = input.UV0;
    result.UV1 = input.UV1;
    result.VertexColor = input.VertexColor;
    return result;

}