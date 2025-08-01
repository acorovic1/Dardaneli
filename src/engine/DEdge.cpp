#include "DEdge.h"
#include "DDiskLink.h"
#include "DFace.h"
#include "DLoop.h"



DEdge::DEdge(DVertex* a, DVertex* b) : loop(nullptr), d1(), d2()
{
	v1 = a;
	v2 = b;


	d1.next = this;
	d1.prev = this;
	d2.next = this;
	d2.prev = this;

	if (v1->e)
		addToDisk(v1->e, v1);
	else
		v1->e = this;

	if (v2->e)
		addToDisk(v2->e, v2);
	else v2->e = this;

}

void DEdge::connectLoopToEdge(DLoop* loop)
{

	if (!this->loop)
	{
		this->loop = loop;

		return;
	}

	if (this->loop == loop)return;

	if (this->loop->radialNext)
	{
		DLoop* temp = this->loop->radialNext;

		this->loop->radialNext = loop;
		loop->radialNext = temp;

		temp->radialPrev = loop;
		loop->radialPrev = this->loop;
	}
	else
	{
		this->loop->radialNext = loop;
		this->loop->radialPrev = loop;

		loop->radialNext = this->loop;
		loop->radialPrev = this->loop;
	}




}

void DEdge::removeFromDisk()
{
	// DISK 1
	if (d1.prev == this && d1.next == this) // if this is the only edge in the disk
	{
		d1.prev = nullptr;
		d1.next = nullptr;
	}
	else
	{



		if (d1.prev)
		{

			if (d1.prev->v1 == v1)
				d1.prev->d1.next = d1.next;
			else if (d1.prev->v2 == v1)
				d1.prev->d2.next = d1.next;
			else std::cerr << "\n\n\t ERROR DEdge.removeFromDisk 1";

		}

		if (d1.next)
		{
			if (d1.next->v1 == v1)
				d1.next->d1.prev = d1.prev;
			else if (d1.next->v2 == v1)
				d1.next->d2.prev = d1.prev;
			else std::cerr << "\n\n\t ERROR DEdge.removeFromDisk 2";
		}

	}


	// DISK 2
	if (d2.prev == this && d2.next == this) // if this is the only edge in the disk
	{
		d2.prev = nullptr;
		d2.next = nullptr;

		return;
	}
	else
	{



		if (d2.prev)
		{

			if (d2.prev->v1 == v2)
				d2.prev->d1.next = d2.next;
			else if (d2.prev->v2 == v2)
				d2.prev->d2.next = d2.next;
			else std::cerr << "\n\n\t ERROR DEdge.removeFromDisk 3";

		}

		if (d2.next)
		{
			if (d2.next->v1 == v2)
				d2.next->d1.prev = d2.prev;
			else if (d2.next->v2 == v2)
				d2.next->d2.prev = d2.prev;
			else std::cerr << "\n\n\t ERROR DEdge.removeFromDisk 4";
		}



	}

}

void DEdge::addToDisk(DEdge* pivotEdge, DVertex* pivot)
{
	DDiskLink& disk = (pivotEdge->v1 == pivot) ? pivotEdge->d1 : pivotEdge->d2;

	if (disk.next != pivotEdge)// more than one this around the vertex
	{
		//std::cout << "\n\n\t\t Aloha";
		DEdge* temp = disk.next;

		disk.next = this;

		if (this->v1 == pivot)
		{

			this->d1.next = temp;
			this->d1.prev = pivotEdge;

		}
		else if (this->v2 == pivot)
		{
			this->d2.next = temp;
			this->d2.prev = pivotEdge;

		}
		else std::cerr << "\n\n\tERROR 1\t addEdgeToDisk\n\n";


		if (temp->v1 == pivot)
		{
			temp->d1.prev = this;
		}
		else if (temp->v2 == pivot)
		{
			temp->d2.prev = this;
		}
		else std::cerr << "\n\n\tERROR 2\t addEdgeToDisk\n\n";
	}
	else { // if the pivot vertex only has 1 edge --  pivotEdge


		//std::cout << "\n\n\t\t HEEEEEEEELLoooOOOooo";
		disk.next = this;
		disk.prev = this;


		if (this->v1 == pivot)
		{

			this->d1.next = pivotEdge;
			this->d1.prev = pivotEdge;

		}
		else if (this->v2 == pivot)
		{
			this->d2.next = pivotEdge;
			this->d2.prev = pivotEdge;

		}
		else std::cerr << "\n\n\tERROR 1\t addEdgeToDisk\n\n";


	}

}

std::unordered_set<DFace*> DEdge::getFaces()
{
	if (!loop)
		return std::unordered_set<DFace*>();

	std::unordered_set<DFace*> returnSet;

	DLoop* temp = loop;
	do
	{
		returnSet.insert(temp->face);

		if (temp == temp->radialNext)break;

		temp = temp->radialNext;

	} while (temp != loop);


	return returnSet;
}

DVertex* DEdge::commonVertex(DEdge* edge)
{

	if (edge->v1 == this->v1 || edge->v1 == this->v2)
		return edge->v1;
	else if (edge->v2 == this->v1 || edge->v2 == this->v2)
		return edge->v2;



	std::cerr << "\n\n\t Edge.commonVertex ---> no common vertex";
	return nullptr;
}
