struct ParticleVertex
{
    float4 Position : Position;
    float4 Color : COLOR;
    float3 Velocity : VELOCITY;
    float LifeTime : LIFETIME;
    float Angle : ANGLE;
    float2 Size : SIZE;
    float4 ChannelMask : CHANNELMASK;
};

struct ParticleVStoGS
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float3 Velocity : VELOCITY;
    float LifeTime : LIFETIME;
    float Angle : ANGLE;
    float2 Size : SIZE;
    float4 ChannelMask : CHANNELMASK;
};

struct ParticleGStoPS
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float3 Velocity : VELOCITY;
    float LifeTime : LIFETIME;
    float4 ChannelMask : CHANNELMASK;
    float2 UV : TEXCOORD;
};