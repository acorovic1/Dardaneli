#include "ObjectModeBVH.h"

ObjectModeBVH* ObjectModeBVH::instancePtr = nullptr;

ObjectModeBVH* objectBVHSingleton = ObjectModeBVH::getInstance();




ObjectModeBVH* ObjectModeBVH::getInstance()
{	
	if (!instancePtr) 
		instancePtr = new ObjectModeBVH();
	return instancePtr;
}

void ObjectModeBVH::BuildBottomUp(std::vector<Object*> objects, int numObjects) // O(n^3)
{

	std::vector<BVHNode*> bvhNodes(0);
	for (int i = 0;i < numObjects;i++)
	{
		BVHNode* leaf = new BVHNode(*objects[i]);
		bvhNodes.push_back(leaf); //form leaf nodes
	}


	while (numObjects > 1) {


		int axis = rand() % 3;

		if (axis == 0) std::sort(bvhNodes.begin(), bvhNodes.begin() + numObjects, [](BVHNode* a, BVHNode* b) {return a->box.min.x < b->box.min.x;});
		else if (axis == 1) std::sort(bvhNodes.begin(), bvhNodes.begin() + numObjects, [](BVHNode* a, BVHNode* b) {return a->box.min.y < b->box.min.y;});
		else if (axis == 2) std::sort(bvhNodes.begin(), bvhNodes.begin() + numObjects, [](BVHNode* a, BVHNode* b) {return a->box.min.z < b->box.min.z;});

		BVHNode* newNode = new BVHNode(bvhNodes[0], bvhNodes[1]);

		bvhNodes[0] = newNode;
		bvhNodes.erase(bvhNodes.begin() + 1);
		numObjects--;

	}
	root = bvhNodes[0];
	
}


BVHNode* ObjectModeBVH::getRoot() { return root; }

void ObjectModeBVH::Refit() {
	getRoot()->refitNode();
}


void ObjectModeBVH::Draw(Camera& camera, Shader& shader,int subdivision)
{
	auto root = objectBVHSingleton->getRoot();


	root->Draw(camera, shader);
	
	DrawTree(root,camera, shader,subdivision);
	

}

void ObjectModeBVH::DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision) {

	if (subdivision == 0) return;
	subdivision--;

	if(node->left)
	{
		node->left->Draw(camera, shader);
		DrawTree(node->left, camera, shader, subdivision);
	}

	if(node->right)
	{
		node->right->Draw(camera, shader);
		DrawTree(node->right, camera, shader, subdivision);
	}


};
