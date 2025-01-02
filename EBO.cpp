#include "EBO.h"

#include <iostream>

EBO::EBO(){
	glGenBuffers(1, &ID);
	
}
EBO::EBO(std::vector<GLuint>& indices) {
	glGenBuffers(1, &ID);
	this->Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_DYNAMIC_DRAW);	

}
void EBO::bufferData(std::vector<GLuint>& indices)
{
	this->Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_DYNAMIC_DRAW);
}


void EBO::Bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}
void EBO::Unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void EBO::Delete() {
	glDeleteBuffers(1, &ID);
}