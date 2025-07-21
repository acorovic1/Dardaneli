#pragma once

//#include "DEdge.h"
struct DEdge;

// circular lists
struct DDiskLink {
	DEdge* next;
	DEdge* prev;


	DDiskLink() : next(nullptr), prev(nullptr) {}
	DDiskLink(DEdge* n, DEdge* p) : next(n), prev(p) {}
	DDiskLink(const DDiskLink&  disk) {
		
		next = disk.next;prev = disk.prev; };
	DDiskLink& operator=(const DDiskLink& other) {
		if (this != &other) {
			next = other.next;
			prev = other.prev;
		}
		return *this;
	}
};