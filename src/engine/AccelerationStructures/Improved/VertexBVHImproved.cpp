#include "AccelerationStructures/Improved/VertexBVHImproved.h"


#include "AccelerationStructures/Improved/BVHBuilder.h"

#include <numeric>


VertexBVHImproved* VertexBVHImproved::instancePtr = nullptr;

VertexBVHImproved* VertexBVHImprovedSingleton = VertexBVHImproved::getInstance();

VertexBVHImproved* VertexBVHImproved::getInstance()
{
	if (!instancePtr)
		instancePtr = new VertexBVHImproved();
	return instancePtr;
}

void VertexBVHImproved::BuildBottomUp(Object& object)
{
	this->Clear();
	int numObjects = object.getNumberOfVertices();
	if (!numObjects) return;

	std::vector<glm::vec3> vertices = object.getModelXVertices();

	std::vector<int> indices(numObjects);
	std::iota(indices.begin(), indices.end(), 0);

	root = BVHBuilder::BuildBVH(indices,
		[&vertices](int idx) { return new BVHNode(vertices[idx], idx); });

}

BVHNode* VertexBVHImproved::getRoot() { return root; }

void VertexBVHImproved::Refit(Object& object) {
	if (auto* currentRoot = getRoot())
		currentRoot->refitNodeVertex(object);
}

void VertexBVHImproved::Clear()
{
	destroy(root);
	root = nullptr;
}

void VertexBVHImproved::Draw(Camera& camera, Shader& shader, int subdivision, bool rayInteract)
{
	auto root = VertexBVHImprovedSingleton->getRoot();

	root->Draw(camera, shader);

	DrawTree(root, camera, shader, subdivision);
}

void VertexBVHImproved::DrawLeaves(BVHNode* node, Camera& camera, Shader& shader)
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

void VertexBVHImproved::DrawRayInteraction(BVHNode* node, const Ray& ray, Camera& camera, Shader& shader) {
	if (node->box.intersectRayAABB(ray))
	{
		node->Draw(camera, shader);
		if (node->left)
			DrawRayInteraction(node->left, ray, camera, shader);
		if (node->right)
			DrawRayInteraction(node->right, ray, camera, shader);
	}
}

void VertexBVHImproved::DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision) {
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

