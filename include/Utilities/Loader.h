#pragma once
#include <vector>
#include "glm/glm.hpp"
#include <glad/glad.h>
#include "Mesh/DVertex.h"
namespace Loader
{
	bool obj(const char* file, std::vector<DVertex*>& vertices, std::vector<GLuint>& indices, std::vector<GLuint>& edgeIndices);

}