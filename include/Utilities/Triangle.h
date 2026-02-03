#pragma once

#include "glm/glm.hpp"


// used for ray-triangle intersection tests
struct Triangle {

	// vertex position
	float v0x, v0y, v0z;
	float v1x, v1y, v1z;
	float v2x, v2y, v2z;

	// triangle centroid
	float cx, cy, cz; 

};


// used for texturing and shading
struct TriangleMaterial {

	// uv coordinates
	float u0, v0;
	float u1, v1;
	float u2, v2;

	// vertex normals
	float n1x, n1y, n1z;
	float n2x, n2y, n2z;
	float n3x, n3y, n3z;

	//float dummy;

	// 
	bool mirror;

	//// dummy padding to make size multiple of 16 bytes
	bool dummy1;
	bool dummy2;
	bool dummy3;
};



// The material data could simply be added to the triangle struct:
// It turns out that this is not a very good idea. 
// We don’t need the extra data during ray/scene intersection, but it will be read into the caches.
// Smaller data leaves more room for other data, and therefore we split the triangle data. 
// We keep the original Tri struct, and add another one, just for texturing and shading:

