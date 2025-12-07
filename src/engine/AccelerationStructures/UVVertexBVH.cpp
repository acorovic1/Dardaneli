#include "UVVertexBVH.h"

UVVertexBVH* UVVertexBVH::instancePtr = nullptr;

UVVertexBVH* UVVertexBVHSingleton = UVVertexBVH::getInstance();

UVVertexBVH* UVVertexBVH::getInstance()
{
	if (!instancePtr)
		instancePtr = new UVVertexBVH();
	return instancePtr;
}

void UVVertexBVH::BuildBottomUp(Mesh& mesh) // O(n^3)
{
	this->Clear();

	std::vector<std::shared_ptr<UVVertex> >vertices = mesh.getUVCoords();
	int numObjects = vertices.size();
	if (!numObjects)return;
	std::vector<BVHNode*> bvhNodes(0);

	for (int i = 0; i < numObjects; i++)
	{
		BVHNode* leaf = new BVHNode(vertices[i]->uv.x,vertices[i]->uv.y,0.0f, i);
		bvhNodes.push_back(leaf); //form leaf nodes
	}

	while (numObjects > 1) {
		int axis = rand() % 3;

		if (axis == 0)	std::sort(bvhNodes.begin(), bvhNodes.begin() + numObjects, [](BVHNode* a, BVHNode* b) {return a->box.min.x < b->box.min.x; });
		else if (axis == 1) std::sort(bvhNodes.begin(), bvhNodes.begin() + numObjects, [](BVHNode* a, BVHNode* b) {return a->box.min.y < b->box.min.y; });
		else if (axis == 2) std::sort(bvhNodes.begin(), bvhNodes.begin() + numObjects, [](BVHNode* a, BVHNode* b) {return a->box.min.z < b->box.min.z; });

		BVHNode* newNode = new BVHNode(bvhNodes[0], bvhNodes[1]);

		bvhNodes[0] = newNode;
		bvhNodes.erase(bvhNodes.begin() + 1);
		numObjects--;
	}
	root = bvhNodes[0];
}

BVHNode* UVVertexBVH::getRoot() { return root; }

void UVVertexBVH::Refit(Mesh& mesh) {
	getRoot()->refitNodeUVVertex(mesh);
}

void UVVertexBVH::Clear()
{
	destroy(root);
	root = nullptr;
}

void UVVertexBVH::Draw(Camera& camera, Shader& shader, int subdivision)
{
	auto root = UVVertexBVHSingleton->getRoot();

	root->Draw(camera, shader);

	DrawTree(root, camera, shader, subdivision);
}

void UVVertexBVH::DrawLeaves(BVHNode* node, Camera& camera, Shader& shader)
{
	if (root)
		if (!node->left && !node->right)
		{
			node->Draw(camera, shader);
		}
		else {
			DrawLeaves(node->left, camera, shader);
			DrawLeaves(node->right, camera, shader);
		}
}

void UVVertexBVH::DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision) {
	if (subdivision == 0) return;
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