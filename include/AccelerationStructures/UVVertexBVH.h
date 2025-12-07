#pragma once

#include "BVHNode.h"

struct BVHNode;

// the AABBs should be 2D

class UVVertexBVH {
	BVHNode* root;
	static UVVertexBVH* instancePtr;

	UVVertexBVH() :root(nullptr) {};

	void DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision);

	void destroy(BVHNode* node) {
		if (!node) return;
		destroy(node->left);
		destroy(node->right);
		delete node;
	}

public:

	static UVVertexBVH* getInstance();

	UVVertexBVH(const UVVertexBVH& copy) = delete;
	void operator=(const UVVertexBVH& copy) = delete;

	BVHNode* getRoot();
	void BuildBottomUp(Mesh& mesh);
	// pokusati staviti default vrijednosti kroz objectManager

	void Refit(Mesh& mesh);
	void Clear();

	void Draw(Camera& camera, Shader& shader, int subdivision);
	void DrawLeaves(BVHNode* node, Camera& camera, Shader& shader);
};

extern UVVertexBVH* UVVertexBVHSingleton;