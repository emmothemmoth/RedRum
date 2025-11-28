cbuffer DebugBuffer : register(b10)
{
    float DB_Exposure;
    int DB_Tonemap;
    int DB_BloomMode;
    int DB_LuminanceMode;
    int DB_SSAOActive;
    float3 DB_Padding;
};