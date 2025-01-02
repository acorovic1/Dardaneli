#pragma once


#include <glad/glad.h>
#include <vector>

#include "VBO.h"

class EBO {
public:
	GLuint ID;
	EBO();
	EBO(std::vector<GLuint>&indices);
	void bufferData(std::vector<GLuint>& indices);


	void Bind();
	void Unbind();
	void Delete();

};