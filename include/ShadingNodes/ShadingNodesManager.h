#pragma once

#include "glad/glad.h"
#include <vector>
#include <string>
#include <iostream>



class ShadingNodes;

class ShadingNodesManager {

	int counter = 0;
	std::vector<ShadingNodes*> nodes;

	static ShadingNodesManager* instancePtr;

	ShadingNodesManager() {};

public:

	static ShadingNodesManager* getInstance();

	ShadingNodesManager(const ShadingNodesManager& copy) = delete;
	void operator=(const ShadingNodesManager& copy) = delete;



	int getNewID() {
		int returnValue = counter;
		counter += 10;
		return returnValue;
	};

	void addNewNode(ShadingNodes* node);


};

extern ShadingNodesManager* shadingNodesSingleton;