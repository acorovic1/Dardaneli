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
	float aabbMinX, aabbMinY, aabbMinZ;
	// left child is always next to it in the array
	int right;     // offset of right child, -1 if leaf... when searching for the right child, do arr[i + right]
	float aabbMaxX, aabbMaxY, aabbMaxZ;
	int triIndex; // triangle index if leaf, -2 if tlas,-1 if inner blas

	// if its a leaf then aabb is irrelevant and thus set to 0
	flatRTNode() :aabbMinX(0), aabbMinY(0), aabbMinZ(0), aabbMaxX(0), aabbMaxY(0), aabbMaxZ(0), right(-1), triIndex(-1) {}
	flatRTNode(BVHNode* node) :aabbMinX(node->box.min.x), aabbMinY(node->box.min.y), aabbMinZ(node->box.min.z),
		aabbMaxX(node->box.max.x), aabbMaxY(node->box.max.y), aabbMaxZ(node->box.max.z), right(-1), triIndex(-2) {
	}
	flatRTNode(Triangle& tri, unsigned int index) : right(-1), triIndex(index)
	{

		aabbMinX = ffmin(ffmin(tri.v0x, tri.v1x), tri.v2x);
		aabbMinY = ffmin(ffmin(tri.v0y, tri.v1y), tri.v2y);
		aabbMinZ = ffmin(ffmin(tri.v0z, tri.v1z), tri.v2z);


		aabbMaxX = ffmax(ffmax(tri.v0x, tri.v1x), tri.v2x);
		aabbMaxY = ffmax(ffmax(tri.v0y, tri.v1y), tri.v2y);
		aabbMaxZ = ffmax(ffmax(tri.v0z, tri.v1z), tri.v2z);


		if (aabbMinX == aabbMaxX) aabbMaxX += 0.1f;
		if (aabbMinY == aabbMaxY) aabbMaxY += 0.1f;
		if (aabbMinZ == aabbMaxZ) aabbMaxZ += 0.1f;

	}

};