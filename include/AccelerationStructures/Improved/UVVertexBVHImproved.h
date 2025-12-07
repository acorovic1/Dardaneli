#pragma once

#include "../BVHNode.h"

class Camera;
class Shader;
class Mesh;

class UVVertexBVHImproved {
	BVHNode* root;
	static UVVertexBVHImproved* instancePtr;

	UVVertexBVHImproved() :root(nullptr) {};

	void DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision);

	void destroy(BVHNode* node) {
		if (!node) return;
		destroy(node->left);
		destroy(node->right);
		delete node;
	}

public:

	static UVVertexBVHImproved* getInstance();

	UVVertexBVHImproved(const UVVertexBVHImproved& copy) = delete;
	void operator=(const UVVertexBVHImproved& copy) = delete;

	BVHNode* getRoot();
	void BuildBottomUp(Mesh& mesh);

	void Refit(Mesh& mesh);
	void Clear();

	void Draw(Camera& camera, Shader& shader, int subdivision);
	void DrawLeaves(BVHNode* node, Camera& camera, Shader& shader);
};

extern UVVertexBVHImproved* UVVertexBVHImprovedSingleton;

