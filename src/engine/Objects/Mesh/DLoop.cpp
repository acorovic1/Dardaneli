#include "Mesh/DLoop.h"

void DLoop::removeLoopFromRadial() {


	 // Only loop in the ring
	 if (radialNext == this && radialPrev == this)
	 {
		 edge->loop = nullptr;
		 return;
	 }

	 
	 radialPrev->radialNext = radialNext;
	 radialNext->radialPrev = radialPrev;

	 // update edge
	 if (edge->loop == this)
		 edge->loop = radialNext;

	 radialNext = nullptr;
	 radialPrev = nullptr;


	}
