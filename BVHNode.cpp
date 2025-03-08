#include "BVHNode.h"



BVHNode::BVHNode() :box(), left(nullptr), right(nullptr),index(-1) {}
BVHNode::BVHNode(Object &object):box(object),left(nullptr),right(nullptr),index(object.getIndex()){}
BVHNode::BVHNode(glm::vec3& vertex,int i):box(vertex),left(nullptr),right(nullptr),index(i){}

BVHNode::BVHNode(BVHNode *a,BVHNode* b):box(a->box,b->box),left(a),right(b), index(-1) {}

bool BVHNode::Hit(const Ray& ray,std::vector<int> &index)  
{	
	// problem sa prvim nacinom: ako je vise objekata kolinearno i Ray ide u tom pravcu, onda podjela na lijevo i desno nema smisla
	
	// trenutno--> napraviti da ne ubacujem -1 u index vec samo listove.. i izbaciti erase iz application.cpp.. tada manuelno postaviti -1 ako je vector prazan
	
	if(this->left)
		if (this->left->box.intersectRayAABB(ray))
		{
			index.push_back(this->index);
			this->left->Hit(ray, index);
		}
	if(this->right)//
		if (this->right->box.intersectRayAABB(ray))
		{

			index.push_back(this->index);
			this->right->Hit(ray,index);
		}

	if (!this->left && !this->right)  // stavljeno da pokrije slucaj kada je samo jedan objekat na sceni...									
		if(this->box.intersectRayAABB(ray)) //znaci bvh root je upravo taj objekat a on sam nema djece tako da left i right su nullptr
			index.push_back(this->index);
	if (!index.size())index.push_back(-1);
		
	return false;

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

		this->box = AABB(object.getVertexXmodel(this->index));

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