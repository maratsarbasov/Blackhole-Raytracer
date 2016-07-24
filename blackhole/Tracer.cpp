#include "Tracer.h"
#include "FreeImagePlus.h"
#include "Constants.h"
#include "glm/gtx/intersect.hpp"
#include <iostream>
#include "cl.hpp"
#include <OpenCL/opencl.h>
#include <fstream>
#include <sstream>
#include <float.h>


typedef struct
{
    int start;
    cl_float3 m_start;
    cl_float3 m_dir;
} CLRay;





using namespace glm;



/*
double norm(dvec3 vec)
{
    return sqrt(dot(vec, vec));
}

SRay CTracer::MakeRay(glm::uvec2 pixelPos, float offset)
{
    SRay ray;
    ray.m_start = m_camera.m_pos;
    ray.m_dir = m_camera.m_forward + ((pixelPos.x + offset)/m_camera.m_resolution.x - 0.5) * m_camera.m_right + ((pixelPos.y + offset)/m_camera.m_resolution.y - 0.5) * m_camera.m_up; //big formula
    
    return ray;
}



glm::vec3 CTracer::TraceRay(SRay ray)
{
    bool end = false;
    const double delta_t = 5;
    dvec3 position = ray.m_start;
    double cur_t = 0;
    dvec3 velocity = normalize(ray.m_dir) * PHYSICS_C;
    
    float alpha = 0.0;
    vec3 disk_color = vec3(0.0);
    float diskCoef = settings->diskCoef;
    while (!end) {
        dvec3 r = -position + m_pScene->blackhole.m_position;
        dvec3 acceleration = r * PHYSICS_G * m_pScene->blackhole.m_mass / pow(norm(r), 3);
        
        dvec3 old_position = position;
        position += velocity * delta_t + acceleration * pow(delta_t, 2) / 2.0;
        velocity += acceleration * delta_t;
        cur_t += delta_t;

        //sphere intersection
        if (distance(position, m_pScene->blackhole.m_position) < m_pScene->blackhole.m_radius) {
            return alpha*disk_color + (1- alpha) * vec3(0,0,0);
        }
        
        //disk intersection
        double precision = 100;
        dvec3 B = position;
        dvec3 A = old_position;
        dvec3 m = B - A;
        
        dvec2 disk_0 = dvec2(- m_pScene->blackhole.m_radius * diskCoef, -m_pScene->blackhole.m_radius * diskCoef);
        
        
        
        double t = - A.x / m.x;
        if ((t >= 0) && (t <=1)) {
            dvec3 cur_point = A + m * t;
            if (abs((cur_point).x) < precision) {
                if (distance(cur_point, dvec3(0,0,0)) < m_pScene->blackhole.m_radius * diskCoef) {
                    dvec2 disk_point = dvec2((cur_point.y - disk_0.x) / m_pScene->blackhole.m_radius / diskCoef / 2, (cur_point.z - disk_0.y) / m_pScene->blackhole.m_radius / diskCoef / 2);
                    int pixel_y = trunc(disk_point.x * disk->getWidth());
                    int pixel_x = trunc(disk_point.y * disk->getHeight());
                    
                    RGBQUAD *pixel = new RGBQUAD;
                    
                    if (disk->getPixelColor(pixel_x, pixel_y, pixel)) {
                        
                        alpha = pixel->rgbReserved / 255.0;
                        disk_color = vec3(pixel->rgbRed  / 255.0, pixel->rgbGreen/ 255.0, pixel->rgbBlue / 255.0);
                        //return vec3(1,1,1);
                        
                    }
                    
                }
            }
        }
        
        
        
        //no intersection
        if (cur_t > 2000) {
            dvec3 normalized_velocity = normalize(velocity);
            double phi = atan2(normalized_velocity.x, normalized_velocity.y);
            double eta = asin(normalized_velocity.z);
            int stars_x = int(trunc((phi + M_PI) / (2 * M_PI) * stars->getWidth()));
            int stars_y = int(trunc((eta + M_PI_2) / M_PI * stars->getHeight()));
            RGBQUAD color;
            if  (stars->getPixelColor(stars_x, stars_y, &color) == false) {
                throw "no pixel color loaded";
            }
            return alpha * disk_color + float(1.0 - alpha) * vec3( color.rgbRed/255.0,  color.rgbGreen/255.0, color.rgbBlue/255.0);
            
        }
    }
    vec3 color(0, 1, 1);
    return color;
}
 

vec3 CTracer::GetColorForPixel(int x, int y)
{
    std::vector<vec3> colors;
    for (float offset = 0.2; offset < 1.0; offset += 0.2) {
        SRay ray = MakeRay(uvec2(x,y), offset);
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

std::tuple<vec3, float> CTracer::GetDiskColorByPosition(vec3 position)
{
    dvec2 disk_0 = dvec2(- settings->blackholeRadius * settings->diskCoef, -settings->blackholeRadius * settings->diskCoef);
    dvec2 disk_point = dvec2((position.y - disk_0.x) / (settings->blackholeRadius * settings->diskCoef * 2), (position.z - disk_0.y) / (settings->blackholeRadius * settings->diskCoef * 2));
    int pixel_y = trunc(disk_point.x * disk->getWidth());
    int pixel_x = trunc(disk_point.y * disk->getHeight());
    
    RGBQUAD *pixel = new RGBQUAD;
    
    if (disk->getPixelColor(pixel_x, pixel_y, pixel)) {
        
        float alpha = pixel->rgbReserved / 255.0;
        vec3 disk_color = vec3(pixel->rgbRed  / 255.0, pixel->rgbGreen/ 255.0, pixel->rgbBlue / 255.0);
        return std::make_tuple(disk_color, alpha);
    } else {
        throw "can't get pixel";
    }
    
    
}

vec3 CTracer::GetBackgroundColorByVector(vec3 velocity)
{
    
    double phi = atan2(velocity.x, velocity.y);
    double eta = asin(velocity.z);
    int stars_x = int(trunc((phi + M_PI) / (2 * M_PI) * stars->getWidth()));
    int stars_y = int(trunc((eta + M_PI_2) / M_PI * stars->getHeight()));
    RGBQUAD rgbquadcolor;
    if  (stars->getPixelColor(stars_x, stars_y, &rgbquadcolor) == false) {
        throw "no pixel color loaded";
    }
    return vec3(rgbquadcolor.rgbRed/255.0, rgbquadcolor.rgbGreen/255.0, rgbquadcolor.rgbBlue/255.0);
}

CLCamera *CLCameraFromSCamera(SCamera m_camera)
{
    CLCamera *camera = new CLCamera;
    camera->m_pos.x = m_camera.m_pos.x;
    camera->m_pos.y = m_camera.m_pos.y;
    camera->m_pos.z = m_camera.m_pos.z;
    camera->m_forward.x = m_camera.m_forward.x;
    camera->m_forward.y = m_camera.m_forward.y;
    camera->m_forward.z = m_camera.m_forward.z;
    camera->m_up.x = m_camera.m_up.x;
    camera->m_up.y = m_camera.m_up.y;
    camera->m_up.z = m_camera.m_up.z;
    camera->m_right.x = m_camera.m_right.x;
    camera->m_right.y = m_camera.m_right.y;
    camera->m_right.z = m_camera.m_right.z;
    camera->m_resolution.x = m_camera.m_resolution.x;
    camera->m_resolution.y = m_camera.m_resolution.y;
    
    return camera;
}

CLSettings *CLSettingsFromSettings(Settings &settings)
{
    CLSettings *clSettings = new CLSettings;
    clSettings->cameraDistance = settings.cameraDistance;
    clSettings->blackholeRadius = settings.blackholeRadius;
    clSettings->blackholeMass = settings.blackholeMass;
    clSettings->diskCoef = settings.diskCoef;

    return clSettings;
}

void CTracer::setCamera(void)
{
    m_camera.m_viewAngle = vec2(settings->xViewAngle, settings->xViewAngle * settings->yRes / settings->xRes);
    m_camera.m_pos = dvec3(settings->cameraX , settings->cameraY , settings->cameraZ);
    m_camera.m_resolution = uvec2(settings->xRes, settings->yRes);
    m_camera.m_pixels.resize(settings->xRes * settings->yRes);
    
    m_camera.m_forward = normalize(- m_camera.m_pos);
    m_camera.m_up = glm::dvec3(m_camera.m_forward.y, -m_camera.m_forward.x, m_camera.m_forward.z);
    if (m_camera.m_up.x < 0)
        m_camera.m_up *= -1;
    m_camera.m_right = normalize(cross(m_camera.m_forward, m_camera.m_up));
    
    m_camera.m_up *= settings->yRes;
    m_camera.m_right *= settings->xRes;
    m_camera.m_forward *= m_camera.m_resolution.x / (2 * tan(m_camera.m_viewAngle.x / 2));
}

vec3 CTracer::GetColorByResult(Result result)
{
    vec3 color;
    
    if (result.type == 1) {
        // disk, blackhole
        
        vec3 pos = vec3(result.intersect1.x, result.intersect1.y, result.intersect1.z);
        std::tuple<vec3, float> diskcolor = GetDiskColorByPosition(pos);
        float alpha = std::get<1>(diskcolor);
        color = alpha * std::get<0>(diskcolor) + (1 - alpha) * vec3(0);
    }else if (result.type == 2) {
        // blackhole
        
        color = vec3(0,0,0);
    } else if (result.type == 3) {
        // disk, background
        
        vec3 pos = vec3(result.intersect1.x, result.intersect1.y, result.intersect1.z);
        std::tuple<vec3, float> diskcolor = GetDiskColorByPosition(pos);
        vec3 velocity = vec3(result.intersect2.x, result.intersect2.y, result.intersect2.z);
        vec3 backcolor = GetBackgroundColorByVector(velocity);
        float alpha = std::get<1>(diskcolor);
        color = alpha * std::get<0>(diskcolor) + (1 - alpha) * backcolor;
    } else if (result.type ==4) {
        // background
        
        
        color = GetBackgroundColorByVector(vec3(result.intersect1.x, result.intersect1.y, result.intersect1.z));
    } else if (result.type == 5) {
        
        // disk, disk, blackhole
        
        vec3 pos1 = vec3(result.intersect1.x, result.intersect1.y, result.intersect1.z);
        std::tuple<vec3, float> diskcolor1 = GetDiskColorByPosition(pos1);
        vec3 pos2 = vec3(result.intersect2.x, result.intersect2.y, result.intersect2.z);
        std::tuple<vec3, float> diskcolor2 = GetDiskColorByPosition(pos2);
        
        float alpha1 = std::get<1>(diskcolor1);
        float alpha2 = std::get<1>(diskcolor2);
        color = alpha1 * std::get<0>(diskcolor1) + alpha2 * std::get<0>(diskcolor2) + (1 - alpha1 - alpha2) * vec3(0);
        
    } else if (result.type == 6) {
        vec3 pos1 = vec3(result.intersect1.x, result.intersect1.y, result.intersect1.z);
        std::tuple<vec3, float> diskcolor1 = GetDiskColorByPosition(pos1);
        vec3 pos2 = vec3(result.intersect2.x, result.intersect2.y, result.intersect2.z);
        std::tuple<vec3, float> diskcolor2 = GetDiskColorByPosition(pos2);
        vec3 pos3 = vec3(result.intersect3.x, result.intersect3.y, result.intersect3.z);
        std::tuple<vec3, float> diskcolor3 = GetDiskColorByPosition(pos3);
        
        float alpha1 = std::get<1>(diskcolor1);
        float alpha2 = std::get<1>(diskcolor2);
        //float alpha3 = std::get<1>(diskcolor3);
        color = alpha1 * std::get<0>(diskcolor1) + alpha2 * std::get<0>(diskcolor2) + (1 - alpha1 - alpha2) * std::get<0>(diskcolor3);
        // disk, disk, disk
    } else if (result.type == 7) {
        // disk, disk, background
        
        vec3 pos1 = vec3(result.intersect1.x, result.intersect1.y, result.intersect1.z);
        std::tuple<vec3, float> diskcolor1 = GetDiskColorByPosition(pos1);
        vec3 pos2 = vec3(result.intersect2.x, result.intersect2.y, result.intersect2.z);
        std::tuple<vec3, float> diskcolor2 = GetDiskColorByPosition(pos2);
        vec3 velocity = vec3(result.intersect3.x, result.intersect3.y, result.intersect3.z);
        vec3 backcolor = GetBackgroundColorByVector(velocity);
        float alpha1 = std::get<1>(diskcolor1);
        float alpha2 = std::get<1>(diskcolor2);
        color = alpha1 * std::get<0>(diskcolor1) + alpha2 * std::get<0>(diskcolor2) + (1 - alpha1 - alpha2) * backcolor;
    }
    
    
    
    
    else {
        throw "Error getting result";
    }
    
    return color;
}

void CTracer::RenderImage()
{

    int xRes = settings->xRes;
    int yRes = settings->yRes;
    stars = LoadImageFromFile("data/stars.jpg");
    disk = LoadImageFromFile("data/disk_32.png");
    
    setCamera();
    
    
    int DATA_SIZE = xRes * yRes;
    
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    
    std::vector<cl::Device> devices;
    platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);
    
    
    //select device here!
    cl::Device device = devices[1];
    std::cout << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    
    
    std::vector<cl::Device> contextDevices;
    contextDevices.push_back(device);
    cl::Context context(contextDevices);
    cl::CommandQueue queue(context, device);
    
    //Create memory buffers
    int *pInputVector1 = (int *)malloc(DATA_SIZE * sizeof(int));
    int *pInputVector2 = (int*)malloc(DATA_SIZE*sizeof(int));
    Result *pOutputVector = (Result *)malloc(DATA_SIZE * sizeof(Result));

    
    for (int i = 0; i < yRes; i++) {
        for (int j = 0; j < xRes; j++) {
            pInputVector1[i*xRes + j] = i;
            pInputVector2[i*xRes + j] = j;
        }
    }
    
    cl_int error;
    cl::Buffer clmInputVector1 = cl::Buffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, DATA_SIZE * sizeof(int), pInputVector1, &error);
    if (error) {
        throw error;
    }
    cl::Buffer clmInputVector2 = cl::Buffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, DATA_SIZE * sizeof(int), pInputVector2, &error);
    if (error) {
        throw error;
    }
    cl::Buffer clmOutputVector = cl::Buffer(context, CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR, DATA_SIZE * sizeof(Result), pOutputVector, &error);
    if (error) {
        throw error;
    }
    
    CLCamera *clCamera = CLCameraFromSCamera(m_camera);
    cl::Buffer clmCamera = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(*clCamera), clCamera);
    CLSettings *clSettings = CLSettingsFromSettings(*settings);
    cl::Buffer clmSettings = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(*clSettings), clSettings);
    
    std::ifstream t("kernel.cl");
    std::stringstream buffer;
    buffer <<  t.rdbuf();
    
    std::string sourceCode = buffer.str();
    //sourceCode = "#define BLACKHOLE_RADIUS " + m_pScene->blackhole.m_radius
    cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
    cl::Program program = cl::Program(context, source);
    
    int buildRes = program.build(contextDevices);
    if (buildRes != CL_SUCCESS){
        throw "build error";
    }
    cl::Kernel kernel(program, "TestKernel");
    
    
    
    
    int iArg = 0;
    
    kernel.setArg(iArg++, clmInputVector1);
    kernel.setArg(iArg++, clmInputVector2);
    kernel.setArg(iArg++, clmOutputVector);
    kernel.setArg(iArg++, DATA_SIZE);
    kernel.setArg(iArg++, clmCamera);
    kernel.setArg(iArg++, clmSettings);

    
    
    
    int res = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(DATA_SIZE), cl::NullRange);
    if (res != CL_SUCCESS)
        throw "was not run";
    error = queue.finish();
    if (0 != error) {
        throw "erro";
    }
    
    int readRes = queue.enqueueReadBuffer(clmOutputVector, CL_TRUE, 0, DATA_SIZE * sizeof(Result), pOutputVector);
    
    if (readRes != 0) {
        throw "can't read result";
    }
    
    
    for(int i = 0; i < yRes; i++)
        for(int j = 0; j < xRes; j++)
        {
            Result result = pOutputVector[i*xRes + j];
            
            
            
            m_camera.m_pixels[i*xRes + j] = GetColorByResult(result);
        }
    
    
    
    /*
    
    for(int i = 0; i < yRes; i++) {
        std::cout << i << std::endl;
        for(int j = 0; j < xRes; j++)
        {
            SRay ray = MakeRay(uvec2(j,i), 0.5);
            m_camera.m_pixels[i * xRes + j] = TraceRay(ray);
        }
    }
    */
}

