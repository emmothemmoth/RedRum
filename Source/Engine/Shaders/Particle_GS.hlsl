#include "Include/ParticleInclude.hlsli"

struct GSOutput
{
	float4 pos : SV_POSITION;
};


float4x4 CreateRotationAroundX(const float anAngleRadians)
{
    float4x4 rotationMatrix;
    rotationMatrix[0][0] = 1.0f;
    rotationMatrix[0][1] = 0.0f;
    rotationMatrix[0][2] = 0.0f;
    rotationMatrix[0][3] = 0.0f;

    rotationMatrix[1][0] = 0.0f;
    rotationMatrix[1][1] = cos(anAngleRadians);
    rotationMatrix[1][2] = sin(anAngleRadians);
    rotationMatrix[1][3] = 0.0f;

    rotationMatrix[2][0] = 0.0f;
    rotationMatrix[2][1] = -1.0f * sin(anAngleRadians);
    rotationMatrix[2][2] = cos(anAngleRadians);
    rotationMatrix[2][3] = 0.0f;
			
    rotationMatrix[3][0] = 0.0f;
    rotationMatrix[3][1] = 0.0f;
    rotationMatrix[3][2] = 0.0f;
    rotationMatrix[3][3] = 1.0f;
    return rotationMatrix;
}
float4x4 CreateRotationAroundY(const float anAngleRadians)
{
    float4x4 rotationMatrix;
    rotationMatrix[0][0] = cos(anAngleRadians);
    rotationMatrix[0][1] = 0.0f;
    rotationMatrix[0][2] = -1.0f * sin(anAngleRadians);
    rotationMatrix[0][3] = 0.0f;

    rotationMatrix[1][0] = 0.0f;
    rotationMatrix[1][1] = 1.0f;
    rotationMatrix[1][2] = 0.0f;
    rotationMatrix[1][3] = 0.0f;

    rotationMatrix[2][0] = sin(anAngleRadians);
    rotationMatrix[2][1] = 0.0f;
    rotationMatrix[2][2] = cos(anAngleRadians);
    rotationMatrix[2][3] = 0.0f;
			
    rotationMatrix[3][0] = 0.0f;
    rotationMatrix[3][1] = 0.0f;
    rotationMatrix[3][2] = 0.0f;
    rotationMatrix[3][3] = 1.0f;
    return rotationMatrix;
    
}

float4x4 CreateRotationAroundZ(const float anAngleRadians)
{
    float4x4 rotationMatrix;
    rotationMatrix[0][0] = cos(anAngleRadians);
    rotationMatrix[0][1] = sin(anAngleRadians);
    rotationMatrix[0][2] = 0.0f;
    rotationMatrix[0][3] = 0.0f;

    rotationMatrix[1][0] = -1.0f * sin(anAngleRadians);;
    rotationMatrix[1][1] = cos(anAngleRadians);
    rotationMatrix[1][2] = 0.0f;
    rotationMatrix[1][3] = 0.0f;

    rotationMatrix[2][0] = 0.0f;
    rotationMatrix[2][1] = 0.0f;
    rotationMatrix[2][2] = 1.0f;
    rotationMatrix[2][3] = 0.0f;
			
    rotationMatrix[3][0] = 0.0f;
    rotationMatrix[3][1] = 0.0f;
    rotationMatrix[3][2] = 0.0f;
    rotationMatrix[3][3] = 1.0f;
    return rotationMatrix;
}

[maxvertexcount(4)]
void main(point ParticleVStoGS input[1], inout TriangleStream<ParticleGStoPS> output)
{
    const ParticleVStoGS vertex = input[0];
    
    const float PI = 3.14159265358979323846f;
    const float DEGTORAD = PI / 180.0f;
    
    float2 offsets[4] =
    {
        { -1.0f, 1.0f },
        { 1.0f, 1.0f },
        { -1.0f, -1.0f },
        { 1.0f, -1.0f }
    };

    float2 uvs[4] =
    {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f }
    };
    
    for (uint index = 0; index < 4; ++index)
    {
        ParticleGStoPS result;
        result.Position = vertex.Position;
        float4 origin = result.Position;
        result.Position.xy += offsets[index] * vertex.Size;
        
        //Rotate result.position around origin
        //float4x4 rotationX = CreateRotationAroundX(vertex.Angle * DEGTORAD);
        //float4x4 rotationY = CreateRotationAroundY(vertex.Angle * DEGTORAD);
        //float4x4 rotationZ = CreateRotationAroundZ(vertex.Angle * DEGTORAD);
        //
        //float4x4 combinedRotations = rotationX * rotationY * rotationZ;
        //result.Position = mul(combinedRotations, result.Position);
        
        result.Color = vertex.Color;
        result.LifeTime = vertex.LifeTime;
        result.Velocity = vertex.Velocity;
        result.ChannelMask = vertex.ChannelMask;
        result.UV = uvs[index];
        output.Append(result);

    }

}