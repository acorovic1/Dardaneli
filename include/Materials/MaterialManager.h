#pragma once

#include <vector>
#include "Material.h"



class MaterialManager {
	std::vector<Material*> materials;
	static MaterialManager* instancePtr;

	MaterialManager() :materials(0) {};

public:

	static MaterialManager* getInstance();

	MaterialManager(const MaterialManager& copy) = delete;
	void operator=(const MaterialManager& copy) = delete;

	void addMaterial(Material* mat);
	void deleteMaterial(GLuint index);

	Material* getMaterial(std::string name);

	std::vector<Material*>& getAllMaterials();
};

extern MaterialManager* materialSingleton;