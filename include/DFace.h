#pragma once

#include "DVertex.h"

struct DLoop;


struct DFace {

	DLoop* loop;


	DFace() : loop(nullptr) {}


	std::unordered_set<DVertex*> getVertices()const;

	std::unordered_set<DEdge*> getEdges()const;

	// used for drawing 
	std::vector<DVertex*> getVerticesVector()const;


	std::unordered_set<DLoop*>getLoops()const;

	std::unordered_set<DFace*>getAdjecentFaces()const;


	void flip();

};