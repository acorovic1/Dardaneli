#pragma once

#include "DVertex.h"
#include "DEdge.h"
#include "DFace.h"
#include "UVVertex.h"

struct DLoop {

	DVertex* tip;
	DEdge* edge;
	DFace* face;

	DLoop* prev;
	DLoop* next;

	DLoop* radialPrev;
	DLoop* radialNext;


	std::shared_ptr<UVVertex> uvVertex;


	DLoop() : tip(nullptr), edge(nullptr), face(nullptr), prev(nullptr), next(nullptr), radialPrev(this), radialNext(this),uvVertex(nullptr) {}
	DLoop(DVertex* v, DEdge* e, DFace* f) : tip(v), edge(e), face(f), prev(nullptr), next(nullptr), radialPrev(this), radialNext(this), uvVertex(nullptr) {}

	void removeLoopFromRadial();
};



