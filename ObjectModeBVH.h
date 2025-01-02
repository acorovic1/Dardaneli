#pragma once



#include "BVHNode.h"

struct BVHNode;

class ObjectModeBVH {

	BVHNode* root ;
	static ObjectModeBVH* instancePtr;

	ObjectModeBVH():root(nullptr) {};

	void DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision);


public:

	static ObjectModeBVH* getInstance();

	ObjectModeBVH(const ObjectModeBVH& copy) = delete;
	void operator=(const ObjectModeBVH& copy) = delete;


	BVHNode* getRoot();
	void BuildBottomUp(std::vector<Object*> objects, int numObjects); 
	// pokusati staviti default vrijednosti kroz objectManager

	void Refit();

	void Draw(Camera& camera, Shader& shader,int subdivision);
};

extern ObjectModeBVH* objectBVHSingleton;