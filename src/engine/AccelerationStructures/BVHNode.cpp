#include "BVHNode.h"

BVHNode::BVHNode() :box(), left(nullptr), right(nullptr), index(-1) {}

BVHNode::BVHNode(Object* object) :box(object), left(nullptr), right(nullptr), index{ object->getIndex() } {}
BVHNode::BVHNode(glm::vec3& vertex, unsigned int i) :box(vertex), left(nullptr), right(nullptr), index{ i } {}
BVHNode::BVHNode(float x, float y, float z, unsigned int index): box(glm::vec3(x,y,z)),left(nullptr),right(nullptr),index{index}{}

BVHNode::BVHNode(glm::vec3 a, glm::vec3 b, GLuint start, GLuint end) :box(a,b), left(nullptr), right(nullptr), index{ start,end } {}


BVHNode::BVHNode(BVHNode* a, BVHNode* b) :box(a->box, b->box), left(a), right(b), index{ std::numeric_limits<unsigned int>::max() } {}

bool BVHNode::Hit(const Ray& ray, std::vector<int>& index)
{
	if (!this)return false;
	// problem sa prvim nacinom: ako je vise objekata kolinearno i Ray ide u tom pravcu, onda podjela na lijevo i desno nema smisla

	// trenutno--> napraviti da ne ubacujem -1 u index vec samo listove.. i izbaciti erase iz application.cpp.. tada manuelno postaviti -1 ako je vector prazan

	// kako je lijepo biti glup
	if (this->box.intersectRayAABB(ray))
	{
		if (this->index.size() > 2) // for face selection
			index.push_back(this->index.size()); // this vector input represents the number of vertices of a face

		index.insert(index.end(), this->index.begin(), this->index.end());

		if (this->left)
			this->left->Hit(ray, index);
		if (this->right)
			this->right->Hit(ray, index);
	}

	if (!index.size())index.push_back(-1); // miss

	return false;
}

void BVHNode::refitNode()
{
	if (!this->left && !this->right)
	{
		this->box = AABB(objectSingleton->getObject(this->index[0]));
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
void BVHNode::refitNodeVertex(Mesh* mesh)
{
	if (!this->left && !this->right) // if its a leaf node
	{
		this->box = AABB(mesh->getModelXVertex(this->index[0]));
	}
	else
	{
		this->left->refitNodeVertex(mesh);
		this->right->refitNodeVertex(mesh);

		if (this->left && this->right)
			this->box = AABB(this->left->box, this->right->box);
		else if (this->left)
			this->box = AABB(this->left->box);
		else this->box = AABB(this->right->box);
	}
}
void BVHNode::refitNodeEdge(Mesh& mesh)
{
	if (!this->left && !this->right)
	{
		this->box = AABB(mesh.getModelXVertex(this->index[0]), mesh.getModelXVertex(this->index[0]));
	}
	else
	{
		this->left->refitNodeEdge(mesh);
		this->right->refitNodeEdge(mesh);

		if (this->left && this->right)
			this->box = AABB(this->left->box, this->right->box);
		else if (this->left)
			this->box = AABB(this->left->box);
		else this->box = AABB(this->right->box);
	}
}



void BVHNode::refitNodeUVVertex(Mesh& mesh)
{
	if (!this->left && !this->right) // if its a leaf node
	{
		auto uv = mesh.getUVVertex(this->index[0]);
		this->box = AABB(glm::vec3(uv->uv.x, uv->uv.y, 0.0f));
	}
	else
	{
		this->left->refitNodeUVVertex(mesh);
		this->right->refitNodeUVVertex(mesh);

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