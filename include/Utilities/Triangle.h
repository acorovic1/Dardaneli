#pragma once

#include "glm/glm.hpp"






// used for ray-triangle intersection tests
struct Triangle {

	float v0x, v0y, v0z;
	float v1x, v1y, v1z;
	float v2x, v2y, v2z;
	float cx, cy, cz; 

};



struct TriangleMaterial {
	glm::vec2 uv0, uv1; // triangle uv coords

	glm::vec3 normal0, normal1, normal2; // triangle normals
};