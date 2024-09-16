#pragma once

#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"

class VAO {

public:
	GLuint ID;

	VAO();

	void LinkAttribute(VBO VBO,GLuint layout, GLint componentNumber, GLenum type,
		GLsizei stride, const GLvoid* pointer);

	void Bind();
	void Unbind();
	void Delete();



};