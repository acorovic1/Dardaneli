#pragma once

#include "Edge.h"

struct Face {

	Edge* edge = nullptr; // arbitrary edge that belongs to the face 

	Face() = default;

	std::vector<Vertex*> getVertices() const
	{
		std::vector<Vertex*> vertices;
		auto temp = edge;

		do {
			vertices.push_back(temp->tip);
			temp = temp->next;
		} while (temp != edge);
		return vertices;
	}
};




