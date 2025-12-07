
#include "AccelerationStructures/Improved/UVVertexBVHImproved.h"

#include "AccelerationStructures/Improved/BVHBuilder.h"

#include <numeric>


UVVertexBVHImproved* UVVertexBVHImproved::instancePtr = nullptr;

UVVertexBVHImproved* UVVertexBVHImprovedSingleton = UVVertexBVHImproved::getInstance();

UVVertexBVHImproved* UVVertexBVHImproved::getInstance()
{
	if (!instancePtr)
		instancePtr = new UVVertexBVHImproved();
	return instancePtr;
}

void UVVertexBVHImproved::BuildBottomUp(Mesh& mesh)
{
	this->Clear();

	std::vector<std::shared_ptr<UVVertex>> vertices = mesh.getUVCoords();
	int numObjects = static_cast<int>(vertices.size());
	if (!numObjects) return;


	std::vector<int> indices(numObjects);
	std::iota(indices.begin(), indices.end(), 0);

	root = BVHBuilder::BuildBVH(indices,
		[&vertices](int idx)
		{
			return new BVHNode(vertices[idx]->uv.x, vertices[idx]->uv.y, 0.0f, idx);
		});

}

BVHNode* UVVertexBVHImproved::getRoot() { return root; }

void UVVertexBVHImproved::Refit(Mesh& mesh) {
	if (auto* currentRoot = getRoot())
		currentRoot->refitNodeUVVertex(mesh);
}

void UVVertexBVHImproved::Clear()
{
	destroy(root);
	root = nullptr;
}

void UVVertexBVHImproved::Draw(Camera& camera, Shader& shader, int subdivision)
{
	auto root = UVVertexBVHImprovedSingleton->getRoot();

	root->Draw(camera, shader);

	DrawTree(root, camera, shader, subdivision);
}

void UVVertexBVHImproved::DrawLeaves(BVHNode* node, Camera& camera, Shader& shader)
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

void UVVertexBVHImproved::DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision) {
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

