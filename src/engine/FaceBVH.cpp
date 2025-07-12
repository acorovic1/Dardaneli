#include "FaceBVH.h"
#include "FaceBVHNode.h"
#include "DFace.h"

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
	this->Clear();

	std::unordered_set<DFace*>faces = mesh.getAllFaces();


	std::vector<DVertex>vertices = mesh.getVerticesCopy();
	int numObjects = faces.size();
	if (!numObjects)return;
	std::vector<FaceBVHNode*> bvhNodes(0);

	std::vector<int>vertexIndices;
	std::vector<glm::vec3>vertexLocations;
	//form leaf nodes
	for (DFace* face : faces)
	{
		//std::cout << "\n\n NUMBER OF FACES " << faces.size()<<"\n\n";
		std::unordered_set<DVertex*> temp = face->getVertices();


		for (DVertex* vertex : face->getVertices())
		{
			vertexIndices.push_back(std::find(vertices.begin(), vertices.end(), *vertex) - vertices.begin());
			vertexLocations.push_back(mesh.getModelXVertex(vertexIndices.back()));
		}

		FaceBVHNode* leaf = new FaceBVHNode(vertexLocations, face);
		//for (auto x : vertexIndices)
		//	std::cout << x << " ";
		//std::cout << "\n";
		//std::cout << i << " " << startIndex << " " << endIndex << "		";
		vertexIndices.clear();
		vertexLocations.clear();
		bvhNodes.push_back(leaf);
	}

	while (numObjects > 1) {
		int axis = rand() % 3;

		if (axis == 0)	std::sort(bvhNodes.begin(), bvhNodes.begin() + numObjects, [](FaceBVHNode* a, FaceBVHNode* b) {return a->box.min.x < b->box.min.x; });
		else if (axis == 1) std::sort(bvhNodes.begin(), bvhNodes.begin() + numObjects, [](FaceBVHNode* a, FaceBVHNode* b) {return a->box.min.y < b->box.min.y; });
		else if (axis == 2) std::sort(bvhNodes.begin(), bvhNodes.begin() + numObjects, [](FaceBVHNode* a, FaceBVHNode* b) {return a->box.min.z < b->box.min.z; });

		FaceBVHNode* newNode = new FaceBVHNode(bvhNodes[0], bvhNodes[1]);

		bvhNodes[0] = newNode;
		bvhNodes.erase(bvhNodes.begin() + 1);
		numObjects--;
	}
	root = bvhNodes[0];
}

FaceBVHNode* FaceBVH::getRoot() { return root; }

void FaceBVH::Refit(Mesh& mesh) {
	//getRoot()->refitNodeEdge(mesh);
}
void FaceBVH::Clear()
{
	destroy(root);
	root = nullptr;
}

void FaceBVH::Draw(Camera& camera, Shader& shader, int subdivision)
{
	auto root = FaceBVHSingleton->getRoot();

	root->Draw(camera, shader);

	DrawTree(root, camera, shader, subdivision);
}

void FaceBVH::DrawLeaves(FaceBVHNode* node, Camera& camera, Shader& shader)
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

void FaceBVH::DrawTree(FaceBVHNode* node, Camera& camera, Shader& shader, int subdivision) {
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