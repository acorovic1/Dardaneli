#include "FaceBVHNode.h"
#include "DFace.h"


FaceBVHNode::FaceBVHNode():box(),left(nullptr),right(nullptr),face(nullptr){}

FaceBVHNode::FaceBVHNode(std::vector<glm::vec3> vertices, DFace* f) :box(vertices), left(nullptr), right(nullptr),face(f){}

FaceBVHNode::FaceBVHNode(FaceBVHNode* a, FaceBVHNode* b) :box(a->box, b->box), left(a), right(b), face(nullptr) {};


bool FaceBVHNode::Hit(const Ray& ray, std::vector<DFace*>& facesHit)
{
	if (!this)return false;
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

void FaceBVHNode::refitNode(Mesh& mesh)
{
	if (!this->left && !this->right)
	{
		auto faceVertices = face->getVerticesVector();
		std::vector<glm::vec3>faceVertexPositions;
		for (auto vert : faceVertices)
			faceVertexPositions.push_back(vert->position);

		this->box = AABB(faceVertexPositions);
	}
	else
	{
		this->left->refitNode(mesh);
		this->right->refitNode(mesh);

		if (this->left && this->right)
			this->box = AABB(this->left->box, this->right->box);
		else if (this->left)
			this->box = AABB(this->left->box);
		else this->box = AABB(this->right->box);
	}

}



void FaceBVHNode::Draw(Camera& camera, Shader& shader) {
	box.Draw(camera, shader);
}