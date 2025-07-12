#include "VBO.h"

VBO::VBO() {
	glGenBuffers(1, &ID);
}

VBO::VBO(std::vector<DVertex>& vertices) {
	glGenBuffers(1, &ID);
	this->Bind();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(DVertex), vertices.data(), GL_DYNAMIC_DRAW);
}
VBO::VBO(std::vector<glm::vec2>& vertices) {
	glGenBuffers(1, &ID);
	this->Bind();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_DYNAMIC_DRAW);
}
void VBO::bufferData(std::vector<DVertex*>& vertices)
{
	std::vector<DVertex> gpuVertices;
	for(auto x:vertices)
		gpuVertices.push_back(*x);

	this->Bind();
	glBufferData(GL_ARRAY_BUFFER, gpuVertices.size() * sizeof(DVertex), gpuVertices.data(), GL_DYNAMIC_DRAW);
}

void VBO::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}
void VBO::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void VBO::Delete() {
	glDeleteBuffers(1, &ID);
}