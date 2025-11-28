struct ShadowVSInput
{
    float4 worldPosition : POSITION;
};

struct ShadowResult
{
    float r : Depth;
    float4 Position : SV_POSITION;
};