#pragma once

#include "Edge.h"

struct Face {

	Edge* edge = nullptr; // arbitrary edge that belongs to the face 

	Face() = default;
	~Face() {}

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

	std::vector<Edge*>getEdges()
	{
		std::vector<Edge*> edges;

		auto temp = edge;

		do {
			edges.push_back(temp);

			temp = temp->next;

			if (!temp) std::cerr << "\n\n Face.getEdges().. ne valja struktura podataka (Half-Edge)\n\n";


		} while (temp != edge);

		return edges;
	}
};




