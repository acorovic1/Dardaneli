#include "AccelerationStructures/Improved/EdgeBVHImproved.h"

#include "AccelerationStructures/Improved/BVHBuilder.h"

EdgeBVHImproved* EdgeBVHImproved::instancePtr = nullptr;

EdgeBVHImproved* EdgeBVHImprovedSingleton = EdgeBVHImproved::getInstance();

EdgeBVHImproved* EdgeBVHImproved::getInstance()
{
	if (!instancePtr)
		instancePtr = new EdgeBVHImproved();
	return instancePtr;
}

void EdgeBVHImproved::BuildBottomUp(Mesh& mesh)
{
	this->Clear();
	std::unordered_set<DEdge*> edges = mesh.getAllEdges();
	int numObjects = static_cast<int>(edges.size());
	if (!numObjects) return;

	std::vector<DEdge*> edgeList(edges.begin(), edges.end());
	root = BVHBuilder::BuildBVH(edgeList,
		[](DEdge* edge) { return new EdgeBVHNode(edge); });
}

EdgeBVHNode* EdgeBVHImproved::getRoot() { return root; }

void EdgeBVHImproved::Refit(Mesh& mesh) {
	if (root)
		root->refitNode(mesh);
}

void EdgeBVHImproved::Clear()
{
	destroy(root);
	root = nullptr;
}

void EdgeBVHImproved::Draw(Camera& camera, Shader& shader, int subdivision)
{
	auto root = EdgeBVHImprovedSingleton->getRoot();

	root->Draw(camera, shader);

	DrawTree(root, camera, shader, subdivision);
}

void EdgeBVHImproved::DrawLeaves(EdgeBVHNode* node, Camera& camera, Shader& shader)
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

void EdgeBVHImproved::DrawTree(EdgeBVHNode* node, Camera& camera, Shader& shader, int subdivision) {
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

