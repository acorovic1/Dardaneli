#include "VertexBVH.h"

VertexBVH* VertexBVH::instancePtr = nullptr;

VertexBVH* VertexBVHSingleton = VertexBVH::getInstance();

VertexBVH* VertexBVH::getInstance()
{
	if (!instancePtr)
		instancePtr = new VertexBVH();
	return instancePtr;
}

void VertexBVH::BuildBottomUp(Mesh* mesh) // O(n^3)
{
	this->Clear();
	int numObjects = mesh->getNumberOfVertices();
	if (!numObjects)return;
	std::vector<glm::vec3 >vertices = mesh->getModelXVertices();
	std::vector<BVHNode*> bvhNodes(0);

	for (int i = 0; i < numObjects; i++)
	{
		BVHNode* leaf = new BVHNode(vertices[i], i);
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

BVHNode* VertexBVH::getRoot() { return root; }

void VertexBVH::Refit(Mesh* mesh) {
	getRoot()->refitNodeVertex(mesh);
}

void VertexBVH::Clear()
{
	destroy(root);
	root = nullptr;
}

void VertexBVH::Draw(Camera& camera, Shader& shader, int subdivision)
{
	auto root = VertexBVHSingleton->getRoot();

	root->Draw(camera, shader);

	DrawTree(root, camera, shader, subdivision);
}

void VertexBVH::DrawLeaves(BVHNode* node, Camera& camera, Shader& shader)
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

void VertexBVH::DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision) {
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