#include "Tracer.h"
#include "stdio.h"
#include "cl.hpp"
#include <iostream>
#include <FreeImage.h>
#include "Types.h"

int main(int argc, char** argv)
{
    CTracer tracer;
    CScene scene;
    
    int xRes = 1024;  // Default resolution
    int yRes = 768;
    
    Settings *settings;
    
    float xViewAngle = 2;
    float blackholeMass = 8.57e+36;
    float diskCoef = 8;
    float cameraX = 11e+10;
    float cameraY = 0;
    float cameraZ = 0;
    
    if(argc == 2) // There is input file in parameters
    {
        FILE* file = fopen(argv[1], "r");
        if(file)
        {
            int xResFromFile = 0;
            int yResFromFile = 0;
            float xViewAngleFromFile = 0;
            float cameraXFromFile = 0;
            float cameraYFromFile = 0;
            float cameraZFromFile = 0;
            float blackholeMassFromFile = 0;
            float diskCoefFromFile = 0;
            if(fscanf(file,
                      "%d %d %f %f %f %f %f %f",
                      &xResFromFile,
                      &yResFromFile,
                      &xViewAngleFromFile,
                      &cameraXFromFile,
                      &cameraYFromFile,
                      &cameraZFromFile,
                      &blackholeMassFromFile,
                      &diskCoefFromFile) == 8)
            {
                xRes = xResFromFile;
                yRes = yResFromFile;
                settings = new Settings(xResFromFile,
                                    yResFromFile,
                                    xViewAngleFromFile,
                                    cameraXFromFile,
                                    cameraYFromFile,
                                    cameraZFromFile,
                                    blackholeMassFromFile,
                                    diskCoefFromFile);
                
            }
            else {
                settings = new Settings(xRes,
                                        yRes,
                                        xViewAngle,
                                        cameraX,
                                        cameraY,
                                        cameraZ,
                                        blackholeMass,
                                        diskCoef);
                printf("Invalid config format! Using default parameters.\r\n");
            }
            
            fclose(file);
        }
        else
            printf("Invalid config path! Using default parameters.\r\n");
    }
    else
        printf("No config! Using default parameters.\r\n");
    
    tracer.m_pScene = &scene;
    tracer.settings = settings;
    try {
        tracer.RenderImage();

    } catch (char const *a) {
        std::cout << "Error: " << a << std::endl;
        return -1;
    } catch (int a) {
        std::cout << "Error # " << a << std::endl;
        return -1;
    }
    tracer.SaveImageToFile("Result.png");
    
}
