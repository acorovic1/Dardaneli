#pragma once

#include "glm/glm.hpp"
#include <vector>

namespace GeometryUtils {


	glm::vec3 bestSlideDirection(std::vector<glm::vec3> directions, std::vector<glm::vec2> unProjectedDirections, glm::vec2 mainDir);

}