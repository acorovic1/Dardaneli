#include "ShadingNodes/Texture/Texture.h"


#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

std::string getExecutablePathh() {
#if defined(_WIN32)
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	return std::string(buffer);
#elif defined(__linux__)
	char buffer[1024];
	ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	if (len == -1) throw std::runtime_error("Cannot get executable path");
	buffer[len] = '\0';
	return std::string(buffer);
#elif defined(__APPLE__)
	char buffer[1024];
	uint32_t size = sizeof(buffer);
	if (_NSGetExecutablePath(buffer, &size) != 0)
		throw std::runtime_error("Cannot get executable path");
	return std::string(buffer);
#else
	throw std::runtime_error("Unsupported platform");
#endif
}

// Remove last N path components to get project directory
std::string getProjectDirr(int levelsUp = 3) {
	std::string path = getExecutablePathh();
	for (int i = 0; i < levelsUp; ++i) {
		auto pos = path.find_last_of("/\\");
		if (pos == std::string::npos) break;
		path = path.substr(0, pos);
	}
	return path;
}



Texture::Texture(const char* image,
	GLuint slot, GLenum pixelType) {

	int imgWidth, imgHeight, numColorChannels;

	std::string projectDir = getProjectDirr();

	std::string imagePath = projectDir + "/assets/" + image;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(imagePath.c_str(), &imgWidth, &imgHeight, &numColorChannels, 0);
	if (!data) {
		std::cerr << "Failed to load texture: " << imagePath << "\n";
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