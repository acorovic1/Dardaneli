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

	DEdge() : v1(nullptr), v2(nullptr), d1(), d2(), loop(nullptr) {}


	std::unordered_set<DFace*> getFaces();


};