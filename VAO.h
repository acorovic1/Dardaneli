#pragma once



#include <glad/glad.h>
#include "VBO.h"

class VAO {

	GLuint ID;
public:
	~VAO();
	VAO();
	/*VAO(const VAO&vao);
	VAO operator=(const VAO& vao);*/

	void LinkAttribute(VBO VBO,GLuint layout, GLint componentNumber, GLenum type,
		GLsizei stride, const GLvoid* pointer);

	void Bind();
	void Unbind();
	void Delete();

	void Generate();

	GLuint getID() { return ID; }



};