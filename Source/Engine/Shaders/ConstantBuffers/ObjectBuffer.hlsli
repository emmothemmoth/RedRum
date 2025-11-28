cbuffer ObjectBuffer : register(b1)
{
	float4x4 OB_Transform;
    float4x4 OB_WorldInvTranspose;
    float4x4 OB_BoneTransForms[128];
    bool OB_HasBone;
    bool OB_IsInstanced;
    float2 OB_padding;
}