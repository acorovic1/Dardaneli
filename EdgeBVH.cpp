#include "EdgeBVH.h"

EdgeBVH* EdgeBVH::instancePtr = nullptr;

EdgeBVH* EdgeBVHSingleton = EdgeBVH::getInstance();

EdgeBVH* EdgeBVH::getInstance()
{
	if (!instancePtr)
		instancePtr = new EdgeBVH();
	return instancePtr;
}

void EdgeBVH::BuildBottomUp(Mesh& mesh) // O(n^3)
{
	std::vector<Edge*>edges= mesh.getEdges();
	std::vector<Vertex>vertices= mesh.getVerticesCopy();
	int numObjects = edges.size();
	std::vector<BVHNode*> bvhNodes(0);

	//form leaf nodes
	int startIndex, endIndex;
	for (int i = 0; i < numObjects; i++)
	{
		startIndex =  std::find(vertices.begin(), vertices.end(), *edges[i]->pair->tip) - vertices.begin();
		endIndex =  std::find(vertices.begin(), vertices.end(), *edges[i]->tip) - vertices.begin();
		BVHNode* leaf = new BVHNode(edges[i],startIndex,endIndex);
		//std::cout << i << " " << startIndex << " " << endIndex<<"		";
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

BVHNode* EdgeBVH::getRoot() { return root; }

void EdgeBVH::Refit(Mesh& mesh) {
	getRoot()->refitNodeEdge(mesh);
}

void EdgeBVH::Draw(Camera& camera, Shader& shader, int subdivision)
{
	auto root = EdgeBVHSingleton->getRoot();

	root->Draw(camera, shader);

	DrawTree(root, camera, shader, subdivision);
}

void EdgeBVH::DrawLeaves(BVHNode* node, Camera& camera, Shader& shader)
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

void EdgeBVH::DrawTree(BVHNode* node, Camera& camera, Shader& shader, int subdivision) {
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