void CTracer::SaveImageToFile(std::string fileName)
{
    
    
    int width = m_camera.m_resolution.x;
    int height = m_camera.m_resolution.y;
    
    fipImage *image = new fipImage(FIT_BITMAP, width, height, 24);
    
    int pitch = image->getScanWidth();
    unsigned char* imageBuffer = (unsigned char*)image->accessPixels();
    
    if (pitch < 0)
    {
        imageBuffer += pitch * (height - 1);
        pitch = - pitch;
    }
    
    int i, j;
    int imageDisplacement = 0;
    int textureDisplacement = 0;
    
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            vec3 color = m_camera.m_pixels[textureDisplacement + j];
            
            imageBuffer[imageDisplacement + j * 3] = clamp(color.b, 0.0f, 1.0f) * 255.0f;
            imageBuffer[imageDisplacement + j * 3 + 1] = clamp(color.g, 0.0f, 1.0f) * 255.0f;
            imageBuffer[imageDisplacement + j * 3 + 2] = clamp(color.r, 0.0f, 1.0f) * 255.0f;
        }
        
        imageDisplacement += pitch;
        textureDisplacement += width;
    }
    
    image->save("result.png");
    delete image;
}

fipImage* CTracer::LoadImageFromFile(std::string fileName)
{
    fipImage *image = new fipImage;
    image->load(fileName.c_str());

    
    return image;
}



