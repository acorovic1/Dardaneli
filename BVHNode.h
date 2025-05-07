#pragma once

#include "AABB.h"
#include "Mesh.h"

#include "algorithm"

// treba destructor pod hitno
struct  BVHNode
{
	AABB box;
	BVHNode* left, * right;

	std::vector<GLuint> index;
	
		// komentar ispod napisan dok nije bio vektor intova vec samo int... imaj to na umu
		
	// moze se izbaciti kada se izbaci  Mesh iz AABB.h , prije toga ne moze zbog kruznih dependencija tj. #include "xyz.h"
	// tada u konstruktoru objekta/mesha napravi AABB, to ce zamijeniti konstrukciju leaf nodeova
	// kasnije kada se dodje do leaf node-a samo izvuci index

	// napravi novi struct leafAABB nasljedjen iz obicnog AABBa, od atributa sadrzi samo indeks objekta
	// taj indeks mogu koristiti u refitu;

	BVHNode();

	BVHNode(Object& object);
	BVHNode(glm::vec3& vertex,unsigned int index);
	BVHNode(Edge* e, GLuint start, GLuint end);

	BVHNode(BVHNode* a, BVHNode* b);

	bool Hit(const Ray& ray, std::vector<int>& index);

	void refitNode();
	void refitNodeVertex(Object& object);
	void refitNodeEdge(Mesh& mesh);

	void Draw(Camera& camera, Shader& shader);
};
