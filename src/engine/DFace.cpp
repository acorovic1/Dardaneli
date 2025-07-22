#include "DFace.h"
#include "DLoop.h"



std::unordered_set<DVertex*> DFace::getVertices()const
{
	std::unordered_set<DVertex*> returnVec;

	DLoop* temp = loop;
	do {
		returnVec.insert(temp->tip);
		temp = temp->next;

	} while (temp != loop);


	return returnVec;

};
std::unordered_set<DEdge*> DFace::getEdges()const
{
	std::unordered_set<DEdge*> returnVec;

	DLoop* temp = loop;
	do {
		returnVec.insert(temp->edge);
		temp = temp->next;

	} while (temp != loop);


	return returnVec;

}
std::vector<DVertex*> DFace::getVerticesVector() const
{
	std::vector<DVertex*> returnVec;

	DLoop* temp = loop;
	do {
		returnVec.push_back(temp->tip);
		temp = temp->next;

	} while (temp != loop);


	return returnVec;
}
std::unordered_set<DLoop*> DFace::getLoops()
{

	std::unordered_set<DLoop*>returnVec;

	DLoop* temp = loop;

	do {
		returnVec.insert(temp);
		temp = temp->next;
	} while (temp != loop);

	return returnVec;
}
;
