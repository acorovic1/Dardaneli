#include "Texture.h"

Texture::Texture(const char* image,
	GLuint slot, GLenum format, GLenum pixelType) {

	int imgWidth, imgHeight, numColorChannels;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(image, &imgWidth, &imgHeight, &numColorChannels, 0);
	if (!data) {
		std::cerr << "Failed to load texture: " << image << "\n";
		return;
	}
	format = (numColorChannels == 4) ? GL_RGBA : GL_RGB;
	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(GL_TEXTURE_2D, ID);

	//Configures the algorithm that makes the image smaller or bigger based on distance
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//How does the texture repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, format, pixelType, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);


	std::cout << "\n\n Texture created successfully with ID: " << ID;
}

void Texture::textureUniform(Shader& shader, const char* uniform, GLuint unit) {
	shader.setInteger(false, uniform, unit);
}

void Texture::Bind()
{
	//std::cout << "Texture ID: " << ID << " unit: " << unit << std::endl;

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}