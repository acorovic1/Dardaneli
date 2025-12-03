#pragma once

#include "RaytracingBVHNode.h"
#include "Mesh/Mesh.h"

struct RaytracingBVHNode;

class RaytracingBVH {
	RaytracingBVHNode* root;
	static RaytracingBVH* instancePtr;

	RaytracingBVH() :root(nullptr) {};

	void DrawTree(RaytracingBVHNode* node, Camera& camera, Shader& shader, int subdivision);
	void destroy(RaytracingBVHNode* node) {
		if (!node) return;
		destroy(node->left);
		destroy(node->right);
		delete node;
	}

public:

	static RaytracingBVH* getInstance();

	RaytracingBVH(const RaytracingBVH& copy) = delete;
	void operator=(const RaytracingBVH& copy) = delete;

	RaytracingBVHNode* getRoot();
	void Build(Mesh* mesh);
	// pokusati staviti default vrijednosti kroz objectManager

	//void Refit(Mesh& mesh);
	void Clear();

	//void Draw(Camera& camera, Shader& shader, int subdivision);
	//void DrawLeaves(RaytracingBVHNode* node, Camera& camera, Shader& shader);
};

extern RaytracingBVH* RaytracingBVHSingleton;