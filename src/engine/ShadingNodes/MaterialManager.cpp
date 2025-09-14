#include "ShadingNodes/MaterialManager.h"
#include "ShadingNodes/Material.h"

MaterialManager* MaterialManager::instancePtr = nullptr;

MaterialManager* materialSingleton = MaterialManager::getInstance();

MaterialManager* MaterialManager::getInstance()
{
	if (!instancePtr)
		instancePtr = new MaterialManager();

	return instancePtr;
}

void MaterialManager::addMaterial(Material* mat)
{
	materials.push_back(mat);
}

void MaterialManager::deleteMaterial(GLuint index)
{
	materials.erase(materials.begin() + index);
}

Material* MaterialManager::getMaterial(std::string name)
{
	for (auto mat : materials)
		if (mat->getName() == name)
			return mat;
}

std::vector<Material*>& MaterialManager::getAllMaterials()
{
	return materials;
}
