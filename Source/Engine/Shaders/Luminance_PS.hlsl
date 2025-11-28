#include "Include/SamplerInclude.hlsli"
#include "Include/QuadInclude.hlsli"
#include "ConstantBuffers/FrameBuffer.hlsli"
#include "ConstantBuffers/DebugBuffer.hlsli"

Texture2D intermediateLDR : register(t126);

float4 main(QuadVStoPS pixel) : SV_TARGET
{
    const float4 color = intermediateLDR.Sample(defaultSampler, pixel.UV);
    if (color.a < 0.05f)
        discard;

    const float3 kernel = float3(0.2126f, 0.7152f, 0.0722f);
    float threshold = 0.7f;
    float luminance = dot(color.rgb, kernel);

    switch (DB_LuminanceMode)
    {
        case 0:
            float3 result = color.rgb * pow(luminance, 5);
            return float4(result, 1);
        case 1:
			[flatten]
            if (luminance >= threshold)
                return float4(color.rgb, 1);
            else
                return 0;
        case 2:
            luminance = saturate(dot(color.rgb, kernel) - threshold);
            return float4(color.rgb * luminance * (1.0f / threshold), 1);
        case 3:
			[flatten]
            if (luminance >= threshold)
                return float4(color.rgb, 1);
            else if (luminance >= threshold * 0.5f)
            {
                float fade = luminance / threshold;
                fade = pow(fade, 5);
                return float4(color.rgb * fade, 1);
            }
            else
            {
                return 0;
            }
        default:
            return 1;
    }
}