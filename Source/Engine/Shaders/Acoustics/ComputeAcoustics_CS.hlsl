
#pragma pack_matrix(row_major)
struct GPURay
{
    float3 Origin;
    float Power;
    float3 Direction;
    float Padding;
};

struct GPUObstacle
{
    matrix InverseTransform;
    matrix Transform;
    float3 MinPoint;
    float Absorption;
    float3 MaxPoint;
    float Padding;
};

struct GPURayResult
{
    int HitListener;
    float TotalDistance;
    float FinalPower;
    float PanAngle;
};

// INPUTS
StructuredBuffer<GPURay> InputRays : register(t6);
StructuredBuffer<GPUObstacle> Obstacles : register(t7);

cbuffer SceneData : register(b7)
{
    float3 ListenerPos;
    float ListenerRadius;
    float3 ListenerRight;
    int ObstacleCount;
}

// OUTPUT
RWStructuredBuffer<GPURayResult> OutputResults : register(u0);

// Helper function for AABB Raycast
bool RaycastAABB(float3 rayOrigin, float3 rayDir, float3 boxMin, float3 boxMax, out float outT, out float3 outNormal)
{
    float3 invDir = 1.0f / rayDir;
    float3 t0 = (boxMin - rayOrigin) * invDir;
    float3 t1 = (boxMax - rayOrigin) * invDir;

    float3 tmin = min(t0, t1);
    float3 tmax = max(t0, t1);

    float maxTmin = max(max(tmin.x, tmin.y), tmin.z);
    float minTmax = min(min(tmax.x, tmax.y), tmax.z);

    if (maxTmin > minTmax || minTmax < 0.0f) 
        return false;

    outT = maxTmin;

    // Calculate Normal
    if (outT == tmin.x)
        outNormal = float3(sign(rayOrigin.x - boxMin.x), 0, 0); // Need to adjust based on exact bounds logic
    else if (outT == tmin.y)
        outNormal = float3(0, sign(rayOrigin.y - boxMin.y), 0);
    else
        outNormal = float3(0, 0, sign(rayOrigin.z - boxMin.z));

    return true;
}

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    
    // Safety check (in case we dispatch more threads than rays)
    uint totalRays, stride;
    InputRays.GetDimensions(totalRays, stride);
    if (index >= totalRays)
        return;

    GPURay currentRay = InputRays[index];
    float totalDistance = 0.0f;
    
    GPURayResult result;
    result.HitListener = 0;
    result.TotalDistance = 0.0f;
    result.FinalPower = 0.0f;
    result.PanAngle = 0.0f;

    const int MAX_BOUNCES = 10;

    for (int bounce = 0; bounce < MAX_BOUNCES; ++bounce)
    {
        float closestT = 999999.0f;
        float3 bestNormal = float3(0, 0, 0);
        bool hitListener = false;

        // A. Check Listener Sphere
        float3 L = ListenerPos - currentRay.Origin;
        float tca = dot(L, currentRay.Direction);
        if (tca > 0.0f)
        {
            float d2 = dot(L, L) - (tca * tca);
            float radius2 = ListenerRadius * ListenerRadius;
            if (d2 < radius2)
            {
                float thc = sqrt(radius2 - d2);
                float listenerT = tca - thc;
                if (listenerT > 0.01f && listenerT < closestT)
                {
                    closestT = listenerT;
                    hitListener = true;
                }
            }
        }

        // B. Check Obstacles
        for (int i = 0; i < ObstacleCount; ++i)
        {
            GPUObstacle obs = Obstacles[i];
            
            float3 localOrigin = mul(float4(currentRay.Origin, 1.0f), obs.InverseTransform).xyz;
            float3 localDir = mul(float4(currentRay.Direction, 0.0f), obs.InverseTransform).xyz;
            
            float hitT;
            float3 localNormal;
            if (RaycastAABB(localOrigin, localDir, obs.MinPoint, obs.MaxPoint, hitT, localNormal))
            {
                float worldT = hitT * length(localDir); // Scale back to world space
                
                if (worldT > 0.01f && worldT < closestT)
                {
                    closestT = worldT;
                    hitListener = false;
                    bestNormal = normalize(mul(float4(localNormal, 0.0f), obs.Transform).xyz);
                }
            }
        }

        if (closestT == 999999.0f)
            break; // Ray escaped

        totalDistance += closestT;
        float3 hitPoint = currentRay.Origin + (currentRay.Direction * closestT);

        if (hitListener)
        {
            result.HitListener = 1;
            result.TotalDistance = totalDistance;
            result.FinalPower = currentRay.Power;
            
            float pan = dot(-currentRay.Direction, ListenerRight);
            result.PanAngle = (pan + 1.0f) * 0.5f * (3.14159f * 0.5f);
            break;
        }
        else
        {
            currentRay.Power *= 0.95f; // Wall absorption
            currentRay.Direction = reflect(currentRay.Direction, bestNormal);
            currentRay.Origin = hitPoint + (bestNormal * 0.1f);
        }
    }

    OutputResults[index] = result;
}