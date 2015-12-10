#include "common/RayTracer.h"
#include <string>

#define ASSIGNMENT 8
#if ASSIGNMENT == 5
#define APPLICATION Assignment5
#include "assignment5/Assignment5.h"
#elif ASSIGNMENT == 6
#define APPLICATION Assignment6
#include "assignment6/Assignment6.h"
#elif ASSIGNMENT == 7
#define APPLICATION Assignment7
#include "assignment7/Assignment7.h"
#elif ASSIGNMENT == 8
#define APPLICATION Assignment8
#include "assignment8/Assignment8.h"
#endif

#ifdef _WIN32
#define WAIT_ON_EXIT 1
#else
#define WAIT_ON_EXIT 0
#endif

int main(int argc, char** argv)  
{
    std::vector <std::string> coords;
    std::string filename = "_full";
    if (argc == 5) {
        for (int i = 1; i < argc; ++i) {
            coords.push_back(argv[i]);
        }
        filename = std::string("_") + argv[1] + std::string("_") + argv[2];
    }
    
    std::unique_ptr<APPLICATION> currentApplication = make_unique<APPLICATION>();
    currentApplication->SetOutputFilename("output" + filename + ".png");
    RayTracer rayTracer(std::move(currentApplication));

    DIAGNOSTICS_TIMER(timer, "Ray Tracer");
    rayTracer.Run(coords);
    DIAGNOSTICS_END_TIMER(timer);

    DIAGNOSTICS_PRINT();

#if defined(_WIN32) && WAIT_ON_EXIT
    int exit = 0;
    std::cin >> exit;
#endif

    return 0;
}