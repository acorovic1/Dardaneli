#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "DVertex.h"
#include "GPUVertex.h"

class VBO {
public:
	GLuint ID;
	VBO();
	VBO(std::vector<DVertex>& vertices);
	VBO(std::vector<glm::vec2>& vertices);

	void bufferData(std::vector<DVertex*>& vertices);
	void bufferData(std::vector<GPUVertex>& vertices);
	void bufferData(float* vertices,int size);
	void Bind();
	void Unbind();
	void Delete();
};