#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

#include<iostream>

#include "Edge.h"

struct Vertex {

	glm::vec3 position;
	glm::vec3 normal;

	//glm::vec2 texUV;


	Edge* edge=nullptr; // arbitrary edge that comes out of the vertex



	Vertex(glm::vec3 pos, glm::vec3 norm=glm::vec3()) { position = pos; normal = norm; }

	// only temporary...
	// very bad logic... what if the vertex has the same position..
	bool operator==(Vertex vert)
	{
		return glm::all(glm::epsilonEqual(position, vert.position, 0.001f));
	}


	glm::vec3& getPositionReference() { return position; }//izbacit
	glm::vec3 getPositionCopy() const { return position; }//izbacit







	void Translate(glm::vec3 offset) {
		position += offset;
		// calculateNormals();
	};

	void Translate(float x, float y, float z) {
		position.x += x;
		position.y += y;
		position.z += z;
		// calculateNormals();
	};

	void Translate(float* offset) {
		position.x += offset[0];
		position.y += offset[1];
		position.z += offset[2];
		std::cout << "\nV.translate " << position.x;
		// calculateNormals();
	};
};
