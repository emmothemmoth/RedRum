Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D materialTexture : register(t2);
Texture2D fxTexture : register(t3);
Texture2D BlueNoiseTexture : register(t4);
Texture2D ParticleTexture : register(t5);

Texture2D SSAOTexture : register(t119);
TextureCube environmentCube : register(t120);
Texture2D BRDG_LUT_Texture : register(t127);
Texture2D dirLightShadowMap : register(t100);
Texture2D PointLightShadowMaps[4] : register(t101);
Texture2D SpotLightShadowMaps[4] : register(t105);