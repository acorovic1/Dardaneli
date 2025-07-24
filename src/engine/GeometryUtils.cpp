#include "GeometryUtils.h"
#include "glm/gtx/vector_angle.hpp"
#include "iostream"

glm::vec3 GeometryUtils::bestSlideDirection(std::vector<glm::vec3> directions, std::vector<glm::vec2> unProjectedDirections, glm::vec2 mainDir)
{
     mainDir = glm::normalize(mainDir);

    float bestDot = -std::numeric_limits<float>::infinity();
    int returnIndex = 0;

    for (int i = 0; i < unProjectedDirections.size(); ++i)
    {
        float dot = glm::dot(mainDir, unProjectedDirections[i]); // ranges from -1 to 1

        if (dot > bestDot)
        {
            bestDot = dot;
            returnIndex = i;
        }
    }

    // If the best direction is opposite, invert it.
    //std::cout << "\n\n\t bestDot " << bestDot;
    if (bestDot < 0.0f)
    {
        //std::cout << "\n\n\tDIRECTION REVERSED";
        return -directions[returnIndex];
    }
    else
        return directions[returnIndex];
}