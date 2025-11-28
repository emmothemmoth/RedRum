
struct DefaultMaterial_VStoPS
{
    //Clip position
    float4 Position : SV_POSITION;
    //Local position
    float4 LocalPosition : LOCALPOSITION;
    //World position
    float4 WorldPosition : WORLDPOSITION;
    
    //Tangent space
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
    
    //TextureCoordinates
    float2 UV0 : TEXCOORD0;
    float2 UV1 : TEXCOORD1;
    
    //Vertex Color
    float4 VertexColor : COLOR;
};

struct DefaultMaterial_Result
{
    float4 Color : SV_TARGET;
};

struct UI_VS_to_PS
{
    float4 position : SV_Position;
    float4 tint : COLOR;
    float2 uv : TEXCOORD;
};

struct SpriteVertex
{
    float4 position : WORLDPOS;
    float4 color : COLOR;
    float4 uv : TEXCOORD;
};

