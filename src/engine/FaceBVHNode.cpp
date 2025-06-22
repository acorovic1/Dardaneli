#include "FaceBVHNode.h"


FaceBVHNode::FaceBVHNode():box(),left(nullptr),right(nullptr),face(nullptr){}

FaceBVHNode::FaceBVHNode(std::vector<glm::vec3> vertices, DFace* f) :box(vertices), left(nullptr), right(nullptr),face(f){}

FaceBVHNode::FaceBVHNode(FaceBVHNode* a, FaceBVHNode* b) :box(a->box, b->box), left(a), right(b), face(nullptr) {};


bool FaceBVHNode::Hit(const Ray& ray, std::vector<DFace*>& facesHit)
{

	// kako je lijepo biti glup
	if (this->box.intersectRayAABB(ray))
	{
		if (this->face) 
			facesHit.push_back(this->face); 	

		if (this->left)
			this->left->Hit(ray, facesHit);
		if (this->right)
			this->right->Hit(ray, facesHit);
	}


	return false;
}

void FaceBVHNode::refitNode()
{
}



void FaceBVHNode::Draw(Camera& camera, Shader& shader) {
	box.Draw(camera, shader);
}