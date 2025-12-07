#include "AccelerationStructures/Improved/FaceBVHImproved.h"


#include "AccelerationStructures/Improved/BVHBuilder.h"
#include "Mesh/Mesh.h"
#include <algorithm>


FaceBVHImproved* FaceBVHImproved::instancePtr = nullptr;

FaceBVHImproved* FaceBVHImprovedSingleton = FaceBVHImproved::getInstance();

FaceBVHImproved* FaceBVHImproved::getInstance()
{
	if (!instancePtr)
		instancePtr = new FaceBVHImproved();
	return instancePtr;
}

void FaceBVHImproved::BuildBottomUp(Mesh& mesh)
{
	this->Clear();

	std::unordered_set<DFace*> faces = mesh.getAllFaces();
	int numObjects = static_cast<int>(faces.size());
	if (!numObjects) return;

	std::vector<DVertex> vertices = mesh.getVerticesCopy();

	std::vector<DFace*> faceList(faces.begin(), faces.end());

	root = BVHBuilder::BuildBVH(faceList,
		[&mesh, &vertices](DFace* face)
		{
			std::vector<glm::vec3> vertexLocations;
			for (DVertex* vertex : face->getVertices())
			{
				auto it = std::find(vertices.begin(), vertices.end(), *vertex);
				int idx = static_cast<int>(std::distance(vertices.begin(), it));
				vertexLocations.push_back(mesh.getModelXVertex(idx));
			}
			return new FaceBVHNode(vertexLocations, face);
		});

}

FaceBVHNode* FaceBVHImproved::getRoot() { return root; }

void FaceBVHImproved::Refit(Mesh& mesh) {
	if (root)
		root->refitNode(mesh);
}
void FaceBVHImproved::Clear()
{
	destroy(root);
	root = nullptr;
}

void FaceBVHImproved::Draw(Camera& camera, Shader& shader, int subdivision)
{
	auto root = FaceBVHImprovedSingleton->getRoot();

	root->Draw(camera, shader);

	DrawTree(root, camera, shader, subdivision);
}

void FaceBVHImproved::DrawLeaves(FaceBVHNode* node, Camera& camera, Shader& shader)
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

void FaceBVHImproved::DrawTree(FaceBVHNode* node, Camera& camera, Shader& shader, int subdivision) {
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

