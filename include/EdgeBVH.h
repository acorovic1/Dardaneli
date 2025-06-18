#pragma once

#include "EdgeBVHNode.h"
#include "Mesh.h"

struct EdgeBVHNode;

class EdgeBVH {
	EdgeBVHNode* root;
	static EdgeBVH* instancePtr;

	EdgeBVH() :root(nullptr) {};

	void DrawTree(EdgeBVHNode* node, Camera& camera, Shader& shader, int subdivision);

public:

	static EdgeBVH* getInstance();

	EdgeBVH(const EdgeBVH& copy) = delete;
	void operator=(const EdgeBVH& copy) = delete;

	EdgeBVHNode* getRoot();
	void BuildBottomUp(Mesh& mesh);
	// pokusati staviti default vrijednosti kroz objectManager

	void Refit(Mesh& mesh);

	void Draw(Camera& camera, Shader& shader, int subdivision);
	void DrawLeaves(EdgeBVHNode* node, Camera& camera, Shader& shader);
};

extern EdgeBVH* EdgeBVHSingleton;