#include "DEdge.h"
#include "DDiskLink.h"
#include "DFace.h"
#include "DLoop.h"


std::unordered_set<DFace*> DEdge::getFaces()
{
	if (!loop)
		return std::unordered_set<DFace*>();

	std::unordered_set<DFace*> returnSet;

	DLoop* temp = loop;
	do
	{
		returnSet.insert(temp->face);

		temp = temp->radialNext;

		if (!temp)break;


	} while (temp != loop);


	return returnSet;
}