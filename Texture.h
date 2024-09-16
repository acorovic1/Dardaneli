#pragma once

#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>
#include "Shader.h"


class Texture {
public:
	GLuint ID;
	GLuint unit; // GL_TEXTURE0 + unit
	const char* type; // diffuse, specular, etc...

	Texture(const char* image, const char* textureType, GLuint slot, GLenum format, GLenum pixelType);
	void textureUniform(Shader& shader, const char* uniform, GLuint unit);

	void Bind();
	void Unbind();
	void Delete();
};