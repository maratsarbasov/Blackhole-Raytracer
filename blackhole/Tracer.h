#pragma once

#include "glm/glm.hpp"
#include "Types.h"
#include "Scene.h"
#include <FreeImagePlus.h>

#include "string"


class CTracer
{
public:
    SRay MakeRay(glm::uvec2 pixelPos, float offset);  // Create ray for specified pixel
    glm::vec3 TraceRay(SRay ray); // Trace ray, compute its color
    void RenderImage();
    void SaveImageToFile(std::string fileName);
    fipImage* LoadImageFromFile(std::string fileName);    
public:
    SCamera m_camera;
    CScene* m_pScene;
    Settings *settings;
    void setCamera(void);
    
    
private:
    fipImage *stars;
    fipImage *disk;
    glm::vec3 GetColorForPixel(int x, int y);
    glm::vec3 GetBackgroundColorByVector(glm::vec3 velocity);
    std::tuple<glm::vec3, float> GetDiskColorByPosition(glm::vec3 position);
    glm::vec3 GetColorByResult(Result result);

    
    
};