#include "RaytracingBVHNode.h"


RaytracingBVHNode::RaytracingBVHNode(RaytracingBVHNode* a, RaytracingBVHNode* b):box(a->box, b->box), left(a), right(b){	}

RaytracingBVHNode::RaytracingBVHNode(Triangle& tri)
{

	this->tri = tri;
	std::vector<glm::vec3> verts{ tri.v0,tri.v1,tri.v2 };
	box = AABB(verts);
}
