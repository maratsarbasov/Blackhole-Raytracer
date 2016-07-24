//#include <math.h>
#define PHYSICS_G 6.67e-11
#define PHYSICS_C 3e8



typedef struct
    {
        int start;
        float3 m_start;
        float3 m_dir;
    } CLRay;

typedef struct
    {
        float3 m_pos;          // Camera position and orientation
        float3 m_forward;      // Orthonormal basis
        float3 m_right;
        float3 m_up;
        
        float2 m_viewAngle;    // View angles, rad
        float2 m_resolution;  // Image resolution: w, h
        
    }CLCamera;

typedef struct
    {
        float3 intersect1;
        float3 intersect2;
        float3 intersect3;
        int type;
        // 1 - disk, blackhole
        // 2 - blackhole
        // 3 - disk, background
        // 4 - background
        // 5 - disk, disk, blackhole
        // 6 - disk, disk, disk
        // 7 - disk, disk, background
    } Result;

typedef struct
    {
        float cameraDistance;
        float blackholeMass;
        float blackholeRadius;
        float diskCoef;
    } CLSettings;



CLRay MakeRay(int x, int y, float offset, __global CLCamera *gcamera)
{
    CLRay ray;
    ray.m_start = gcamera->m_pos;
    ray.m_dir = gcamera->m_forward + ((x + offset)/gcamera->m_resolution.x - (float)0.5) * gcamera->m_right + ((y + offset)/gcamera->m_resolution.y - (float)0.5) * gcamera->m_up; //big formula
    
    return ray;
}




Result TraceRay(CLRay ray, __global  CLSettings *settings)
{
    float blackholeMass = settings->blackholeMass;
    float blackholeRadius = settings->blackholeRadius;
    float diskCoef = settings->diskCoef;
    
    
    const float delta_t = 5;
    float3 position = ray.m_start;
    float cur_t = 0;
    float3 velocity = normalize(ray.m_dir) * (float)PHYSICS_C;
    
    int intersect1isset = 0;
    int intersect2isset = 0;
    float3 intersect1 = (float3)(0,0,0);
    float3 intersect2 = (float3)(0,0,0);
    float3 intersect3 = (float3)(0,0,0);
    
    Result result;
    while (1) {
        float3 r = -position;
        float coef = (float)PHYSICS_G * (float)blackholeMass / length(r);
        coef /= length(r);
        coef /= length(r);
        //float3 acceleration = r * (float)PHYSICS_G * (float)BLACKHOLE_MASS / pow(length(r), 3);
        float3 acceleration = r * coef;
        
        float3 old_position = position;
        position = position + velocity* delta_t + acceleration * (float)pow(delta_t, 2) / (float)2.0;
        velocity = velocity + acceleration *delta_t;
        cur_t += delta_t;
        
        //sphere intersection
        if (length(position) < blackholeRadius) {
            
            if (intersect1isset) {
                result.type = 1;
                result.intersect1 = intersect1;
                result.intersect2 = position;
                result.intersect3 = (float3)(0,0,0);
                return result;
            }
            if (intersect2isset) {
                result.type = 5;
                result.intersect1 = intersect1;
                result.intersect2 = intersect2;
                result.intersect3 = position;
                return result;
            } else {
                result.type = 2;
                result.intersect1 = position;
                result.intersect3 = (float3)(0,0,0);
                result.intersect2 = (float3)(0,0,0);
                return result;
            }
            
        }
        
        //disk intersection
        float precision = 1000;
        float3 B = position;
        float3 A = old_position;
        float3 m = B - A;
        
        //float2 disk_0 = (float2)(- m_pScene->blackhole.m_radius * ACCREATION_DISK_COEF, -m_pScene->blackhole.m_radius * ACCREATION_DISK_COEF);
        
        
        
        float t = - A.x / m.x;
        if ((t >= 0) && (t <=1)) {
            float3 cur_point = A + m * t;
            if (fabs(cur_point).x < precision) {
                if (length(cur_point) < blackholeRadius * diskCoef) {
                    
                    if ((intersect1isset) && (!intersect2isset)) {
                        intersect2isset = 1;
                        intersect2 = cur_point;
                    } else if (!intersect1isset) {
                        intersect1isset = 1;
                        intersect1 = cur_point;
                    } else if (intersect1isset && intersect2isset) {
                        result.intersect1 = intersect1;
                        result.intersect2 = intersect2;
                        result.intersect3 = cur_point;
                        result.type = 6;
                        return result;
                    }
                    
                    

                }
            }
        }
        
        
        
        //background
        if (cur_t > 1000) {
            Result result;
            if (intersect1isset && !intersect2isset) {
                result.type = 3;
                result.intersect1 = intersect1;
                result.intersect2 = normalize(velocity);
                result.intersect3 = (float3)(0,0,0);
            } else if ((!intersect1isset) && (!intersect2isset)) {
                result.type = 4;
                result.intersect1 = normalize(velocity);
                result.intersect2 = (float3)(0,0,0);
                result.intersect3 = (float3)(0,0,0);
                
            } else if (intersect1isset && intersect2isset) {
                result.type = 7;
                
                result.intersect1 = intersect1;
                result.intersect2 = intersect2;
                result.intersect3 = normalize(velocity);
            }
            //printf("Type %d", result.type);
            if (!result.type)
                result.type = 15;
            return result;
        }
    }
    
        
}

/*

vec3 CTracer::GetColorForPixel(int x, int y)
{
    vector<float3> colors;
    for (float offset = 0.2; offset < 1.0; offset += 0.2) {
        CLRay ray = MakeRay(x, y, offset);
        colors.push_back(TraceRay(ray));
    }
    
    vec3 mean_color = vec3(0.0);
    for (int i = 0; i < colors.size(); i++) {
        mean_color += colors[i];
    }
    mean_color /= float(colors.size());
    return mean_color;
}
 */


__kernel void TestKernel(
                        __global const int* y,
                        __global const int* x,
                        __global Result* results,
                        int iNumElements,
                         __global CLCamera *camera,
                         __global CLSettings *settings)
{

    
    
    // get index into global data array
    int iGID = get_global_id(0);
    // bound check (equivalent to the limit on a 'for' loop for standard/serial C code
    if (iGID >= iNumElements)
    {
        return;
    }

    results[iGID] = TraceRay(MakeRay(x[iGID],y[iGID], 0.5, camera), settings);
    
    
    
}