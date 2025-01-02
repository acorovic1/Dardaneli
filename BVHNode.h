#pragma once



#include "AABB.h"


#include "algorithm"



struct  BVHNode
{

	AABB box;
	BVHNode* left, *right;

	GLuint index = -123;	// moze se izbaciti kada se izbaci  Mesh iz AABB.h , prije toga ne moze zbog kruznih dependencija tj. #include "xyz.h"
							// tada u konstruktoru objekta/mesha napravi AABB, to ce zamijeniti konstrukciju leaf nodeova
							// kasnije kada se dodje do leaf node-a samo izvuci index

							// napravi novi struct leafAABB nasljedjen iz obicnog AABBa, od atributa sadrzi samo indeks objekta
							// taj indeks mogu koristiti u refitu;

	BVHNode();
	BVHNode(Object &object);
	BVHNode(glm::vec3 &vertex, int index);
	BVHNode(BVHNode* a, BVHNode* b);

	bool Hit(const Ray& ray, int& index);
	void refitNode();
	void refitNodeVertex(Object& object);

	void Draw(Camera& camera, Shader& shader);

	
};




