#include "Shader.h"
#include <fstream>
#include <string>

#include "Utilities/FileSystem.h"






Shader::Shader(std::string name, const char* computeFile)
{
	std::string projectDir = FileSystem::getProjectDir();
	std::string computePath = projectDir + "/src/shaders/" + computeFile;
	std::string computeCode = FileSystem::getFileContents(computePath.c_str());
	const char* computeSource = computeCode.c_str();

	// Compile compute shader
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &computeSource, NULL);
	glCompileShader(computeShader);
	compileErrors(computeShader, "COMPUTE");

	// Link program
	ID = glCreateProgram();
	glAttachShader(ID, computeShader);
	glLinkProgram(ID);
	compileErrors(ID, "PROGRAM");

	glDeleteShader(computeShader);


	shaderSingleton->addShader(name, this);


}

Shader::Shader(std::string name, const char* vertexFile, const char* fragmentFile)
{
	std::string projectDir = FileSystem::getProjectDir();
	//std::cout << "\n ExeDir = " << projectDir << "\n";

	std::string vertexPath = projectDir + "/src/shaders/" + vertexFile;
	std::string fragmentPath = projectDir + "/src/shaders/" + fragmentFile;


	std::string vertexCode = FileSystem::getFileContents(vertexPath.c_str());
	const char* vertexSource = vertexCode.c_str();

	std::string fragmentCode;
	if (FileSystem::fileExists(fragmentPath.c_str())) {
		// Treat as file path
		fragmentCode = FileSystem::getFileContents(fragmentPath.c_str());
	}
	else {
		// Treat as raw GLSL code
		fragmentCode = fragmentFile;
	}
	const char* fragmentSource = fragmentCode.c_str();

	// Compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	compileErrors(vertexShader, "VERTEX");

	// Compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	compileErrors(fragmentShader, "FRAGMENT");

	// Link program
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	compileErrors(ID, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	shaderSingleton->addShader(name, this);
}

Shader::Shader(std::string name, const char* vertexFile, const char* fragmentFile, const char* geometryFile)
{
	std::string projectDir = FileSystem::getProjectDir();

	std::string vertexPath = projectDir + "/src/shaders/" + vertexFile;
	std::string fragmentPath = projectDir + "/src/shaders/" + fragmentFile;
	std::string geometryPath = projectDir + "/src/shaders/" + geometryFile;

	std::string vertexCode = FileSystem::getFileContents(vertexPath.c_str());
	std::string fragmentCode;

	if (FileSystem::fileExists(fragmentPath.c_str())) {
		// Treat as file path
		fragmentCode = FileSystem::getFileContents(fragmentPath.c_str());
	}
	else {
		// Treat as raw GLSL code
		fragmentCode = fragmentFile;
	}
	std::string geometryCode = FileSystem::getFileContents(geometryPath.c_str());

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();
	const char* geometrySource = geometryCode.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	compileErrors(vertexShader, "VERTEX");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	compileErrors(fragmentShader, "FRAGMENT");

	GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, &geometrySource, NULL);
	glCompileShader(geometryShader);
	compileErrors(geometryShader, "GEOMETRY");

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glAttachShader(ID, geometryShader);

	glLinkProgram(ID);

	compileErrors(ID, "PROGRAM");


	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);


}


void Shader::activate() {
	glUseProgram(ID);
}

void Shader::Delete() {
	shaderSingleton->deleteShader(this);
	glDeleteProgram(ID);
}

GLuint Shader::getID() { return ID; }

void Shader::setBool(bool activated, const char* uniform, bool value)
{
	if (!activated)
		this->activate();
	glUniform1i(glGetUniformLocation(this->ID, uniform), value);
}

void Shader::setFloat(bool activated, const char* uniform, float value)
{
	if (!activated)
		this->activate();
	glUniform1f(glGetUniformLocation(this->ID, uniform), value);
}
void Shader::setInteger(bool activated, const char* uniform, int value)
{
	if (!activated)
		this->activate();
	glUniform1i(glGetUniformLocation(this->ID, uniform), value);
}

void Shader::setVector2i(bool activated, const char* uniform, float x, float y)
{
	if (!activated)
		this->activate();
	glUniform2i(glGetUniformLocation(this->ID, uniform), x, y);
}
void Shader::setVector2i(bool activated, const char* uniform, const glm::vec2& vec)
{
	if (!activated)
		this->activate();
	glUniform2i(glGetUniformLocation(this->ID, uniform), vec.x, vec.y);
}

void Shader::setVector2f(bool activated, const char* uniform, float x, float y)
{
	if (!activated)
		this->activate();
	glUniform2f(glGetUniformLocation(this->ID, uniform), x, y);
}
void Shader::setVector2f(bool activated, const char* uniform, const glm::vec2& vec)
{
	if (!activated)
		this->activate();
	glUniform2f(glGetUniformLocation(this->ID, uniform), vec.x, vec.y);
}
void Shader::setVector3f(bool activated, const char* uniform, float x, float y, float z)
{
	if (!activated)
		this->activate();
	glUniform3f(glGetUniformLocation(this->ID, uniform), x, y, z);
}
void Shader::setVector3f(bool activated, const char* uniform, const glm::vec3& vec)
{
	if (!activated)
		this->activate();
	glUniform3f(glGetUniformLocation(this->ID, uniform), vec.x, vec.y, vec.z);
}
void Shader::setVector4f(bool activated, const char* uniform, float x, float y, float z, float w)
{
	if (!activated)
		this->activate();
	glUniform4f(glGetUniformLocation(this->ID, uniform), x, y, z, w);
}
void Shader::setVector4f(bool activated, const char* uniform, const glm::vec4& vec)
{
	if (!activated)
		this->activate();
	glUniform4f(glGetUniformLocation(this->ID, uniform), vec.x, vec.y, vec.z, vec.w);
}
void Shader::setMat4(bool activated, const char* uniform, const glm::mat4& mat)
{
	if (!activated)
		this->activate();
	glUniformMatrix4fv(glGetUniformLocation(this->ID, uniform), 1, false, glm::value_ptr(mat));
}

void Shader::compileErrors(unsigned int shader, const char* type)
{
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (std::string(type) != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog;
			std::cout << "Shader.cpp Error: " << glGetError() << std::endl;
			std::cout << "Shaders created: " << shaderSingleton->getNumberOfShaders() << std::endl;
		}
	}
	else
	{
		glValidateProgram(ID);
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog;
			std::cout << "Shaders created: " << shaderSingleton->getNumberOfShaders() << std::endl;
		}
	}
}

