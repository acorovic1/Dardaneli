#include "VBO.h"

VBO::VBO() {
	glGenBuffers(1, &ID);
}

VBO::VBO(std::vector<DVertex>& vertices) {
	glGenBuffers(1, &ID);
	this->bind();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(DVertex), vertices.data(), GL_DYNAMIC_DRAW);
}
VBO::VBO(std::vector<glm::vec2>& vertices) {
	glGenBuffers(1, &ID);
	this->bind();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_DYNAMIC_DRAW);
}
void VBO::bufferData(std::vector<DVertex*>& vertices)
{
	std::vector<DVertex> vboVerts;
	for(auto x:vertices)
		vboVerts.push_back(*x);

	this->bind();
	glBufferData(GL_ARRAY_BUFFER, vboVerts.size() * sizeof(DVertex), vboVerts.data(), GL_DYNAMIC_DRAW);
}
void VBO::bufferData(std::vector<GPUVertex>& vertices)
{


	this->bind();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GPUVertex), vertices.data(), GL_DYNAMIC_DRAW);
}

void VBO::bufferData(float* vertices, int size)
{
	this->bind();
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices, GL_DYNAMIC_DRAW);
}

void VBO::bufferData(std::vector<glm::vec3>& vertices)
{
	this->bind();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
}

void VBO::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}
void VBO::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void VBO::Delete() {
	glDeleteBuffers(1, &ID);
}