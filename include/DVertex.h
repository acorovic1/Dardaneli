#pragma once

#include "DEdge.h"

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

#include<iostream>
#include<vector>

#include "unordered_set"


struct DVertex {

	glm::vec3 position;
	glm::vec3 normal;
	//glm::vec2 texUV;


	DEdge* e;

	DVertex() : e(nullptr) {}
	DVertex(glm::vec3 pos, glm::vec3 norm = glm::vec3()) : position(pos), normal(norm), e(nullptr) {}



	std::unordered_set<DVertex*> getAdjecentVertices()const;
	std::unordered_set<DEdge*> getAdjecentEdges()const;
	std::unordered_set<DFace*> getAdjecentFaces()const;



	bool operator==(DVertex vert)
	{
		return glm::all(glm::epsilonEqual(position, vert.position, 0.001f));
	}

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





