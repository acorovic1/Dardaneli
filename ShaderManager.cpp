#include "ShaderManager.h"

ShaderManager* ShaderManager::instancePtr = nullptr;
ShaderManager* shaderSingleton = ShaderManager::getInstance();

ShaderManager* ShaderManager::getInstance() {
	if (!instancePtr)
		instancePtr = new ShaderManager();
	return instancePtr;
}


GLuint ShaderManager::getNumberOfShaders()const
{
	return shaders.size();
};


void ShaderManager::addShader(std::string name,Shader* shader)
{
	shaders.insert({name, shader});
}

std::unordered_map<std::string,Shader*>::iterator ShaderManager::deleteShader(Shader* shader)
{
	for (auto it = shaderSingleton->shaders.begin(); it != shaderSingleton->shaders.end(); )
	{
		
		std::cout << "\nPassing " << it->first;
		if (it->second == shader)
		{
			std::cout << "...we here";
			return shaders.erase(it);
			
			
		}
		else { it++; }		
	}
};

Shader& ShaderManager::getShader(std::string name)
{
	if (shaders.find(name) != shaders.end())
		return *(shaders[name]);
	else std::cout << "Shader '" << name << "' does not exist.\n";
}

