#pragma once



#include "glad/glad.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>


class Shader;


class ShaderManager {

	std::unordered_map<std::string,Shader*> shaders;

	static ShaderManager* instancePtr;

	ShaderManager()  {};

public:

	static ShaderManager* getInstance();



	ShaderManager(const ShaderManager& copy) = delete;
	void operator=(const ShaderManager& copy) = delete;



	GLuint getNumberOfShaders()const;


	void addShader(std::string name, Shader* shader);
	std::unordered_map<std::string, Shader*>::iterator deleteShader(Shader* shader);

	Shader& getShader(std::string name);

	friend void DeleteAllShaders();
	
};

extern ShaderManager* shaderSingleton;