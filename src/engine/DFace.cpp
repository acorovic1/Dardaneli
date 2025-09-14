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
std::unordered_set<DLoop*> DFace::getLoops()const
{

	std::unordered_set<DLoop*>returnSet;

	DLoop* temp = loop;

	do {
		returnSet.insert(temp);
		temp = temp->next;
	} while (temp != loop);

	return returnSet;
}
std::vector<DLoop*> DFace::getLoopsVector() const
{
	std::vector<DLoop*>returnVec;

	DLoop* temp = loop;

	do {
		returnVec.push_back(temp);
		temp = temp->next;
	} while (temp != loop);

	return returnVec;

	
}
std::unordered_set<DFace*> DFace::getAdjecentFaces() const
{
	std::unordered_set<DFace*>returnSet;


	DLoop* temp = loop;

	do {
		DLoop* anotherTemp = temp;

		do
		{
			returnSet.insert(anotherTemp->face);
			anotherTemp = anotherTemp->radialNext;

		} while (anotherTemp != temp);

		temp = temp->next;
	} while (temp != loop);

	returnSet.erase(const_cast<DFace*>(this));

	return returnSet;
}

void DFace::flip()
{
	DLoop* temp = loop;
	DVertex* tip = temp->tip;

	do
	{
		DLoop* prev = temp->prev;
		DLoop* next = temp->next;

		temp->tip = temp->prev->tip;
		temp->prev = next;
		temp->next = prev;

		temp = prev;

	} while (temp != loop);


	temp->prev->tip = tip;



}

