#pragma once

#include "common/common.h"

class RayTracer {
public:
    RayTracer(std::unique_ptr<class Application> app);

    void Run(std::vector <std::string> coords);
private:
    std::unique_ptr<class Application> storedApplication;
    int x;
    int y;
    int w;
    int h;
};