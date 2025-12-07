#pragma once

#include "BVHNode.h"

struct BVHNode;

class VertexBVH {
	BVHNode* root;
	static VertexBVH* instancePtr;

	VertexBVH() :root(nullptr) {};

	void DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision);

	void destroy(BVHNode* node) {
		if (!node) return;
		destroy(node->left);
		destroy(node->right);
		delete node;
	}

public:

	static VertexBVH* getInstance();

	VertexBVH(const VertexBVH& copy) = delete;
	void operator=(const VertexBVH& copy) = delete;

	BVHNode* getRoot();
	void BuildBottomUp(Object& object);
	// pokusati staviti default vrijednosti kroz objectManager

	void Refit(Object& object);
	void Clear();

	void Draw(Camera& camera, Shader& shader, int subdivision);
	void DrawLeaves(BVHNode* node, Camera& camera, Shader& shader);
};

extern VertexBVH* VertexBVHSingleton;