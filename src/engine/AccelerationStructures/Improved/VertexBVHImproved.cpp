#include "AccelerationStructures/Improved/VertexBVHImproved.h"

#include <algorithm>
#include <limits>

namespace {

glm::vec3 ComputeCentroid(const AABB& box)
{
	return (box.min + box.max) * 0.5f;
}

template <typename NodeType>
int ChooseSplitAxis(const std::vector<NodeType*>& nodes, int start, int end)
{
	glm::vec3 minC(std::numeric_limits<float>::max());
	glm::vec3 maxC(std::numeric_limits<float>::lowest());

	for (int i = start; i < end; ++i)
	{
		glm::vec3 c = ComputeCentroid(nodes[i]->box);
		minC = glm::min(minC, c);
		maxC = glm::max(maxC, c);
	}

	glm::vec3 extent = maxC - minC;
	int axis = 0;
	if (extent.y > extent.x && extent.y >= extent.z) axis = 1;
	else if (extent.z > extent.x && extent.z >= extent.y) axis = 2;
	return axis;
}

template <typename NodeType>
NodeType* BuildMedianSplit(std::vector<NodeType*>& nodes, int start, int end)
{
	int count = end - start;
	if (count <= 0) return nullptr;
	if (count == 1) return nodes[start];

	int axis = ChooseSplitAxis(nodes, start, end);
	int mid = start + count / 2;
	auto comparator = [axis](NodeType* a, NodeType* b)
	{
		float ca = (a->box.min[axis] + a->box.max[axis]) * 0.5f;
		float cb = (b->box.min[axis] + b->box.max[axis]) * 0.5f;
		return ca < cb;
	};

	std::nth_element(nodes.begin() + start, nodes.begin() + mid, nodes.begin() + end, comparator);

	NodeType* left = BuildMedianSplit(nodes, start, mid);
	NodeType* right = BuildMedianSplit(nodes, mid, end);

	return new NodeType(left, right);
}

} // namespace

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
	std::vector<BVHNode*> bvhNodes;
	bvhNodes.reserve(numObjects);

	for (int i = 0; i < numObjects; i++)
	{
		bvhNodes.push_back(new BVHNode(vertices[i], i));
	}

	root = BuildMedianSplit(bvhNodes, 0, static_cast<int>(bvhNodes.size()));
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

void VertexBVHImproved::Draw(Camera& camera, Shader& shader, int subdivision)
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

