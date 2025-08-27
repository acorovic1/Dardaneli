#pragma once

#include "DDiskLink.h"
#include <unordered_set>

struct DVertex;
struct DFace;
struct DLoop;

struct DEdge {

	DVertex* v1;
	DVertex* v2;

	DDiskLink d1;
	DDiskLink d2;

	DLoop* loop;

	bool isSeam = false;


	DEdge() : v1(nullptr), v2(nullptr), d1(), d2(), loop(nullptr) {}
	DEdge(DVertex* a, DVertex* b);

	void connectLoopToEdge(DLoop* loop);

	// only removes the edge from the disks, probably needs to be deleted afterwards
	void removeFromDisk();

	void addToDisk(DEdge* pivotEdge, DVertex* pivot);
	

	std::unordered_set<DFace*> getFaces();

	DVertex* commonVertex(DEdge* edge);


};