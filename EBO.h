#pragma once
#define EBO_CLASS_H

#include <glad/glad.h>
#include <vector>

#include "VBO.h"

class EBO {
public:
	GLuint ID;
	EBO(std::vector<GLuint>&indices);
	void Bind();
	void Unbind();
	void Delete();

};