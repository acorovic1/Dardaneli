#include "AccelerationStructures/Improved/ObjectModeBVHImproved.h"

#include "AccelerationStructures/Improved/BVHBuilder.h"

ObjectModeBVHImproved* ObjectModeBVHImproved::instancePtr = nullptr;


ObjectModeBVHImproved* objectBVHImprovedSingleton = ObjectModeBVHImproved::getInstance();

ObjectModeBVHImproved* ObjectModeBVHImproved::getInstance()
{
	if (!instancePtr)
		instancePtr = new ObjectModeBVHImproved();
	return instancePtr;
}

void ObjectModeBVHImproved::BuildBottomUp(std::vector<Object*> objects, int numObjects)
{
	if (numObjects <= 0)
	{
		root = nullptr;
		return;
	}

	root = BVHBuilder::BuildBVH(objects.begin(), objects.begin() + numObjects,
		[](Object* object) { return new BVHNode(*object); });
}

BVHNode* ObjectModeBVHImproved::getRoot() { return root; }

void ObjectModeBVHImproved::Refit() {
	if (auto* currentRoot = getRoot())
		currentRoot->refitNode();
}

void ObjectModeBVHImproved::Draw(Camera& camera, Shader& shader, int subdivision)
{
	auto root = objectBVHImprovedSingleton->getRoot();

	root->Draw(camera, shader);

	DrawTree(root, camera, shader, subdivision);
}

void ObjectModeBVHImproved::DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision) {
	if (subdivision == 0)
		return;
	subdivision--;

	if (node->left)
	{
		node->left->Draw(camera, shader);
		DrawTree(node->left, camera, shader, subdivision);
	}

	if (node->right)
	{
		node->right->Draw(camera, shader);
		DrawTree(node->right, camera, shader, subdivision);
	}
};

