#pragma once

#include "FaceBVHNode.h"
#include "Mesh/Mesh.h"

struct FaceBVHNode;

class FaceBVH {
	FaceBVHNode* root;
	static FaceBVH* instancePtr;

	FaceBVH() :root(nullptr) {};

	void DrawTree(FaceBVHNode* node, Camera& camera, Shader& shader, int subdivision);
	void destroy(FaceBVHNode* node) {
		if (!node) return;
		destroy(node->left);
		destroy(node->right);
		delete node;
	}

public:

	static FaceBVH* getInstance();

	FaceBVH(const FaceBVH& copy) = delete;
	void operator=(const FaceBVH& copy) = delete;

	FaceBVHNode* getRoot();
	void BuildBottomUp(Mesh& mesh);
	// pokusati staviti default vrijednosti kroz objectManager

	void Refit(Mesh& mesh);
	void Clear();

	void Draw(Camera& camera, Shader& shader, int subdivision);
	void DrawLeaves(FaceBVHNode* node, Camera& camera, Shader& shader);
};

extern FaceBVH* FaceBVHSingleton;