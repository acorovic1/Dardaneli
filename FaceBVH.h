#pragma once

#include "BVHNode.h"
#include "Mesh.h"

struct BVHNode;

class FaceBVH {
	BVHNode* root;
	static FaceBVH* instancePtr;

	FaceBVH() :root(nullptr) {};

	void DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision);

public:

	static FaceBVH* getInstance();

	FaceBVH(const FaceBVH& copy) = delete;
	void operator=(const FaceBVH& copy) = delete;

	BVHNode* getRoot();
	void BuildBottomUp(Mesh& mesh);
	// pokusati staviti default vrijednosti kroz objectManager

	void Refit(Mesh& mesh);

	void Draw(Camera& camera, Shader& shader, int subdivision);
	void DrawLeaves(BVHNode* node, Camera& camera, Shader& shader);
};

extern FaceBVH* FaceBVHSingleton;