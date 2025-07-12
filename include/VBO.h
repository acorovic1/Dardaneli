#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "DVertex.h"

class VBO {
public:
	GLuint ID;
	VBO();
	VBO(std::vector<DVertex>& vertices);
	VBO(std::vector<glm::vec2>& vertices);

	void bufferData(std::vector<DVertex*>& vertices);
	void Bind();
	void Unbind();
	void Delete();
};