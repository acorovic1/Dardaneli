#pragma once

#include "../FaceBVHNode.h"

class Camera;
class Shader;
class Mesh;

class FaceBVHImproved {
	FaceBVHNode* root;
	static FaceBVHImproved* instancePtr;

	FaceBVHImproved() :root(nullptr) {};

	void DrawTree(FaceBVHNode* node, Camera& camera, Shader& shader, int subdivision);
	void destroy(FaceBVHNode* node) {
		if (!node) return;
		destroy(node->left);
		destroy(node->right);
		delete node;
	}

public:

	static FaceBVHImproved* getInstance();

	FaceBVHImproved(const FaceBVHImproved& copy) = delete;
	void operator=(const FaceBVHImproved& copy) = delete;

	FaceBVHNode* getRoot();
	void BuildBottomUp(Mesh& mesh);

	void Refit(Mesh& mesh);
	void Clear();

	void Draw(Camera& camera, Shader& shader, int subdivision);
	void DrawLeaves(FaceBVHNode* node, Camera& camera, Shader& shader);
};

extern FaceBVHImproved* FaceBVHImprovedSingleton;

