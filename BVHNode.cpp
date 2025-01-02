#include "BVHNode.h"



BVHNode::BVHNode() :box(), left(nullptr), right(nullptr),index(-1) {}
BVHNode::BVHNode(Object &object):box(object),left(nullptr),right(nullptr),index(object.getIndex()){}
BVHNode::BVHNode(glm::vec3& vertex,int i):box(vertex),left(nullptr),right(nullptr),index(i){}

BVHNode::BVHNode(BVHNode *a,BVHNode* b):box(a->box,b->box),left(a),right(b), index(-1) {}

bool BVHNode::Hit(const Ray& ray,int &index) 
{	
	
	if (this == nullptr) return true;
	index = this->index;
	if (this->box.intersectRayAABB(ray))
	{
		if (this->left->Hit(ray,index)) return true;
		else return this->right->Hit(ray,index);
		
	}
	else
	{
		index = -1; // miss
		return false;
	}
}

void BVHNode::refitNode()
{

		
	if (!this->left && !this->right)
	{
		this->box = AABB(*objectSingleton->getObject(this->index));

		
	}
	else
	{
		this->left->refitNode();
		this->right->refitNode();

		if (this->left && this->right)
			this->box = AABB(this->left->box, this->right->box);
		else if (this->left)
			this->box = AABB(this->left->box);
		else this->box = AABB(this->right->box);
	}



}
void BVHNode::refitNodeVertex(Object&object)
{


	if (!this->left && !this->right)
	{
		this->box = AABB(object.getVerticesReference().at(this->index).getPosition());

	}
	else
	{
		this->left->refitNodeVertex(object);
		this->right->refitNodeVertex(object);

		if (this->left && this->right)
			this->box = AABB(this->left->box, this->right->box);
		else if (this->left)
			this->box = AABB(this->left->box);
		else this->box = AABB(this->right->box);
	}



}


void BVHNode::Draw(Camera& camera, Shader& shader) {
	
	box.Draw(camera, shader);
}