#pragma once

#include "../EdgeBVHNode.h"

class Camera;
class Shader;
class Mesh;

class EdgeBVHImproved {
	EdgeBVHNode* root;
	static EdgeBVHImproved* instancePtr;

	EdgeBVHImproved() :root(nullptr) {};

	void DrawTree(EdgeBVHNode* node, Camera& camera, Shader& shader, int subdivision);
	void destroy(EdgeBVHNode* node) {
		if (!node) return;
		destroy(node->left);
		destroy(node->right);
		delete node;
	}

public:

	static EdgeBVHImproved* getInstance();

	EdgeBVHImproved(const EdgeBVHImproved& copy) = delete;
	void operator=(const EdgeBVHImproved& copy) = delete;

	EdgeBVHNode* getRoot();
	void BuildBottomUp(Mesh& mesh);

	void Refit(Mesh& mesh);
	void Clear();

	void Draw(Camera& camera, Shader& shader, int subdivision);
	void DrawLeaves(EdgeBVHNode* node, Camera& camera, Shader& shader);
};

extern EdgeBVHImproved* EdgeBVHImprovedSingleton;

