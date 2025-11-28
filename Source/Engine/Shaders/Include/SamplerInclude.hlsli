SamplerState defaultSampler : register(s0);
SamplerState defaultClampSampler : register(s1);
SamplerState PointClampSampler : register(s2);
SamplerState PointWrapSampler : register(s3);

SamplerState BRDF_LUT_Sampler : register(s14);
SamplerComparisonState shadowCmpSampler : register(s15);