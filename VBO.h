#pragma once

#define VBO_CLASS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Vertex.h"



class VBO {

public:
	GLuint ID;

	VBO(std::vector<Vertex>&vertices);

	void Bind();
	void Unbind();
	void Delete();



};