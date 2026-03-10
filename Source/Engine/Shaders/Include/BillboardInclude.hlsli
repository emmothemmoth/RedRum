struct BillboardVS_INPUT
{
    float3 Center : POSITION;
    float2 Corner : TEXCOORDFIRST;
    float2 UV : TEXCOORDSECOND;
};

struct BillboardVStoPS
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
};
