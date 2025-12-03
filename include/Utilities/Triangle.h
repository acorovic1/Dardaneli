#pragma once

#include "glm/glm.hpp"

// used for ray-triangle intersection tests
struct Triangle {

	glm::vec3 v0,v1,v2; // triangle verts
	glm::vec3 centroid; // used for determining closest hit

};

struct TriangleMaterial {
	glm::vec2 uv0, uv1; // triangle uv coords

	glm::vec3 normal0, normal1, normal2; // triangle normals
};