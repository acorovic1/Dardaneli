#include "RaytracingBVH.h"
#include "ObjectModeBVH.h"

#include <algorithm>
#include <limits>



glm::vec3 ComputeCentroid(const AABB& box)
{
	return (box.min + box.max) * 0.5f;
}


int ChooseSplitAxis(const std::vector<flatRTNode>& nodes, int start, int end)
{
	glm::vec3 minC(std::numeric_limits<float>::max());
	glm::vec3 maxC(std::numeric_limits<float>::lowest());

	for (int i = start; i < end; ++i)
	{
		/*minC = glm::min(minC, nodes[i].aabbMin);
		maxC = glm::max(maxC, nodes[i].aabbMax);*/

		minC.x = ffmin(minC.x, nodes[i].aabbMinX);
		minC.y = ffmin(minC.y, nodes[i].aabbMinY);
		minC.z = ffmin(minC.z, nodes[i].aabbMinZ);

		maxC.x = ffmax(maxC.x, nodes[i].aabbMaxX);
		maxC.y = ffmax(maxC.y, nodes[i].aabbMaxY);
		maxC.z = ffmax(maxC.z, nodes[i].aabbMaxZ);
	}

	glm::vec3 extent = maxC - minC;
	int axis = 0;
	if (extent.y > extent.x && extent.y >= extent.z) axis = 1;
	else if (extent.z > extent.x && extent.z >= extent.y) axis = 2;
	return axis;
}


int BuildMedianSplit(std::vector<flatRTNode>& bvhNodes, int start, int end)
{
	int count = end - start;
	if (count <= 0) return -1;
	if (count == 1)
	{
		
		RaytracingBVHSingleton->nodes.emplace_back(bvhNodes[start]);
		return RaytracingBVHSingleton->nodes.size()-1;

	}

	int axis = ChooseSplitAxis(bvhNodes, start, end);
	int mid = start + count / 2;
	auto comparator = [axis](flatRTNode& a, flatRTNode& b)
		{
			/*float ca = (a.aabbMin[axis] + a.aabbMax[axis]) * 0.5f;
			float cb = (b.aabbMin[axis] + b.aabbMax[axis]) * 0.5f;*/

			float ca, cb;
			if (axis == 0) {
				ca = (a.aabbMinX + a.aabbMaxX) * 0.5f;
				cb = (b.aabbMinX + b.aabbMaxX) * 0.5f;
			}
			else if (axis == 1) {
				ca = (a.aabbMinY + a.aabbMaxY) * 0.5f;
				cb = (b.aabbMinY + b.aabbMaxY) * 0.5f;
			}
			else {
				ca = (a.aabbMinZ + a.aabbMaxZ) * 0.5f;
				cb = (b.aabbMinZ + b.aabbMaxZ) * 0.5f;
			}

			return ca < cb;
		};

	std::nth_element(bvhNodes.begin() + start, bvhNodes.begin() + mid, bvhNodes.begin() + end, comparator);

	size_t parentIndex = RaytracingBVHSingleton->nodes.size(); // 2 3 4
	RaytracingBVHSingleton->nodes.emplace_back(flatRTNode());
	//flatRTNode& parent = RaytracingBVHSingleton->nodes.back();

	int leftIndex = BuildMedianSplit(bvhNodes, start, mid);
//	std::cout << "\n Node with indexX " << index << " has right offset = " << parent.right;


	int rightIndex = BuildMedianSplit(bvhNodes, mid, end);

	/*parent.aabbMin = glm::min(left.aabbMin, right.aabbMin);
	parent.aabbMax = glm::max(left.aabbMax, right.aabbMax);*/

	flatRTNode& parent = RaytracingBVHSingleton->nodes[parentIndex];
	flatRTNode& left = RaytracingBVHSingleton->nodes[leftIndex];
	flatRTNode& right = RaytracingBVHSingleton->nodes[rightIndex];

	std::cout << "\n Created parent node at index " << parentIndex << "( " << parent.triIndex<<" )" << " with left child index " << leftIndex << "( " << left.triIndex << " )" << " and right child index " << rightIndex << "( " << right.triIndex << " )";

	parent.aabbMinX = ffmin(left.aabbMinX, right.aabbMinX);
	parent.aabbMinY = ffmin(left.aabbMinY, right.aabbMinY);
	parent.aabbMinZ = ffmin(left.aabbMinZ, right.aabbMinZ);

	parent.aabbMaxX = ffmax(left.aabbMaxX, right.aabbMaxX);
	parent.aabbMaxY = ffmax(left.aabbMaxY, right.aabbMaxY);
	parent.aabbMaxZ = ffmax(left.aabbMaxZ, right.aabbMaxZ);

	parent.right = rightIndex - parentIndex;

	return parentIndex;
}



