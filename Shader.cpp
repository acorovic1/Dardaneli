#include "Shader.h"

std::string get_file_contents(const char* filename) {
	std::ifstream in(filename, std::ios::binary);

	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return (contents);
	}
	throw (errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile) 
{

	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);
	const char* vertexSource =vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	GLuint vertexShader, fragmentShader;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	compileErrors(vertexShader, "VERTEX");

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	compileErrors(fragmentShader, "FRAGMENT");


	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);		
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	compileErrors(ID, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

};

void Shader::Activate() {
	glUseProgram(ID);
}
void Shader::Delete() {
	glDeleteProgram(ID);
}


void Shader::setFloat(bool activated,const char* uniform, float value)
{	
	if (!activated)
		this->Activate();
	glUniform1f(glGetUniformLocation(this->ID, uniform), value);
}
void Shader::setInteger(bool activated,const char* uniform, int value)
{
	if (!activated)
		this->Activate();
	glUniform1i(glGetUniformLocation(this->ID, uniform), value);
}
void Shader::setVector2f(bool activated,const char* uniform, float x, float y)
{
	if (!activated)
		this->Activate();
	glUniform2f(glGetUniformLocation(this->ID, uniform), x,y);
}
void Shader::setVector2f(bool activated,const char* uniform, glm::vec2& vec) 
{
	if (!activated)
		this->Activate();
	glUniform2f(glGetUniformLocation(this->ID, uniform), vec.x, vec.y);
}
void Shader::setVector3f(bool activated,const char* uniform, float x, float y, float z)
{
	if (!activated)
		this->Activate();
	glUniform3f(glGetUniformLocation(this->ID, uniform), x, y, z);
}
void Shader::setVector3f(bool activated,const char* uniform, glm::vec3& vec)
{
	if (!activated)
		this->Activate();
	glUniform3f(glGetUniformLocation(this->ID, uniform), vec.x,vec.y,vec.z);
}
void Shader::setVector4f(bool activated,const char* uniform, float x, float y, float z, float w)
{
	if (!activated)
		this->Activate();
	glUniform4f(glGetUniformLocation(this->ID, uniform), x, y, z, w);
}
void Shader::setVector4f(bool activated,const char* uniform, glm::vec4& vec)
{
	if (!activated)
		this->Activate();
	glUniform4f(glGetUniformLocation(this->ID, uniform), vec.x, vec.y, vec.z,vec.w);
}
void Shader::setMat4(bool activated,const char* uniform, glm::mat4& mat)
{
	if (!activated)
		this->Activate();
	glUniformMatrix4fv(glGetUniformLocation(this->ID, uniform), 1, false, glm::value_ptr(mat)); 
}












void Shader::compileErrors(unsigned int shader, const char* type)
{
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}