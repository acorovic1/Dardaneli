#pragma once

#include "BoundingVolumes\AABB.h"
#include "Triangle.h"
#include "BVHNode.h"

#include "algorithm"

class Mesh;

struct  RaytracingBVHNode
{
	AABB box;
	RaytracingBVHNode* left = nullptr, * right = nullptr;

	Triangle tri;

	RaytracingBVHNode(RaytracingBVHNode* a, RaytracingBVHNode* b);

	RaytracingBVHNode(Triangle& tri);

	RaytracingBVHNode(BVHNode* node);

};


struct flatRTNode
{
	glm::vec3 aabbMin;
	// left child is always next to it in the array
	int right;     // offset of right child, -1 if leaf... when searching for the right child, do arr[i + right]
	glm::vec3 aabbMax;  // AABB 
	int triIndex; // triangle index if leaf, -2 if tlas,-1 if inner blas

	// if its a leaf then aabb is irrelevant and thus set to 0
	flatRTNode() :aabbMin(0), aabbMax(0), right(-1), triIndex(-1) {}
	flatRTNode(BVHNode* node, bool leaf = false) :aabbMin(node->box.min), aabbMax(node->box.max), right(-1), triIndex(-2) {}
	flatRTNode(Triangle& tri, unsigned int index) : right(-1), triIndex(index)
	{
		aabbMin = glm::vec3(
			ffmin(ffmin(tri.v0x, tri.v1x), tri.v2x),
			ffmin(ffmin(tri.v0y, tri.v1y), tri.v2y),
			ffmin(ffmin(tri.v0z, tri.v1z), tri.v2z)
		);
		aabbMax = glm::vec3(
			ffmax(ffmax(tri.v0x, tri.v1x), tri.v2x),
			ffmax(ffmax(tri.v0y, tri.v1y), tri.v2y),
			ffmax(ffmax(tri.v0z, tri.v1z), tri.v2z)
		);

		if (aabbMin.x == aabbMax.x) aabbMax.x += 0.1f;
		if (aabbMin.y == aabbMax.y) aabbMax.y += 0.1f;
		if (aabbMin.z == aabbMax.z) aabbMax.z += 0.1f;

	}

};