#pragma once

#include <glad/glad.h>
#include <stb/stb_image.h>
#include "Shader.h"


enum class TextureType {
	DIFFUSE,
	SPECULAR,
	NORMAL,
	HEIGHT
};

struct Texture {

	GLuint ID;
	GLuint unit; // GL_TEXTURE0 + unit
	TextureType type; // diffuse, specular, etc...

	Texture(const char* image, GLuint slot, GLenum format=GL_RGB, GLenum pixelType=GL_UNSIGNED_BYTE);
	~Texture() {
		Delete();
	}

	void textureUniform(Shader& shader, const char* uniform, GLuint unit);

	void Bind();
	void Unbind();
	void Delete();
};