#pragma once



#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderManager.h"

/////////// SINGLE RESPONSIBILITY PRINCIPLE ////////
std::string get_file_contents(const char* filename); 
/////////// SINGLE RESPONSIBILITY PRINCIPLE ////////

class Shader {

	GLuint ID;

	void compileErrors(unsigned int shader, const char* type);

	
	void Del();
public:
	
	Shader(std::string name,const char* vertexFile, const char* fragmentFile);

	void Activate();
	GLuint getID();
	
	void Delete();

	void setFloat(bool activated,const char* uniform, float value);
	void setInteger(bool activated,const char* uniform, int value);
	void setVector2f(bool activated,const char* uniform, float x,float y);
	void setVector2f(bool activated,const char* uniform, glm::vec2 &vec);
	void setVector3f(bool activated,const char* uniform, float x,float y,float z);
	void setVector3f(bool activated,const char* uniform, glm::vec3 &vec);
	void setVector4f(bool activated,const char* uniform, float x,float y,float z,float w);
	void setVector4f(bool activated,const char* uniform, glm::vec4 &vec);
	void setMat4(bool activated,const char* uniform, glm::mat4 &mat);

	friend void DeleteAllShaders();
};


void DeleteAllShaders();