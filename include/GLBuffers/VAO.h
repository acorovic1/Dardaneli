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

	// disable copy and move
	VAO(const VAO&) = delete;
	VAO& operator=(const VAO&) = delete;
	// move constructor
	VAO(VAO&& other) noexcept : ID(other.ID) {other.ID = 0;}

	// move assignment
	VAO& operator=(VAO&& other) noexcept {
		if (this != &other) {
			if (ID) glDeleteVertexArrays(1, &ID); // delete old object
			ID = other.ID;                          // take ownership
			other.ID = 0;                           // prevent double-delete
		}
		return *this;
	}

	void linkAttribute(VBO VBO, GLuint layout, GLint componentNumber, GLenum type,
		GLsizei stride, const GLvoid* pointer);

	void bind();
	void unbind();
	void Delete();

	void generate();

	GLuint getID() { return ID; }
};