RaytracingBVH* RaytracingBVH::instancePtr = nullptr;

RaytracingBVH* RaytracingBVHSingleton = RaytracingBVH::getInstance();

RaytracingBVH* RaytracingBVH::getInstance()
{
	if (!instancePtr)
		instancePtr = new RaytracingBVH();
	return instancePtr;
}

std::vector<flatRTNode>& RaytracingBVH::getNodes()
{
	return nodes;
}

std::vector<Triangle>& RaytracingBVH::getTriangles()
{
	return triangles;
}

void RaytracingBVH::Build()
{
	this->Clear();
	BVHNode* objRoot = objectBVHSingleton->getRoot();

	// opali dfs, na kraju buildat Triangle BVH

	findTlasLeaf(objRoot);


	//root = BuildMedianSplit(bvhNodes, 0, static_cast<int>(bvhNodes.size()));
}

void RaytracingBVH::findTlasLeaf(BVHNode* objNode)
{
	if (!objNode)return;

	// tlas internal nodes
	if (objNode->left || objNode->right)
	{

		size_t index = nodes.size();
		nodes.emplace_back(flatRTNode(objNode));
		// aabb set, left index is next, right TBD, triIndex = -1


		findTlasLeaf(objNode->left);
		nodes[index].right = nodes.size() - index; // offsetIndex to right child set after left subtree is built
		//std::cout << "\n Node with index " << index << " has right offset = " << nodes[index].right;
		
		findTlasLeaf(objNode->right);

		return;
	}
	
	std::cout << "\nOBJ index = " << objNode->index.back();
	std::vector<Triangle>& triangles = dynamic_cast<Mesh*>(objectSingleton->getObject(objNode->index.back()))->getTriangles();
	size_t numObjects = triangles.size();
	if (!numObjects)
		return;


	std::vector<flatRTNode> bvhNodes;
	bvhNodes.reserve(numObjects);


	for (size_t i = 0; i < numObjects; ++i)
	{
		bvhNodes.emplace_back(flatRTNode(triangles[i], this->triangles.size()));
		this->triangles.push_back(triangles[i]);
	}

	BuildMedianSplit(bvhNodes, 0, static_cast<int>(bvhNodes.size()));

}




void RaytracingBVH::Clear()
{
	nodes.clear();
}

//void RaytracingBVH::Draw(Camera& camera, Shader& shader, int subdivision)
//{
//	auto root = RaytracingBVHSingleton->getRoot();
//
//	root->Draw(camera, shader);
//
//	DrawTree(root, camera, shader, subdivision);
//}
//
//void RaytracingBVH::DrawLeaves(RaytracingBVHNode* node, Camera& camera, Shader& shader)
//{
//	if (root)
//		if (!node->left && !node->right)
//		{
//			node->Draw(camera, shader);
//		}
//		else {
//			DrawLeaves(node->left, camera, shader);
//			DrawLeaves(node->right, camera, shader);
//		}
//}
//
//void RaytracingBVH::DrawTree(RaytracingBVHNode* node, Camera& camera, Shader& shader, int subdivision) {
//	if (subdivision == 0) return;
//	subdivision--;
//
//	if (node->left)
//	{
//		node->left->Draw(camera, shader);
//		DrawTree(node->left, camera, shader, subdivision);
//	}
//
//	if (node->right)
//	{
//		node->right->Draw(camera, shader);
//		DrawTree(node->right, camera, shader, subdivision);
//	}
//};

