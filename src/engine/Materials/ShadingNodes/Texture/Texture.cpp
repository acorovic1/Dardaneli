#include "ShadingNodes/Texture/Texture.h"





Texture::Texture(const char* image,
	GLuint slot, GLenum pixelType) {

	int imgWidth, imgHeight, numColorChannels;

	std::string projectDir = FileSystem::getProjectDir();


	// this was used for PRS RT 
	//std::string imagePath = projectDir + "/assets/" + image;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(image, &imgWidth, &imgHeight, &numColorChannels, 0);
	if (!data) {
		std::cerr << "Failed to load texture: " << image << "\n";
		return;
	}
	GLenum format;
	if (numColorChannels == 4)
		format = GL_RGBA;
	else if (numColorChannels == 3)
		format = GL_RGB;
	else if (numColorChannels == 1)
		format = GL_RED;
	else std::cout << "\n\n\tInvalid number of color channels\n\n\n";
	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(GL_TEXTURE_2D, ID);

	//Configures the algorithm that makes the image smaller or bigger based on distance
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

void Texture::bind()
{
	//std::cout << "Texture ID: " << ID << " unit: " << unit << std::endl;

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind(){	glBindTexture(GL_TEXTURE_2D, 0);}

void Texture::Delete(){	glDeleteTextures(1, &ID);}