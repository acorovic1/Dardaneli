#pragma once

#include "BoundingVolumes/AABB.h"
#include "Mesh/Mesh.h"

#include "algorithm"

// treba destructor pod hitno
struct  EdgeBVHNode
{
	AABB box;
	EdgeBVHNode* left, * right;

	DEdge* edge;


	// preuzeto od BVHNode.h samo promijenjen index vector u face pokazivac


	// komentar ispod napisan dok nije bio vektor intova vec samo int... imaj to na umu

// moze se izbaciti kada se izbaci  Mesh iz AABB.h , prije toga ne moze zbog kruznih dependencija tj. #include "xyz.h"
// tada u konstruktoru objekta/mesha napravi AABB, to ce zamijeniti konstrukciju leaf nodeova
// kasnije kada se dodje do leaf node-a samo izvuci index

// napravi novi struct leafAABB nasljedjen iz obicnog AABBa, od atributa sadrzi samo indeks objekta
// taj indeks mogu koristiti u refitu;

	EdgeBVHNode();

	EdgeBVHNode(EdgeBVHNode* a, EdgeBVHNode* b);

	EdgeBVHNode( DEdge* e);

	bool Hit(const Ray& ray, std::vector<DEdge*>& edgesHit);

	void refitNode(Mesh& mesh);

	void Draw(Camera& camera, Shader& shader);
};
