#pragma once

#include "AABB.h"
#include "Mesh.h"

#include "algorithm"

// treba destructor pod hitno
struct  FaceBVHNode
{
	AABB box;
	FaceBVHNode* left, * right;

	DFace* face;


	// preuzeto od BVHNode.h samo promijenjen index vector u face pokazivac
	 
	
	// komentar ispod napisan dok nije bio vektor intova vec samo int... imaj to na umu

// moze se izbaciti kada se izbaci  Mesh iz AABB.h , prije toga ne moze zbog kruznih dependencija tj. #include "xyz.h"
// tada u konstruktoru objekta/mesha napravi AABB, to ce zamijeniti konstrukciju leaf nodeova
// kasnije kada se dodje do leaf node-a samo izvuci index

// napravi novi struct leafAABB nasljedjen iz obicnog AABBa, od atributa sadrzi samo indeks objekta
// taj indeks mogu koristiti u refitu;

	FaceBVHNode();

	FaceBVHNode(FaceBVHNode* a, FaceBVHNode* b);

	FaceBVHNode(std::vector<glm::vec3> vertices, DFace* f);

	bool Hit(const Ray& ray, std::vector<DFace*>& facesHit);

	void refitNode();

	void Draw(Camera& camera, Shader& shader);
};
