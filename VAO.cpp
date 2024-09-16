#include "VAO.h"


VAO::VAO() {
	glGenVertexArrays(1,&ID);
}

void VAO::Bind() {
	glBindVertexArray(ID);
}


void VAO :: Unbind() {
	glBindVertexArray(0);
}

void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}

void VAO::LinkAttribute(VBO VBO,GLuint layout, GLint componentNumber, GLenum type ,
	GLsizei stride, const GLvoid* pointer) {

	VBO.Bind();
	glVertexAttribPointer(layout, componentNumber, type, GL_FALSE, stride, pointer); // points to the data
	glEnableVertexAttribArray(layout); // enables the layout
	VBO.Unbind();

}

