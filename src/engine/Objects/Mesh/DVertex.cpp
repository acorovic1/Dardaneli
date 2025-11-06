#include "Mesh/DVertex.h"
#include "Mesh/DEdge.h"
#include "Mesh/DLoop.h"


std::unordered_set<DVertex*>DVertex::getAdjecentVertices()const {

	std::unordered_set<DVertex*> returnVec;
	DDiskLink* disk;

	DEdge* temp = e;
	do {
		if (temp->v2 == this)
		{
			returnVec.insert(temp->v1);
			disk = &temp->d2;
		}
		else
		{
			returnVec.insert(temp->v2);
			disk = &temp->d1;
		}



		temp = disk->next;

	} while (temp != e);


	return returnVec;

}
std::unordered_set<DEdge*> DVertex::getAdjecentEdges()const {

	if (e == nullptr) return std::unordered_set<DEdge*>();

	std::unordered_set<DEdge*> returnVec;

	DDiskLink* disk;

	DEdge* temp = e;
	do {
		/// belaj pravi ova sto se obrise opet ispade edge sa tackama obje te
		returnVec.insert(temp);

		if (temp->v1 == this)
			disk = &temp->d1;
		else if (temp->v2 == this)
			disk = &temp->d2;
		else {
			std::cerr << "NESTA NE VALJA vertex.GetAdjecentEdges()";
			break;
		}
		//disk = (temp->v1 == this) ? &temp->d1 : &temp->d2;

		temp = disk->next;

	} while (temp != e);


	return returnVec;

}
std::unordered_set<DFace*> DVertex::getAdjecentFaces() const {
	if (e == nullptr) return std::unordered_set<DFace*>();

	std::unordered_set<DFace*> returnVec;
	DDiskLink* disk;

	DEdge* temp = e;
	DLoop* tempRadial = nullptr;
	do {
		if (temp->loop)
		{
			tempRadial = temp->loop;
			do {
				returnVec.insert(tempRadial->face);

				if (tempRadial->radialNext==tempRadial)break;

				tempRadial = tempRadial->radialNext;

			} while (tempRadial != temp->loop);

		}
		disk = (temp->v1 == this) ? &temp->d1 : &temp->d2;

		temp = disk->next;

	} while (temp != e);

	returnVec.erase(nullptr);

	return returnVec;

}



DVertex::DVertex() : e(nullptr) {}
DVertex::DVertex(glm::vec3 pos, glm::vec3 norm ) : position(pos), normal(norm), e(nullptr) {}
DVertex::DVertex(const DVertex& vertex) {

	this->position = vertex.position;
	this->normal = vertex.normal;
	e = nullptr;
}


bool DVertex::operator==(const DVertex& vert) const{	return glm::all(glm::epsilonEqual(position, vert.position, 0.001f));}

void DVertex::translate(glm::vec3 offset) {
	position += offset;
	// calculateNormals();
};

void DVertex::translate(float x, float y, float z) {
	position.x += x;
	position.y += y;
	position.z += z;
	// calculateNormals();
};

void DVertex::translate(float* offset) {
	position.x += offset[0];
	position.y += offset[1];
	position.z += offset[2];
	
	// calculateNormals();
};