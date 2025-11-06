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
	


	DEdge* e;

	DVertex();
	DVertex(glm::vec3 pos, glm::vec3 norm = glm::vec3());
	DVertex(const DVertex& vertex);



	std::unordered_set<DVertex*> getAdjecentVertices()const;
	std::unordered_set<DEdge*> getAdjecentEdges()const;
	std::unordered_set<DFace*> getAdjecentFaces()const;



	bool operator==(const DVertex& vert) const;

	void translate(glm::vec3 offset);

	void translate(float x, float y, float z);

	void translate(float* offset);
};





