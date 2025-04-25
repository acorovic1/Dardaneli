#pragma once

struct Vertex;
struct Face;

// half-edge data structure
struct Edge {

	Edge* pair = nullptr; // basically same edge but other direction
	Edge* next = nullptr; // next half-edge of the face
	//Edge* prev; 

	Vertex* tip = nullptr; // vertex that the edge goes into
	Face* face = nullptr; // face to the left

public:
	Edge() = default;
};