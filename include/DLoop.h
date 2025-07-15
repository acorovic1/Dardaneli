#pragma once

#include "DVertex.h"
#include "DEdge.h"
#include "DFace.h"


struct DLoop {

	DVertex* tip;
	DEdge* edge;
	DFace* face;

	DLoop* prev;
	DLoop* next;

	DLoop* radialPrev;
	DLoop* radialNext;


	DLoop() : tip(nullptr), edge(nullptr), face(nullptr), prev(nullptr), next(nullptr), radialPrev(nullptr), radialNext(nullptr) {}
	DLoop(DVertex* v, DEdge* e, DFace* f) : tip(v), edge(e), face(f), prev(nullptr), next(nullptr), radialPrev(nullptr), radialNext(nullptr) {}



	void removeLoopFromRadial() {

		if (!radialNext)return;
		if (radialPrev == radialNext) // if there are only 2 edges in radial link
		{
			radialNext->radialNext = nullptr;
			radialNext->radialPrev = nullptr;
			return;
		}

		if (radialPrev) radialPrev->radialNext = radialNext;
		if (radialNext) radialNext->radialPrev = radialPrev;
		radialPrev = nullptr;
		radialNext = nullptr;
	}

};