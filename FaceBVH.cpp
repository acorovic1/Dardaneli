#include "FaceBVH.h"
#include "Face.h"

FaceBVH* FaceBVH::instancePtr = nullptr;

FaceBVH* FaceBVHSingleton = FaceBVH::getInstance();

FaceBVH* FaceBVH::getInstance()
{
	if (!instancePtr)
		instancePtr = new FaceBVH();
	return instancePtr;
}

void FaceBVH::BuildBottomUp(Mesh& mesh) // O(n^3)
{
	std::vector<Face*>faces = mesh.getFaces();
	std::vector<Vertex>vertices = mesh.getVerticesCopy();
	int numObjects = faces.size();
	std::vector<BVHNode*> bvhNodes(0);

	std::vector<int>vertexIndices;
	//form leaf nodes
	for (int i = 0; i < numObjects; i++)
	{
		std::vector<Vertex*> temp = faces[i]->getVertices();
		for (int j = 0;j < temp.size();j++)
		{
			vertexIndices.push_back(std::find(vertices.begin(), vertices.end(), *temp[j]) - vertices.begin());
		}

		BVHNode* leaf = new BVHNode(faces[i], vertexIndices);
		for (auto x : vertexIndices)
			std::cout << x << " ";
		std::cout << "\n";
		//std::cout << i << " " << startIndex << " " << endIndex << "		";
		vertexIndices.clear();
		bvhNodes.push_back(leaf);
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

BVHNode* FaceBVH::getRoot() { return root; }

void FaceBVH::Refit(Mesh& mesh) {
	getRoot()->refitNodeEdge(mesh);
}

void FaceBVH::Draw(Camera& camera, Shader& shader, int subdivision)
{
	auto root = FaceBVHSingleton->getRoot();

	root->Draw(camera, shader);

	DrawTree(root, camera, shader, subdivision);
}

void FaceBVH::DrawLeaves(BVHNode* node, Camera& camera, Shader& shader)
{

	if (!node->left && !node->right)
	{


		node->Draw(camera, shader);
	}
	else {

		DrawLeaves(node->left, camera, shader);
		DrawLeaves(node->right, camera, shader);
	}
}

void FaceBVH::DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision) {
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