#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

#include<iostream>
#include<vector>
#include<unordered_set>

#include "Edge.h"

struct Vertex {

	glm::vec3 position;
	glm::vec3 normal;

	//glm::vec2 texUV;


	Edge* edge = nullptr; // arbitrary edge that comes out of the vertex

	~Vertex() {}

	std::unordered_set<Face*> getAdjecentFaces() const {
		if (!edge) return std::unordered_set<Face*>();

		std::unordered_set<Face*> faces = std::unordered_set<Face*>();

		for (auto& x : this->getAdjecentEdges())
		{
			if (x->face)
				faces.insert(x->face);
			if (x->pair->face)
				faces.insert(x->pair->face);
		}

		return faces;
	}

	// returns all adjecent half-edges that flow outward from the vertex
	std::vector<Edge*> getAdjecentEdges()const {
		if (!edge) return std::vector<Edge*>();


		Edge* temp = edge;
		Edge* temp2;
		std::vector<Edge*> edges = std::vector<Edge*>();

		do
		{
			edges.push_back(temp);
			temp2 = temp;
			do
			{
				temp = temp->next;
			} while (temp->next != temp2);

			temp = temp->pair;
		
		} while (temp != edge);
		return edges;
	}

	// the algorithm is the same as for the method 'getAdjecentEdges' aside from the return value
	std::vector<Vertex*> getAdjecentVertices()const {
		if (!edge) return std::vector<Vertex*>();


		Edge* temp = edge;
		Edge* temp2;
		std::vector<Vertex*> vertices = std::vector<Vertex*>();

		do
		{
			vertices.push_back(temp->tip);
			temp2 = temp;
			do
			{
				temp = temp->next;
			} while (temp->next != temp2);
			if (temp->pair)
				temp = temp->pair;
			else break;
		} while (temp != edge);
		return vertices;
	}

	Vertex(glm::vec3 pos, glm::vec3 norm = glm::vec3()) { position = pos; normal = norm; }

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
