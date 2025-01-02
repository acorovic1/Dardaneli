#pragma once



#include "BVHNode.h"

struct BVHNode;

class EditModeBVH {

	BVHNode* root;
	static EditModeBVH* instancePtr;

	EditModeBVH() :root(nullptr) {};

	void DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision);


public:

	static EditModeBVH* getInstance();

	EditModeBVH(const EditModeBVH& copy) = delete;
	void operator=(const EditModeBVH& copy) = delete;


	BVHNode* getRoot();
	void BuildBottomUp(Object& object);
	// pokusati staviti default vrijednosti kroz objectManager

	void Refit(Object& object);

	void Draw(Camera& camera, Shader& shader, int subdivision);
	void DrawLeaves(BVHNode* node, Camera& camera, Shader& shader);
};

extern EditModeBVH* editModeBVHSingleton;