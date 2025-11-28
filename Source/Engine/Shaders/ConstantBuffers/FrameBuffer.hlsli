cbuffer FrameBuffer : register(b0)
{
	float4x4 FB_InvView;
	float4x4 FB_Projection;
    float4 FB_CameraPosition;
    float2 FB_Resolution;
    float2 FB_Padding2;
}