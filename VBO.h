#pragma once



#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Vertex.h"



class VBO {

public:
	GLuint ID;
	VBO();
	VBO(std::vector<Vertex>&vertices);
	VBO(std::vector<glm::vec2>& vertices);

	void bufferData(std::vector<Vertex>& vertices);
	void Bind();
	void Unbind();
	void Delete();



};