#pragma once

#include <vector>

#include "../BVHNode.h"

class Camera;
class Shader;
class Object;

class ObjectModeBVHImproved {
	BVHNode* root;
	static ObjectModeBVHImproved* instancePtr;

	ObjectModeBVHImproved() :root(nullptr) {};

	void DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision);

public:

	static ObjectModeBVHImproved* getInstance();

	ObjectModeBVHImproved(const ObjectModeBVHImproved& copy) = delete;
	void operator=(const ObjectModeBVHImproved& copy) = delete;

	BVHNode* getRoot();
	void BuildBottomUp(std::vector<Object*> objects, int numObjects);

	void Refit();

	void Draw(Camera& camera, Shader& shader, int subdivision);
	void DrawRayInteraction(BVHNode* node, const Ray& ray, Camera& camera, Shader& shader);
};

extern ObjectModeBVHImproved* objectBVHImprovedSingleton;

