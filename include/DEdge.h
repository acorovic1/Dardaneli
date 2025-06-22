#pragma once

#include "DDiskLink.h"

struct DVertex;
//struct DDiskLink;
struct DFace;
struct DLoop;

struct DEdge{

	DVertex* v1;
	DVertex* v2;

	DDiskLink d1;
	DDiskLink d2;

	DLoop* loop;
	 
	DEdge() : v1(nullptr), v2(nullptr), d1(), d2(), loop(nullptr) {}

};