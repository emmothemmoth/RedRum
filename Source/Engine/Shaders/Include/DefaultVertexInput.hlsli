struct DefaultVertexInput
{
    float4 Position             : POSITION;
    float4 VertexColor          : COLOR;
    float3 Normal               : NORMAL;
    float3 Tangent              : TANGENT;
    uint4 BoneIDs               : BONEIDS;
    float4 BoneWeights          : BONEWEIGHTS;
    float2 UV0                  : TEXCOORDFIRST;
    float2 UV1                  : TEXCOORDSECOND;
    float4x4 RelativeTransform  : RELATIVEXF;
};

float4x4 GetSkinMatrix(DefaultVertexInput input, float4x4 BoneTransforms[128])
{
    float4x4 skinMatrix = 0;
    skinMatrix += mul(input.BoneWeights.x, BoneTransforms[input.BoneIDs.x]);
    skinMatrix += mul(input.BoneWeights.y, BoneTransforms[input.BoneIDs.y]);
    skinMatrix += mul(input.BoneWeights.z, BoneTransforms[input.BoneIDs.z]);
    skinMatrix += mul(input.BoneWeights.w, BoneTransforms[input.BoneIDs.w]);
    
    return skinMatrix;
}
