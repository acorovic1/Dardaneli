#pragma once
#include <glm/glm.hpp>



// used to render with materials assigned
struct GPUVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};