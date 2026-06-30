#pragma once

#include <glad/glad.h>
#include <stb/stb_image.h>
#include "Shader.h"
#include "Utilities/FileSystem.h"


enum class TextureType {
	DIFFUSE,
	SPECULAR,
	NORMAL,
	HEIGHT
};

struct Texture {

	GLuint ID;
	GLuint unit; // GL_TEXTURE0 + unit
	

	Texture(const char* image, GLuint slot, GLenum pixelType=GL_UNSIGNED_BYTE);
	~Texture() {
		Delete();
	}

	void textureUniform(Shader& shader, const char* uniform, GLuint unit);

	void bind();
	void unbind();
	void Delete();
};