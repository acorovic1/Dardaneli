#pragma once

#include "../BVHNode.h"

class Camera;
class Shader;
class Object;

class VertexBVHImproved {
	BVHNode* root;
	static VertexBVHImproved* instancePtr;

	VertexBVHImproved() :root(nullptr) {};

	void DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision);

	void destroy(BVHNode* node) {
		if (!node) return;
		destroy(node->left);
		destroy(node->right);
		delete node;
	}

public:

	static VertexBVHImproved* getInstance();

	VertexBVHImproved(const VertexBVHImproved& copy) = delete;
	void operator=(const VertexBVHImproved& copy) = delete;

	BVHNode* getRoot();
	void BuildBottomUp(Object& object);

	void Refit(Object& object);
	void Clear();

	void Draw(Camera& camera, Shader& shader, int subdivision);
	void DrawLeaves(BVHNode* node, Camera& camera, Shader& shader);
};

extern VertexBVHImproved* VertexBVHImprovedSingleton;

