#include "VAO.h"

VAO::~VAO()
{
	unbind();
	Delete();
}

VAO::VAO() {
	glGenVertexArrays(1, &ID);
}

void VAO::bind() {
	glBindVertexArray(ID);
}

void VAO::unbind() {
	glBindVertexArray(0);
}

void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}

void VAO::generate()
{
	if (glIsVertexArray(ID))return;
	glGenVertexArrays(1, &ID);
}

void VAO::linkAttribute(VBO VBO, GLuint layout, GLint componentNumber, GLenum type,
	GLsizei stride, const GLvoid* pointer) {
	VBO.bind();
	glVertexAttribPointer(layout, componentNumber, type, GL_FALSE, stride, pointer); // points to the data
	glEnableVertexAttribArray(layout); // enables the layout
	VBO.unbind();
}