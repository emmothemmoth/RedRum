#pragma pack_matrix(row_major)

struct GPURay
{
    float3 Origin;
    float Padding1;
    float3 Power;
    float Padding2;
    float3 Direction;
    float Padding3;
};

struct GPUObstacle
{
    matrix InverseTransform;
    matrix Transform;
    float3 MinPoint;
    float Padding1;
    float3 MaxPoint;
    float Padding2;
    float3 Reflection;
    float Padding3;
};

struct GPUMegaHit
{
    float3 RayDirection;
    int ProbeIndex;
    float3 Power;
    float Distance;
};

// --- INPUTS ---
StructuredBuffer<GPURay> InputRays : register(t6);
StructuredBuffer<GPUObstacle> Obstacles : register(t7);
StructuredBuffer<float3> Probes : register(t8);

cbuffer SceneData : register(b7)
{
    float3 ListenerPos; // Kept for legacy/scout rays
    float ListenerRadius; // Used as the radius for ALL probes
    float3 ListenerRight;
    int ObstacleCount;
    int TotalProbeCount; // New: How many probes in the grid
    float3 PaddingCB;
}

// --- OUTPUT ---
// Use an AppendBuffer because one ray can hit 0, 1, or 50 probes
AppendStructuredBuffer<GPUMegaHit> OutputHits : register(u0);

// Helper for Box Collision
bool RaycastAABB(float3 rayOrigin, float3 rayDir, float3 boxMin, float3 boxMax, out float outT, out float3 outNormal)
{
    float3 invDir = 1.0f / (rayDir + 1e-9); // Prevent div by zero
    float3 t0 = (boxMin - rayOrigin) * invDir;
    float3 t1 = (boxMax - rayOrigin) * invDir;

    float3 tmin = min(t0, t1);
    float3 tmax = max(t0, t1);

    float maxTmin = max(max(tmin.x, tmin.y), tmin.z);
    float minTmax = min(min(tmax.x, tmax.y), tmax.z);

    if (maxTmin > minTmax || minTmax < 0.0f) 
        return false;

    outT = maxTmin;

    // Calculate Normal based on which side was hit
    if (outT == tmin.x)
        outNormal = float3(-sign(rayDir.x), 0, 0);
    else if (outT == tmin.y)
        outNormal = float3(0, -sign(rayDir.y), 0);
    else
        outNormal = float3(0, 0, -sign(rayDir.z));

    return true;
}

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    
    uint totalRays, stride;
    InputRays.GetDimensions(totalRays, stride);
    if (index >= totalRays)
        return;

    GPURay currentRay = InputRays[index];
    float accumulatedDistance = 0.0f;
    const int MAX_BOUNCES = 200;
    const float PI = 3.14159265359f;

    for (int bounce = 0; bounce < MAX_BOUNCES; ++bounce)
    {
        float closestWallT = 1000000.0f;
        float3 bestNormal = float3(0, 0, 0);
        float3 bestReflection = float3(0, 0, 0);

        // ==========================================
        // 1. CHECK OBSTACLES FIRST (Find the wall)
        // ==========================================
        for (int i = 0; i < ObstacleCount; ++i)
        {
            GPUObstacle obs = Obstacles[i];
            
            float3 localOrigin = mul(float4(currentRay.Origin, 1.0f), obs.InverseTransform).xyz;
            float3 localDir = mul(float4(currentRay.Direction, 0.0f), obs.InverseTransform).xyz;
            
            float hitT;
            float3 localNormal;
            if (RaycastAABB(localOrigin, localDir, obs.MinPoint, obs.MaxPoint, hitT, localNormal))
            {
                float worldT = hitT * length(mul(float4(localDir, 0.0f), obs.Transform).xyz);
                
                if (worldT > 0.01f && worldT < closestWallT)
                {
                    closestWallT = worldT;
                    bestNormal = normalize(mul(float4(localNormal, 0.0f), obs.Transform).xyz);
                    bestReflection = obs.Reflection;
                }
            }
        }

        // ==========================================
        // 2. CHECK PROBES (Using known wall distance)
        // ==========================================
        for (int p = 0; p < TotalProbeCount; ++p)
        {
            float3 L = Probes[p] - currentRay.Origin;
            float tca = dot(L, currentRay.Direction);
            
            float d2 = dot(L, L) - (tca * tca);
            float radius2 = ListenerRadius * ListenerRadius;

            if (d2 <= radius2)
            {
                float thc = sqrt(radius2 - d2);
                float t0 = tca - thc;
                float t1 = tca + thc;
                
                float hitT = (t0 > 0.01f) ? t0 : t1;

                // CRITICAL FIX: Only log if the probe is CLOSER than the wall!
                if (hitT > 0.01f && hitT < closestWallT)
                {
                    if (bounce > 0)
                    {
                        GPUMegaHit hit;
                        hit.ProbeIndex = p;
                        hit.Distance = accumulatedDistance + hitT;
                        hit.Power = currentRay.Power;
                        hit.RayDirection = currentRay.Direction;
                    
                        OutputHits.Append(hit);
                    }
                }
            }
        }

        // ==========================================
        // 3. ADVANCE AND BOUNCE
        // ==========================================
        if (closestWallT > 99999.0f)
            break; // Ray escaped into the void (no walls hit)

        accumulatedDistance += closestWallT;
        float3 hitPoint = currentRay.Origin + (currentRay.Direction * closestWallT);

        // Reflection math
        currentRay.Direction = reflect(currentRay.Direction, bestNormal);
        currentRay.Origin = hitPoint + (bestNormal * 0.1f); // Nudge off surface 
        currentRay.Power *= bestReflection; // Wall absorption
    }
}