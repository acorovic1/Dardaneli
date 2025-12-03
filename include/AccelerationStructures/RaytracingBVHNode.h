#pragma once

#include "BoundingVolumes\AABB.h"
#include "Triangle.h"


#include "algorithm"

class Mesh;

struct  RaytracingBVHNode
{
	AABB box;
	RaytracingBVHNode* left = nullptr, * right = nullptr;

	Triangle tri;


	

	//RaytracingBVHNode();

	RaytracingBVHNode(RaytracingBVHNode* a, RaytracingBVHNode* b);

	RaytracingBVHNode(Triangle& tri);
	//
	//bool Hit(const Ray& ray, std::vector<DFace*>& facesHit);
	//
	//
	//
	//void Draw(Camera& camera, Shader& shader);
};

// bvh konstruisati direktno preko ovih nodova
struct flatRTNode
{
	glm::vec3 aabbMin,aabbMax;  // AABB 
	int left;     // index of left child, -1 if leaf
	int right;    // index of right child, -1 if leaf
	int triIndex; // triangle index if leaf, -1 if inner
};