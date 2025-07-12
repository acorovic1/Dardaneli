#include "EdgeBVHNode.h"
#include "DVertex.h"

EdgeBVHNode::EdgeBVHNode() :box(), left(nullptr), right(nullptr), edge(nullptr) {}

EdgeBVHNode::EdgeBVHNode( DEdge* e) :box(std::vector<glm::vec3>{ e->v1->position, e->v2->position }),
left(nullptr), right(nullptr), edge(e){}

EdgeBVHNode::EdgeBVHNode(EdgeBVHNode* a, EdgeBVHNode* b) :box(a->box, b->box), left(a), right(b), edge(nullptr) {};


bool EdgeBVHNode::Hit(const Ray& ray, std::vector<DEdge*>& edgesHit)
{
	if (!this)return false;
	// kako je lijepo biti glup
	if (this->box.intersectRayAABB(ray))
	{
		if (this->edge)
			edgesHit.push_back(this->edge);

		if (this->left)
			this->left->Hit(ray, edgesHit);
		if (this->right)
			this->right->Hit(ray, edgesHit);
	}


	return false;
}

void EdgeBVHNode::refitNode(Mesh& mesh)
{

	if (!this->left && !this->right)
	{
		std::pair<int, int> edgeVertices = mesh.getEdgeIndices(this->edge);
		this->box = AABB(mesh.getModelXVertex(edgeVertices.first), mesh.getModelXVertex(edgeVertices.second));
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



void EdgeBVHNode::Draw(Camera& camera, Shader& shader) {
	box.Draw(camera, shader);
}