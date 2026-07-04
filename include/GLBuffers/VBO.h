#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Mesh/DVertex.h"
#include "Mesh/GPUVertex.h"

class VBO {
public:
	GLuint ID;
	VBO();
	VBO(std::vector<DVertex>& vertices);
	VBO(std::vector<glm::vec2>& vertices);

	void bufferData(std::vector<DVertex*>& vertices);
	void bufferData(std::vector<GPUVertex>& vertices);
	void bufferData(float* vertices,int size);
	void bufferData(std::vector<glm::vec3>& vertices);
	void bind();
	void unbind();
	void Delete();
};