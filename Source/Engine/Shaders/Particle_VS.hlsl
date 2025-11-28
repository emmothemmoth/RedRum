#include "Include/ParticleInclude.hlsli"
#include "ConstantBuffers/FrameBuffer.hlsli"
#include "ConstantBuffers/ObjectBuffer.hlsli"

ParticleVStoGS main(ParticleVertex input)
{
    ParticleVStoGS result;
    
    //From local to world
    float4 world = mul(OB_Transform, input.Position);
	
	//From world to view
    float4 view = mul(FB_InvView, world);
    
    //from view to clip
    float4 clip = mul(FB_Projection, view);
    
    result.Position = clip;
    result.ChannelMask = input.ChannelMask,
    result.Color = input.Color;
    result.LifeTime = input.LifeTime;
    result.Size = input.Size;
    result.Velocity = input.Velocity;
    result.Angle = input.Angle;
    
    return result;
}