#include "ShadingNodes/ShadingNodesManager.h"

ShadingNodesManager* ShadingNodesManager::instancePtr = nullptr;
ShadingNodesManager* shadingNodesSingleton = ShadingNodesManager::getInstance();

ShadingNodesManager* ShadingNodesManager::getInstance() {
	if (!instancePtr)
		instancePtr = new ShadingNodesManager();
	return instancePtr;
}

void ShadingNodesManager::addNewNode(ShadingNodes* node)
{
	nodes.push_back(node);
}
