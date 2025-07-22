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


	// circular lists
	DLoop* radialPrev;
	DLoop* radialNext;


	DLoop() : tip(nullptr), edge(nullptr), face(nullptr), prev(nullptr), next(nullptr), radialPrev(this), radialNext(this) {}
	DLoop(DVertex* v, DEdge* e, DFace* f) : tip(v), edge(e), face(f), prev(nullptr), next(nullptr), radialPrev(this), radialNext(this) {}



	void removeLoopFromRadial() {



		// Only loop in the ring
		if (radialNext == this && radialPrev == this)
		{
			
			//radialNext = nullptr;
			//radialPrev = nullptr;
		
			edge->loop = nullptr;

			return;
		}

		// Remove this loop from the ring
		radialPrev->radialNext = radialNext;
		radialNext->radialPrev = radialPrev;

		//if (radialNext == this)std::cerr << "HOWWwwwww";

		// update edge
		if (edge->loop == this)
		{
			//std::cout << "\n\n\t tip before " << edge->loop->tip->position.x << " " << edge->loop->tip->position.y << " " << edge->loop->tip->position.z;

			//std::cout <<"\n\n\t" <<edge->v1->position.x << " " << edge->v1->position.y << " " << edge->v1->position.z;
			//std::cout <<"\n\n\t" <<edge->v2->position.x << " " << edge->v2->position.y << " " << edge->v2->position.z;
			edge->loop = radialNext;

			//std::cout << "\n\n\t tip after " << edge->loop->tip->position.x << " " << edge->loop->tip->position.y << " " << edge->loop->tip->position.z;
		}
		//std::cout << "\n\n\n\t ";

		radialNext = nullptr;
		radialPrev = nullptr;
	}

};



