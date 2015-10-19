#include "common/Acceleration/Naive/NaiveAcceleration.h"
#include "common/Scene/Geometry/Ray/Ray.h"
#include "common/Intersection/IntersectionState.h"

bool NaiveAcceleration::Trace(Ray* inputRay, IntersectionState* outputIntersection) const
{
    std::vector<IntersectionState> allIntersections;
    for (size_t i = 0; i < nodes.size(); ++i) {
        IntersectionState newIntersection;
        newIntersection.TestAndCopyLimits(outputIntersection);

        bool hit = nodes[i]->Trace(inputRay, &newIntersection);
        // early exit when we just want to know whether or not we hit.
        if (hit && !outputIntersection) {
            return true; 
        }

        if (!hit) {
            continue;
        }

        allIntersections.push_back(newIntersection);
    }

    if (allIntersections.empty()) {
        return false;
    }

    // Find the closest intersection and store that into outputIntersection
    if (outputIntersection) {
        size_t closestIntersection = 0;
        float minT = std::numeric_limits<float>::max();
        for (size_t i = 0; i < allIntersections.size(); ++i) {
            if (allIntersections[i].intersectionT - minT < EPSILON) {
                minT = allIntersections[i].intersectionT;
                closestIntersection = i;
            }
        }

        IntersectionState intersection = allIntersections[closestIntersection];
        outputIntersection->intersectedPrimitive = intersection.intersectedPrimitive;
        outputIntersection->intersectionRay = intersection.intersectionRay;
        outputIntersection->intersectionT = intersection.intersectionT;
        outputIntersection->hasIntersection = intersection.hasIntersection;
    }
    
    return true;
}