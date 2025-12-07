#pragma once
#include "Mesh/DVertex.h"

#include "Ray.h"
#include "Triangle.h"


#include "glm/glm.hpp"
#include <vector>


namespace GeometryUtils {


	glm::vec3 bestSlideDirection(std::vector<glm::vec3> directions, std::vector<glm::vec2> unProjectedDirections, glm::vec2 mainDir);

	DVertex* findClosestVertex(std::vector<DVertex*>choose, DVertex* a, DVertex* b);


    // Assumes quadCorners is in loop order: [A, B, C, D].

    // If a and b lie on opposite edges (so they bisect the quad), this fills out1/out2
    // with the two resulting quads and returns true. Otherwise returns false.


    bool splitQuadAlongMidpointsOpposite(
        const std::vector<DVertex*>& quadCorners,
        DVertex* a,
        DVertex* b,
        std::vector<DVertex*>& out1,
        std::vector<DVertex*>& out2);


}