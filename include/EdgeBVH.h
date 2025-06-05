#pragma once

#include "BVHNode.h"
#include "Mesh.h"

struct BVHNode;

class EdgeBVH {
	BVHNode* root;
	static EdgeBVH* instancePtr;

	EdgeBVH() :root(nullptr) {};

	void DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision);

public:

	static EdgeBVH* getInstance();

	EdgeBVH(const EdgeBVH& copy) = delete;
	void operator=(const EdgeBVH& copy) = delete;

	BVHNode* getRoot();
	void BuildBottomUp(Mesh& mesh);
	// pokusati staviti default vrijednosti kroz objectManager

	void Refit(Mesh& mesh);

	void Draw(Camera& camera, Shader& shader, int subdivision);
	void DrawLeaves(BVHNode* node, Camera& camera, Shader& shader);
};

extern EdgeBVH* EdgeBVHSingleton;