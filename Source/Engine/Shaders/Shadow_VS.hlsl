#include "ConstantBuffers\FrameBuffer.hlsli"
#include "ConstantBuffers\LightBuffer.hlsli"
#include "ConstantBuffers\ObjectBuffer.hlsli"
#include "Include/ShadowInclude.hlsli"
#include "Include/DefaultVertexInput.hlsli"
#include "Include\DefaultMaterialInclude.hlsli"

ShadowResult main(DefaultVertexInput input)
{
    ShadowResult result;
    
    DefaultVertexInput anim;
    anim = input;
    anim.Position = input.Position;
    
	//From local to world
    float4 world = mul(OB_Transform, anim.Position);
	
	//From world to view
    float4 view = mul(FB_InvView, world);
	
	//from view to clip
    float4 clip = mul(FB_Projection, view);
	

    result.Position = clip;
    result.r = input.VertexColor.r;
    
    return result;
}