
struct BRDF_VS_to_PS
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
};

struct BRDF_LUT_Output
{
    float4 PixelColor : SV_TARGET;
};
