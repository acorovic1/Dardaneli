#include "RaytracingBVHNode.h"


RaytracingBVHNode::RaytracingBVHNode(RaytracingBVHNode* a, RaytracingBVHNode* b):box(a->box, b->box), left(a), right(b){	}

RaytracingBVHNode::RaytracingBVHNode(Triangle& tri)
{

	this->tri = tri;
	std::vector<glm::vec3> verts{ glm::vec3(tri.v0x,tri.v0y,tri.v0z),glm::vec3(tri.v1x,tri.v1y,tri.v1z),glm::vec3(tri.v2x,tri.v2y,tri.v2z) };
	box = AABB(verts);
}

RaytracingBVHNode::RaytracingBVHNode(BVHNode* node) :box(node->box), left(nullptr), right(nullptr){}

