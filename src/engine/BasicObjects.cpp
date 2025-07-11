#include "BasicObjects.h"
#include "DFace.h"
#include "DLoop.h"
#include "DDiskLink.h"
#include "UnorderedPair.h"
#include "PairHash.h"
#include <unordered_map>
#include <map>

// gui for dmesh needs loop.tip



//// DVertex vector (or the vertices themselves) need to be dinamically allocated due to the edge.tip being a pointer
//// otherwise the vector is destroyed after the function ends and the vector(and its contents) no longer exist so the tip 
//// becomes a dangling pointer
//
//
void addPlane()
{

	std::vector<DVertex>* vertices = new std::vector<DVertex>{
		DVertex{glm::vec3(-0.5f, 0.0f, 0.5f),glm::vec3(-0.5f, 0.0f, 0.5f)},
		DVertex{glm::vec3(0.5f, 0.0f, 0.5f), glm::vec3(0.5f, 0.0f, 0.5f)},
		DVertex{glm::vec3(0.5f, 0.0f, -0.5f), glm::vec3(0.5f, 0.0f, -0.5f)},
		DVertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(-0.5f, 0.0f, -0.5f)}
	};
	std::vector<GLuint> indices{
		0, 1, 2,
		2, 3, 0
	};
	std::vector<GLuint>edgeVertices
	{
		0,1,
		1,2,
		2,3,
		3,0
	};


	DEdge* e1 = new DEdge();
	DEdge* e2 = new DEdge();
	DEdge* e3 = new DEdge();
	DEdge* e4 = new DEdge();

	DLoop* l1 = new DLoop();
	DLoop* l2 = new DLoop();
	DLoop* l3 = new DLoop();
	DLoop* l4 = new DLoop();

	DDiskLink d1(e1, e1);
	DDiskLink d2(e2, e2);
	DDiskLink d3(e3, e3);
	DDiskLink d4(e4, e4);


	DFace* face = new DFace();

	// DVertex data
	(*vertices)[0].e = e1;
	(*vertices)[1].e = e2;
	(*vertices)[2].e = e3;
	(*vertices)[3].e = e4;

	// DEdge data
	e1->v1 = &(*vertices)[0];
	e1->v2 = &(*vertices)[1];
	e1->d1 = d4;
	e1->d2 = d2;
	e1->loop = l1;

	e2->v1 = &(*vertices)[1];
	e2->v2 = &(*vertices)[2];
	e2->d1 = d1;
	e2->d2 = d3;
	e2->loop = l2;

	e3->v1 = &(*vertices)[2];
	e3->v2 = &(*vertices)[3];
	e3->d1 = d2;
	e3->d2 = d4;
	e3->loop = l3;

	e4->v1 = &(*vertices)[3];
	e4->v2 = &(*vertices)[0];

	e4->d1 = d3;
	e4->d2 = d1;

	e4->loop = l4;

	// DFace data
	face->loop = l1;

	// DLoop data

	l1->tip = &(*vertices)[1];
	l1->edge = e1;
	l1->face = face;
	l1->prev = l4;
	l1->next = l2;

	l2->tip = &(*vertices)[2];
	l2->edge = e2;
	l2->face = face;
	l2->prev = l1;
	l2->next = l3;

	l3->tip = &(*vertices)[3];
	l3->edge = e3;
	l3->face = face;
	l3->prev = l2;
	l3->next = l4;

	l4->tip = &(*vertices)[0];
	l4->edge = e4;
	l4->face = face;
	l4->prev = l3;
	l4->next = l1;

	DLoop* r1 = new DLoop();
	DLoop* r2 = new DLoop();
	DLoop* r3 = new DLoop();
	DLoop* r4 = new DLoop();

	l1->radialNext = r1; l1->radialPrev = r1;
	l2->radialNext = r2; l2->radialPrev = r2;
	l3->radialNext = r3; l3->radialPrev = r3;
	l4->radialNext = r4; l4->radialPrev = r4;

	r1->edge = e1;
	r2->edge = e2;
	r3->edge = e3;
	r4->edge = e4;

	r1->tip = &(*vertices)[0];
	r2->tip = &(*vertices)[1];
	r3->tip = &(*vertices)[2];
	r4->tip = &(*vertices)[3];

	r1->radialNext = l1; r1->radialPrev = l1;
	r2->radialNext = l2; r2->radialPrev = l2;
	r3->radialNext = l3; r3->radialPrev = l3;
	r4->radialNext = l4; r4->radialPrev = l4;

	new Mesh(
		"Plane",
		vertices,
		indices,
		edgeVertices

	);
}

void addCube()
{
	std::vector<DVertex>* vertices = new std::vector<DVertex>{

	DVertex{ glm::vec3(0.5f, 0.5f, 0.5f) ,glm::vec3(0.5f, 0.5f, 0.5f) }, // V0
	DVertex{ glm::vec3(0.5f, 0.5f, -0.5f) ,glm::vec3(0.5f,  0.5f, -0.5f) }, // V1
	DVertex{ glm::vec3(0.5f, -0.5f,  0.5f) ,glm::vec3(0.5f,  -0.5f,  0.5f) }, // V2
	DVertex{ glm::vec3(0.5f, -0.5f,  -0.5f) ,glm::vec3(0.5f, -0.5f,  -0.5f) }, // V3


	DVertex{ glm::vec3(-0.5f, 0.5f, 0.5f) , glm::vec3(-0.5f, 0.5f, 0.5f) }, // V4
	DVertex{ glm::vec3(-0.5f, 0.5f, -0.5f) , glm::vec3(-0.5f,  0.5f, -0.5f) }, // V5
	DVertex{ glm::vec3(-0.5f, -0.5f,  0.5f) , glm::vec3(-0.5f,  -0.5f,  0.5f) }, // V6
	DVertex{ glm::vec3(-0.5f, -0.5f,  -0.5f) , glm::vec3(-0.5f, -0.5f,  -0.5f) }, // V7
	};
	std::vector<GLuint> indices{
		// Right face
		0, 2, 1,
		1, 2, 3,
		// Left face
		5, 7, 4,
		4, 7, 6,
		// Front face
		4, 6, 0,
		0, 6, 2,
		// Back face
		1, 3, 5,
		5, 3, 7,
		// Top face
		5, 4, 1,
		1, 4, 0,
		// Bottom face
		6, 7, 2,
		2, 7, 3
	};

	std::vector<GLuint>edgeIndices{
		//left face
		0,1,
		1,3,
		2,0,
		3,2,

		//right face
		4,5,
		5,7,
		6,4,
		7,6,

		// X-axis edges (leftovers)
		0,4,
		1,5,
		2,6,
		3,7,
	};

	// key should logically be of type UnorderedPair, but it does not really matter
	/* used for linking with loop data */
	std::unordered_map<UnorderedPair<int>, DEdge*> edgeMap{};

	/* used for linking disks with edges

	 *	each row holds edges for the vertex with the same index

	*/
	std::vector<std::vector<DEdge*>>diskEdges(vertices->size());

	/* used for linking loop.radial atributes*/
	std::unordered_multimap < UnorderedPair<int>, DLoop*, UnorderedPairHash<int> > loopMap{};

	for (int i = 0; i < edgeIndices.size(); i += 2)
	{
		DEdge* e1 = new DEdge();


		edgeMap[UnorderedPair<int>(edgeIndices[i], edgeIndices[i + 1])] = e1;

		// edge data
		e1->v1 = &(*vertices)[edgeIndices[i]];
		e1->v2 = &(*vertices)[edgeIndices[i + 1]];

		diskEdges[edgeIndices[i]].push_back(e1);
		diskEdges[edgeIndices[i + 1]].push_back(e1);


		// vertex data
		(*vertices)[edgeIndices[i]].e = e1;

	}

	for (int i = 0; i < indices.size(); i += 6)
	{
		DFace* face = new DFace();

		DEdge* e1 = edgeMap[UnorderedPair<int>(indices[i], indices[i + 1])];
		DEdge* e2 = edgeMap[UnorderedPair<int>(indices[i + 1], indices[i + 5])];
		DEdge* e3 = edgeMap[UnorderedPair<int>(indices[i + 5], indices[i + 2])];
		DEdge* e4 = edgeMap[UnorderedPair<int>(indices[i + 2], indices[i])];


		// loop data
		DLoop* l1 = new DLoop(&(*vertices)[indices[i + 1]], e1, face);
		DLoop* l2 = new DLoop(&(*vertices)[indices[i + 5]], e2, face);
		DLoop* l3 = new DLoop(&(*vertices)[indices[i + 2]], e3, face);
		DLoop* l4 = new DLoop(&(*vertices)[indices[i]], e4, face);

		l1->next = l2;	l1->prev = l4;
		l2->next = l3;	l2->prev = l1;
		l3->next = l4;	l3->prev = l2;
		l4->next = l1;	l4->prev = l3;

		// will be used for radial attributes
		loopMap.emplace(UnorderedPair<int>(indices[i], indices[i + 1]), l1);
		loopMap.emplace(UnorderedPair<int>(indices[i + 1], indices[i + 5]), l2);
		loopMap.emplace(UnorderedPair<int>(indices[i + 5], indices[i + 2]), l3);
		loopMap.emplace(UnorderedPair<int>(indices[i + 2], indices[i]), l4);


		// face data
		face->loop = l1;



		// edge data
		e1->loop = l1;
		e2->loop = l2;
		e3->loop = l3;
		e4->loop = l4;


	}

	for (int i = 0; i < edgeIndices.size(); i += 2)
	{
		/*
			equal_range returns iterators to the original map
			where iterator points to the "first" element with that key
			and ++iterator to the next element with the same key
		*/

		auto it = loopMap.equal_range(UnorderedPair<int>(edgeIndices[i], edgeIndices[i + 1]));

		DLoop* loop1 = it.first->second;
		DLoop* loop2 = (++it.first)->second;

		loop1->radialNext = loop2;
		loop1->radialPrev = loop2;

		loop2->radialNext = loop1;
		loop2->radialPrev = loop1;

	}

	for (int i = 0; i < vertices->size(); i++)
	{
		for (int j = 0; j < diskEdges[i].size(); j++)
		{
			DEdge* e = diskEdges[i][j];
			if (&(*vertices)[i] == e->v1)
			{

				if (j == 0)
				{
					e->d1.next = diskEdges[i][j + 1];
					e->d1.prev = diskEdges[i][diskEdges[i].size() - 1];
				}
				else if (j == diskEdges[i].size() - 1)
				{
					e->d1.next = diskEdges[i][0];
					e->d1.prev = diskEdges[i][j - 1];
				}
				else
				{
					e->d1.next = diskEdges[i][j + 1];
					e->d1.prev = diskEdges[i][j - 1];
				}
			}
			else if (&(*vertices)[i] == e->v2)
			{
				if (j == 0)
				{
					e->d2.next = diskEdges[i][j + 1];
					e->d2.prev = diskEdges[i][diskEdges[i].size() - 1];
				}
				else if (j == diskEdges[i].size() - 1)
				{
					e->d2.next = diskEdges[i][0];
					e->d2.prev = diskEdges[i][j - 1];
				}
				else
				{
					e->d2.next = diskEdges[i][j + 1];
					e->d2.prev = diskEdges[i][j - 1];
				}
			}

		}



	}

	auto mesh = new Mesh(
		"Cube",
		vertices,
		indices,
		edgeIndices
	);

	for (int i = 0; i < vertices->size(); i++)
	{
		std::cout << "\n\n\nDisk edges for vertex: " << i;
		for (int j = 0; j < diskEdges[i].size(); j++)
		{
			DEdge* e = diskEdges[i][j];
			if (&(*vertices)[i] == e->v1)
			{

				if (j == 0)
				{
					auto next = mesh->getEdgeIndices(diskEdges[i][j + 1]);
					auto prev = mesh->getEdgeIndices(diskEdges[i][diskEdges[i].size() - 1]);

					std::cout << "\n\nnext edge: " << next.first << " " << next.second << "\nprev edge: " << prev.first << " " << prev.second;


				}
				else if (j == diskEdges[i].size() - 1)
				{
					auto next = mesh->getEdgeIndices(diskEdges[i][0]);
					auto prev = mesh->getEdgeIndices(diskEdges[i][j - 1]);

					std::cout << "\n\nnext edge: " << next.first << " " << next.second << "\nprev edge: " << prev.first << " " << prev.second;

				}
				else
				{
					auto next = mesh->getEdgeIndices(diskEdges[i][j + 1]);
					auto prev = mesh->getEdgeIndices(diskEdges[i][j - 1]);

					std::cout << "\n\nnext edge: " << next.first << " " << next.second << "\nprev edge: " << prev.first << " " << prev.second;

				}
			}
			else if (&(*vertices)[i] == e->v2)
			{
				if (j == 0)
				{
					auto next = mesh->getEdgeIndices(diskEdges[i][j + 1]);
					auto prev = mesh->getEdgeIndices(diskEdges[i][diskEdges[i].size() - 1]);

					std::cout << "\n\nnext edge: " << next.first << " " << next.second << "\nprev edge: " << prev.first << " " << prev.second;

				}
				else if (j == diskEdges[i].size() - 1)
				{
					auto next = mesh->getEdgeIndices(diskEdges[i][0]);
					auto prev = mesh->getEdgeIndices(diskEdges[i][j - 1]);

					std::cout << "\n\nnext edge: " << next.first << " " << next.second << "\nprev edge: " << prev.first << " " << prev.second;

				}
				else
				{
					auto next = mesh->getEdgeIndices(diskEdges[i][j + 1]);
					auto prev = mesh->getEdgeIndices(diskEdges[i][j - 1]);

					std::cout << "\n\nnext edge: " << next.first << " " << next.second << "\nprev edge: " << prev.first << " " << prev.second;


				}
			}

		}
	}

	for (int i = 0;i < diskEdges.size();i++)
	{
		std::cout << "\n\n\nDisk edges for vertex: " << i;
		for (auto y : diskEdges[i])
		{
			auto temp = mesh->getEdgeIndices(y);
			std::cout << "\nEdge: " << temp.first << " " << temp.second;
		}
	}

}

void addCircle(int numSegments, float radius)
{
	std::vector<DVertex>* vertices = new std::vector<DVertex>();
	std::vector<GLuint> indices;
	std::vector<GLuint> edgeIndices;

	// key should logically be of type UnorderedPair, but it does not really matter
	/* used for linking with loop data */
	std::vector<DEdge*> edgeVec;

	/* used for linking loop.radial atributes*/
	std::vector< DLoop* > loopVec;

	float angleStep = 2.0f * PI / numSegments;
	float angle, x, z;

	for (int i = 0; i < numSegments; ++i) {
		angle = angleStep * i;
		x = cos(angle) * radius;
		z = -sin(angle) * radius;

		vertices->push_back(DVertex{
			glm::vec3(x, 0.0f, z),   // Position
			glm::normalize(glm::vec3(x, 0.0f, z))  // Normal pointing up
			});
	}


	for (int i = 0; i < numSegments - 2; ++i) {
		indices.push_back(i + 2);       // left most
		indices.push_back(0);           // Anchor 
		indices.push_back(i + 1);       // middle

		edgeIndices.push_back(i);
		edgeIndices.push_back(i + 1);

	}
	// second to last vertex
	edgeIndices.push_back(numSegments - 2);
	edgeIndices.push_back(numSegments - 1);

	// last vertex
	edgeIndices.push_back(numSegments - 1);
	edgeIndices.push_back(0);

	DFace* face = new DFace();
	for (int i = 0; i < edgeIndices.size(); i += 2)
	{
		DEdge* e = new DEdge();

		edgeVec.push_back(e);

		// edge data
		e->v1 = &(*vertices)[edgeIndices[i]];
		e->v2 = &(*vertices)[edgeIndices[i + 1]];


		// vertex data
		(*vertices)[edgeIndices[i]].e = e;

		loopVec.push_back(new DLoop(&(*vertices)[edgeIndices[i + 1]], e, face));

		e->loop = loopVec.back();


	}
	face->loop = loopVec[0];

	int size = loopVec.size();


	for (int i = 0; i < size; ++i) {
		int next = (i + 1) % size;
		int prev = (i - 1 + size) % size;

		loopVec[i]->next = loopVec[next];
		loopVec[i]->prev = loopVec[prev];

		edgeVec[i]->d1.next = edgeVec[prev];
		edgeVec[i]->d1.prev = edgeVec[prev];

		edgeVec[i]->d2.next = edgeVec[next];
		edgeVec[i]->d2.prev = edgeVec[next];
	}


	new Mesh(
		"Circle",
		vertices,
		indices,
		edgeIndices
	);
};

void addSphere(int segments, int rings, float radius)
{
	std::vector<DVertex>* vertices = new std::vector<DVertex>();
	std::vector<GLuint> indices;
	std::vector<GLuint> edgeIndices;

	// key should logically be of type UnorderedPair, but it does not really matter
	/* used for linking with loop data */
	std::unordered_map<UnorderedPair<int>, DEdge*> edgeMap{};

	/* used for linking disks with edges

	 *	each row holds edges for the vertex with the same index

	*/
	std::vector<std::vector<DEdge*>>diskEdges(segments * (rings - 1) + 2);

	/* used for linking loop.radial atributes*/
	std::unordered_multimap < UnorderedPair<int>, DLoop*, UnorderedPairHash<int> > loopMap{};



	float segmentStep = 2 * PI / segments;
	float ringStep = PI / rings;

	float x, y, z;
	float radiusStep;
	int topLeftCorner, topRightCorner, bottomLeftCorner, bottomRightCorner;


	// top vertex
	vertices->push_back(DVertex{ glm::vec3(0.0f,radius,0.0f),glm::normalize(glm::vec3(0.0f,radius,0.0f)) });

	for (int i = 0; i < rings - 1; i++)
	{
		y = radius * sinf(halfPI - ringStep * (i + 1)); // found from the XY/ZY plane (side view)

		radiusStep = radius * cosf(halfPI - ringStep * (i + 1));  // found from the XY/ZY plane (side view)


		for (int j = 0; j < segments; j++)
		{

			x = radiusStep * cosf(segmentStep * j); // found from the XZ plane (top down view)
			z = radiusStep * sinf(segmentStep * j);

			vertices->push_back(DVertex{ glm::vec3(x,y,z),glm::normalize(glm::vec3(x,y,z)) });

			if (i == 0) // 1st ring of faces (triangles)
			{
				edgeIndices.push_back(0);
				edgeIndices.push_back(j + 1);

				DEdge* e = new DEdge();
				edgeMap[UnorderedPair<int>(0, j + 1)] = e;

				diskEdges[0].push_back(e);
				diskEdges[j + 1].push_back(e);


				if (j == segments - 1)
				{
					indices.push_back(1);
				}
				else
				{
					indices.push_back(j + 2); // next vertex
				}

				indices.push_back(j + 1); // current vertex
				indices.push_back(0);     // top vertex



			}
			else // middle part of the sphere (quads)
			{
				topLeftCorner = (i - 1) * segments + 1 + j + 1;
				topRightCorner = (i - 1) * segments + 1 + j;
				bottomLeftCorner = i * segments + 1 + j + 1;
				bottomRightCorner = i * segments + 1 + j;

				if (j == segments - 1) // on the last face, left side vertices are the start of the ring
				{
					topLeftCorner = (i - 1) * segments + 1;
					bottomLeftCorner = i * segments + 1;
				}


				indices.push_back(topLeftCorner);
				indices.push_back(bottomLeftCorner);
				indices.push_back(topRightCorner);

				indices.push_back(topRightCorner);
				indices.push_back(bottomLeftCorner);
				indices.push_back(bottomRightCorner);

				edgeIndices.push_back(topRightCorner);
				edgeIndices.push_back(topLeftCorner);

				edgeIndices.push_back(topRightCorner);
				edgeIndices.push_back(bottomRightCorner);

				DEdge* e1 = new DEdge();

				edgeMap[UnorderedPair<int>(topRightCorner, topLeftCorner)] = e1;
				diskEdges[topRightCorner].push_back(e1);
				diskEdges[topLeftCorner].push_back(e1);

				DEdge* e2 = new DEdge();

				edgeMap[UnorderedPair<int>(topRightCorner, bottomRightCorner)] = e2;
				diskEdges[topRightCorner].push_back(e2);
				diskEdges[bottomRightCorner].push_back(e2);
			}

		}

	}
	float bottomVertex = (rings - 1) * segments + 1;
	float currentVertex;
	for (int j = 0; j < segments; j++)// last ring of faces (triangles)
	{
		currentVertex = (rings - 2) * (segments)+1 + j;

		edgeIndices.push_back(bottomVertex);
		edgeIndices.push_back(currentVertex);

		DEdge* e = new DEdge();
		edgeMap[UnorderedPair<int>(bottomVertex, currentVertex)] = e;

		diskEdges[bottomVertex].push_back(e);
		diskEdges[currentVertex].push_back(e);

		edgeIndices.push_back(currentVertex);

		if (j == segments - 1)
		{
			indices.push_back(currentVertex - j);
			edgeIndices.push_back(currentVertex - j);

			DEdge* ee = new DEdge();
			edgeMap[UnorderedPair<int>(currentVertex, currentVertex - j)] = ee;
			diskEdges[currentVertex].push_back(ee);
			diskEdges[currentVertex - j].push_back(ee);

		}
		else
		{
			edgeIndices.push_back(currentVertex + 1);
			indices.push_back(currentVertex + 1); // next vertex

			DEdge* ee = new DEdge();
			edgeMap[UnorderedPair<int>(currentVertex, currentVertex + 1)] = ee;
			diskEdges[currentVertex].push_back(ee);
			diskEdges[currentVertex + 1].push_back(ee);

		}

		indices.push_back(bottomVertex);
		indices.push_back(currentVertex);

	}

	vertices->push_back(DVertex{ glm::vec3(0.0f,-radius,0.0f),glm::normalize(glm::vec3(0.0f,-radius,0.0f)) });

	(*vertices)[0].e = edgeMap[UnorderedPair<int>(0, 2)];


	for (int i = 0; i < indices.size();)
	{
		DFace* face = new DFace();
		if (i < 3 * segments || i >= 3 * segments + 6 * segments * (rings - 2)) // triangles
		{
			// indices[i] left vertex
			// indices[i+1] right vertex
			// indices[i+2]  top vertex
			//std::cout << indices[i] << " " << indices[i + 1] << " " << indices[i + 2] << "\n";

			DEdge* e1 = edgeMap[UnorderedPair<int>(indices[i], indices[i + 1])];
			DEdge* e2 = edgeMap[UnorderedPair<int>(indices[i + 1], indices[i + 2])];
			DEdge* e3 = edgeMap[UnorderedPair<int>(indices[i + 2], indices[i])];


			if (!(*vertices)[indices[i + 1]].e)
				(*vertices)[indices[i + 1]].e = e2;


			e1->v1 = &(*vertices)[indices[i]];
			e2->v1 = &(*vertices)[indices[i + 1]];
			e3->v1 = &(*vertices)[indices[i + 2]];

			e1->v2 = &(*vertices)[indices[i + 1]];
			e2->v2 = &(*vertices)[indices[i + 2]];
			e3->v2 = &(*vertices)[indices[i]];

			DLoop* l1 = new DLoop(e1->v2, e1, face);
			DLoop* l2 = new DLoop(e2->v2, e2, face);
			DLoop* l3 = new DLoop(e3->v2, e3, face);

			loopMap.emplace(UnorderedPair<int>(indices[i], indices[i + 1]), l1);
			loopMap.emplace(UnorderedPair<int>(indices[i + 1], indices[i + 2]), l2);
			loopMap.emplace(UnorderedPair<int>(indices[i + 2], indices[i]), l3);

			l1->next = l2; l1->prev = l3;
			l2->next = l3; l2->prev = l1;
			l3->next = l1; l3->prev = l2;

			e1->loop = l1;
			e2->loop = l2;
			e3->loop = l3;

			face->loop = l1;

			i += 3;
		}
		else if (i < 3 * segments + 6 * segments * (rings - 2)) // 3 * segments = number of indices for the top tris + 6*segments*(rings-2) = number of indices for the quads
		{
			// indices[i] is the top left index
			// indices[i+2] is the top right index
			// indices[i+1] is the bottom left index
			// indices[i+5] is the bottom right index

			//std::cout << indices[i] << " " << indices[i + 1] << " " << indices[i + 2]<<"\n";



			DEdge* e1 = edgeMap[UnorderedPair<int>(indices[i + 2], indices[i])];     // <--
			DEdge* e2 = edgeMap[UnorderedPair<int>(indices[i], indices[i + 1])];	 //   |
			DEdge* e3 = edgeMap[UnorderedPair<int>(indices[i + 1], indices[i + 5])]; // -->
			DEdge* e4 = edgeMap[UnorderedPair<int>(indices[i + 5], indices[i + 2])]; //	|


			if (!(*vertices)[indices[i + 5]].e)
				(*vertices)[indices[i + 5]].e = e4;

			e1->v1 = &(*vertices)[indices[i + 2]];
			e2->v1 = &(*vertices)[indices[i]];
			e3->v1 = &(*vertices)[indices[i + 1]];
			e4->v1 = &(*vertices)[indices[i + 5]];

			e1->v2 = &(*vertices)[indices[i]];
			e2->v2 = &(*vertices)[indices[i + 1]];
			e3->v2 = &(*vertices)[indices[i + 5]];
			e4->v2 = &(*vertices)[indices[i + 2]];


			DLoop* l1 = new DLoop(&(*vertices)[indices[i]], e1, face);
			DLoop* l2 = new DLoop(&(*vertices)[indices[i + 1]], e2, face);
			DLoop* l3 = new DLoop(&(*vertices)[indices[i + 5]], e3, face);
			DLoop* l4 = new DLoop(&(*vertices)[indices[i + 2]], e4, face);

			l1->next = l2;	l1->prev = l4;
			l2->next = l3;	l2->prev = l1;
			l3->next = l4;	l3->prev = l2;
			l4->next = l1;	l4->prev = l3;


			loopMap.emplace(UnorderedPair<int>(indices[i + 2], indices[i]), l1);
			loopMap.emplace(UnorderedPair<int>(indices[i], indices[i + 1]), l2);
			loopMap.emplace(UnorderedPair<int>(indices[i + 1], indices[i + 5]), l3);
			loopMap.emplace(UnorderedPair<int>(indices[i + 5], indices[i + 2]), l4);

			face->loop = l1;

			e1->loop = l1;
			e2->loop = l2;
			e3->loop = l3;
			e4->loop = l4;

			i += 6;
		}

	}

	for (int i = 0; i < edgeIndices.size(); i += 2)
	{
		/*
			equal_range returns iterators to the original map
			where iterator points to the "first" element with that key
			and ++iterator to the next element with the same key
		*/

		auto it = loopMap.equal_range(UnorderedPair<int>(edgeIndices[i], edgeIndices[i + 1]));

		DLoop* loop1 = it.first->second;
		DLoop* loop2 = (++it.first)->second;

		loop1->radialNext = loop2;
		loop1->radialPrev = loop2;

		loop2->radialNext = loop1;
		loop2->radialPrev = loop1;

	}
	for (int i = 0; i < vertices->size(); i++)
	{
		for (int j = 0; j < diskEdges[i].size(); j++)
		{
			DEdge* e = diskEdges[i][j];
			if (&(*vertices)[i] == e->v1)
			{

				if (j == 0)
				{
					e->d1.next = diskEdges[i][j + 1];
					e->d1.prev = diskEdges[i][diskEdges[i].size() - 1];
				}
				else if (j == diskEdges[i].size() - 1)
				{
					e->d1.next = diskEdges[i][0];
					e->d1.prev = diskEdges[i][j - 1];
				}
				else
				{
					e->d1.next = diskEdges[i][j + 1];
					e->d1.prev = diskEdges[i][j - 1];
				}
			}
			else if (&(*vertices)[i] == e->v2)
			{
				if (j == 0)
				{
					e->d2.next = diskEdges[i][j + 1];
					e->d2.prev = diskEdges[i][diskEdges[i].size() - 1];
				}
				else if (j == diskEdges[i].size() - 1)
				{
					e->d2.next = diskEdges[i][0];
					e->d2.prev = diskEdges[i][j - 1];
				}
				else
				{
					e->d2.next = diskEdges[i][j + 1];
					e->d2.prev = diskEdges[i][j - 1];
				}
			}
		}
		if (i == vertices->size() - 1)
		{
			std::cout << "ljulj";
		}


	}


	(*vertices).back().e = edgeMap[UnorderedPair<int>(vertices->size() - 1, vertices->size() - 2)];

	//std::cout << "\n\n Number of indices = " << indices.size()<<"\n";

	new Mesh(
		"Sphere",
		vertices,
		indices,
		edgeIndices
	);

}

void addCylinder(int numSegments, float height, float radius)
{
	std::vector<DVertex>* vertices = new std::vector<DVertex>();
	std::vector<GLuint> indices;
	std::vector<GLuint> edgeIndices;


	// key should logically be of type UnorderedPair, but it does not really matter
	/* used for linking with loop data */
	std::unordered_map<UnorderedPair<int>, DEdge*> edgeMap{};

	/* used for linking disks with edges

	 *	each row holds edges for the vertex with the same index

	*/
	std::vector<std::vector<DEdge*>>diskEdges(numSegments * 2);

	/* used for linking loop.radial atributes*/
	std::unordered_multimap < UnorderedPair<int>, DLoop*, UnorderedPairHash<int> > loopMap{};

	/*used for loop next/prev attributes*/
	std::vector< DLoop* > loopVecTop;
	std::vector< DLoop* > loopVecBottom;

	float angleStep = 2.0f * PI / numSegments;
	float angle, x, z;

	height /= 2;

	for (int i = 0; i < numSegments; ++i) {
		angle = angleStep * float(i);
		x = cos(angle) * radius;
		z = sin(angle) * radius;

		// Bottom circle vertices
		vertices->push_back(DVertex{
			glm::vec3(x, -height , z),
			glm::normalize(glm::vec3(x, -height, z))
			});

		// Top circle vertices
		vertices->push_back(DVertex{
			glm::vec3(x, height , z),
			glm::normalize(glm::vec3(x,height,z))
			});
	}

	int topLeft;
	int topRight;
	int bottomLeft;
	int bottomRight;
	// quads
	for (int i = 0; i < numSegments; i++)
	{
		topLeft = (i * 2 + 3) % (numSegments * 2);
		bottomLeft = (i * 2 + 2) % (numSegments * 2);
		topRight = i * 2 + 1;
		bottomRight = i * 2;

		indices.push_back(topLeft);
		indices.push_back(bottomLeft);
		indices.push_back(topRight);

		indices.push_back(topRight);
		indices.push_back(bottomLeft);
		indices.push_back(bottomRight);



		edgeIndices.push_back(topLeft);			// |   left edge
		edgeIndices.push_back(bottomLeft);		// V

		DEdge* e1 = new DEdge();
		edgeMap[UnorderedPair<int>(topLeft, bottomLeft)] = e1;
		diskEdges[topLeft].push_back(e1);
		diskEdges[bottomLeft].push_back(e1);


		edgeIndices.push_back(topRight);		// <--
		edgeIndices.push_back(topLeft);			// top edge	

		DEdge* e2 = new DEdge();
		edgeMap[UnorderedPair<int>(topRight, topLeft)] = e2;
		diskEdges[topRight].push_back(e2);
		diskEdges[topLeft].push_back(e2);


		edgeIndices.push_back(bottomRight);			//  bottom edge
		edgeIndices.push_back(bottomLeft);			//  <--

		DEdge* e3 = new DEdge();
		edgeMap[UnorderedPair<int>(bottomRight, bottomLeft)] = e3;
		diskEdges[bottomRight].push_back(e3);
		diskEdges[bottomLeft].push_back(e3);

	}



	// circles
	for (int i = 0; i < numSegments - 2; i++)
	{
		//// top cap
		indices.push_back((i + 2) * 2 + 1);			 // left most
		indices.push_back((i + 1) * 2 + 1);			 // middle
		indices.push_back(1);						 // anchor


		// bottom cap
		indices.push_back((i + 1) * 2);				 // middle
		indices.push_back((i + 2) * 2);				 // left most
		indices.push_back(0);						 // anchor


	}

	//std::cout << "\n NEW  OBJECT\n";
	//for (int i = 0; i < indices.size(); i++)
	//{
	//	if (i % 3 == 0)std::cout << "\n";
	//	if (i % 6 == 0)std::cout << "\n";
	//	//if (i == 54)std::cout << "*";
	//	std::cout << indices[i] << " ";
	//}
	//std::cout << "\n\n size of vector: " << indices.size() << " map size = " << edgeMap.size();

	DFace* top = new DFace();
	DFace* bottom = new DFace();


	// quads
	for (int i = 0; i < 6 * numSegments; i += 6)
	{
		DFace* face = new DFace();

		DEdge* e1 = edgeMap[UnorderedPair<int>(indices[i], indices[i + 1])];
		DEdge* e2 = edgeMap[UnorderedPair<int>(indices[i + 1], indices[i + 5])];
		DEdge* e3 = edgeMap[UnorderedPair<int>(indices[i + 5], indices[i + 2])];
		DEdge* e4 = edgeMap[UnorderedPair<int>(indices[i + 2], indices[i])];

		e1->v1 = &(*vertices)[indices[i]];
		e2->v1 = &(*vertices)[indices[i + 1]];
		e3->v1 = &(*vertices)[indices[i + 5]];
		e4->v1 = &(*vertices)[indices[i + 2]];

		e1->v2 = &(*vertices)[indices[i + 1]];
		e2->v2 = &(*vertices)[indices[i + 5]];
		e3->v2 = &(*vertices)[indices[i + 2]];
		e4->v2 = &(*vertices)[indices[i]];

		DLoop* l1 = new DLoop(e1->v2, e1, face);
		DLoop* l2 = new DLoop(e2->v2, e2, face);
		DLoop* l3 = new DLoop(e3->v2, e3, face);
		DLoop* l4 = new DLoop(e4->v2, e4, face);

		l1->next = l2;	l1->prev = l4;
		l2->next = l3;	l2->prev = l1;
		l3->next = l4;	l3->prev = l2;
		l4->next = l1;	l4->prev = l3;

		loopMap.emplace(UnorderedPair<int>(indices[i], indices[i + 1]), l1);
		loopMap.emplace(UnorderedPair<int>(indices[i + 1], indices[i + 5]), l2);
		loopMap.emplace(UnorderedPair<int>(indices[i + 5], indices[i + 2]), l3);
		loopMap.emplace(UnorderedPair<int>(indices[i + 2], indices[i]), l4);

		face->loop = l1;

		e1->loop = l1;
		e2->loop = l2;
		e3->loop = l3;
		e4->loop = l4;

		(*vertices)[indices[i]].e = e1;
		(*vertices)[indices[i + 1]].e = e2;


	}


	for (int i = 0; i < numSegments * 2; i += 2)
	{
		topRight = (i + 3) % (numSegments * 2);
		// top circle 
		DEdge* e1 = edgeMap[UnorderedPair<int>(i + 1, topRight)];
		e1->v1 = &(*vertices)[i + 1];
		e1->v2 = &(*vertices)[topRight];

		DLoop* l1 = new DLoop(e1->v2, e1, top);
		loopMap.emplace(UnorderedPair<int>(i + 1, topRight), l1);
		loopVecTop.push_back(l1);


		bottomLeft = (i + 2) % (numSegments * 2);
		// bottom circle
		e1 = edgeMap[UnorderedPair<int>(i, bottomLeft)];
		e1->v1 = &(*vertices)[i];
		e1->v2 = &(*vertices)[bottomLeft];

		l1 = new DLoop(e1->v2, e1, bottom);
		loopMap.emplace(UnorderedPair<int>(i, bottomLeft), l1);
		loopVecBottom.push_back(l1);

	}



	top->loop = loopVecTop.front();
	bottom->loop = loopVecBottom.front();

	int size = loopVecTop.size() - 1;
	loopVecTop[0]->next = loopVecTop[1];
	loopVecTop[0]->prev = loopVecTop[size];

	loopVecBottom[0]->next = loopVecBottom[1];
	loopVecBottom[0]->prev = loopVecBottom[size];
	for (int i = 1; i < size; i++)
	{
		loopVecTop[i]->next = loopVecTop[i + 1];
		loopVecTop[i]->prev = loopVecTop[i - 1];

		loopVecBottom[i]->next = loopVecBottom[i + 1];
		loopVecBottom[i]->prev = loopVecBottom[i - 1];
	}
	loopVecTop[size]->next = loopVecTop[0];
	loopVecTop[size]->prev = loopVecTop[size - 1];

	loopVecBottom[size]->next = loopVecBottom[0];
	loopVecBottom[size]->prev = loopVecBottom[size - 1];

	for (int i = 0; i < edgeIndices.size(); i += 2)
	{
		/*
			equal_range returns iterators to the original map
			where iterator points to the "first" element with that key
			and ++iterator to the next element with the same key
		*/

		auto it = loopMap.equal_range(UnorderedPair<int>(edgeIndices[i], edgeIndices[i + 1]));

		DLoop* loop1 = it.first->second;
		DLoop* loop2 = (++it.first)->second;

		loop1->radialNext = loop2;
		loop1->radialPrev = loop2;

		loop2->radialNext = loop1;
		loop2->radialPrev = loop1;

	}

	for (int i = 0; i < vertices->size(); i++)
	{
		for (int j = 0; j < diskEdges[i].size(); j++)
		{
			DEdge* e = diskEdges[i][j];
			if (&(*vertices)[i] == e->v1)
			{

				if (j == 0)
				{
					e->d1.next = diskEdges[i][j + 1];
					e->d1.prev = diskEdges[i][diskEdges[i].size() - 1];
				}
				else if (j == diskEdges[i].size() - 1)
				{
					e->d1.next = diskEdges[i][0];
					e->d1.prev = diskEdges[i][j - 1];
				}
				else
				{
					e->d1.next = diskEdges[i][j + 1];
					e->d1.prev = diskEdges[i][j - 1];
				}
			}
			else if (&(*vertices)[i] == e->v2)
			{
				if (j == 0)
				{
					e->d2.next = diskEdges[i][j + 1];
					e->d2.prev = diskEdges[i][diskEdges[i].size() - 1];
				}
				else if (j == diskEdges[i].size() - 1)
				{
					e->d2.next = diskEdges[i][0];
					e->d2.prev = diskEdges[i][j - 1];
				}
				else
				{
					e->d2.next = diskEdges[i][j + 1];
					e->d2.prev = diskEdges[i][j - 1];
				}
			}
		}

	}


	new Mesh(
		"Cylinder",
		vertices,
		indices,
		edgeIndices
	);
};

void addCone(int numSegments, float height, float radius)
{
	std::vector<DVertex>* vertices = new std::vector<DVertex>();
	std::vector<GLuint> indices;
	std::vector<GLuint> edgeIndices;

	// key should logically be of type UnorderedPair, but it does not really matter
/* used for linking with loop data */
	std::unordered_map<UnorderedPair<int>, DEdge*> edgeMap{};

	/* used for linking disks with edges

	 *	each row holds edges for the vertex with the same index

	*/
	std::vector<std::vector<DEdge*>>diskEdges(numSegments + 1);

	/* used for linking loop.radial atributes*/
	std::unordered_multimap < UnorderedPair<int>, DLoop*, UnorderedPairHash<int> > loopMap{};

	/*used for loop next/prev attributes*/
	std::vector< DLoop* > loopVec;

	float angleStep = 2.0f * PI / numSegments;
	float angle, x, z;
	height /= 2;

	for (int i = 0; i < numSegments; ++i) {
		angle = angleStep * float(i);
		x = cos(angle) * radius;
		z = sin(angle) * radius;

		vertices->push_back(DVertex{
			glm::vec3(x, -height, z),
			glm::normalize(glm::vec3(x,height,z))
			});
	}

	// top vertex
	vertices->push_back(DVertex{
		glm::vec3(0.0f, height, 0.0f),
		glm::normalize(glm::vec3(0.0f, height, 0.0f))
		});

	// Tris
	int topVert = vertices->size() - 1;
	int temp;
	DFace* bottom = new DFace();
	for (int i = 0; i < numSegments; i++) {

		temp = (i + 1) % numSegments;

		indices.push_back(topVert);					// top
		indices.push_back(temp);						// left
		indices.push_back(i);                       // right

		edgeIndices.push_back(topVert);
		edgeIndices.push_back(i);

		DEdge* e1 = new DEdge();
		edgeMap[UnorderedPair<int>(topVert, i)] = e1; // vertical edge
		e1->v1 = &(*vertices)[topVert];
		e1->v2 = &(*vertices)[i];
		diskEdges[topVert].push_back(e1);
		diskEdges[i].push_back(e1);

		edgeIndices.push_back(temp);
		edgeIndices.push_back(i);

		DEdge* e2 = new DEdge();
		edgeMap[UnorderedPair<int>(temp, i)] = e2; // base edge
		e2->v1 = &(*vertices)[temp];
		e2->v2 = &(*vertices)[i];
		diskEdges[temp].push_back(e2);
		diskEdges[i].push_back(e2);

		(*vertices)[i].e = e1;


		DLoop* l = new DLoop(e2->v2, e2, bottom);
		loopVec.push_back(l);
		loopMap.emplace(UnorderedPair<int>(temp, i), l);

	}
	bottom->loop = loopVec.front();
	// bottom circle
	for (int i = 0; i < numSegments - 2; ++i) {
		indices.push_back(i + 1);          // middle
		indices.push_back(i + 2);          // left most
		indices.push_back(0);              // anchor
	}

	(*vertices).back().e = edgeMap[UnorderedPair<int>(vertices->size() - 1, 1)];


	for (int i = 0; i < indices.size(); i += 3)
	{
		if (i < numSegments * 3)
		{
			DFace* face = new DFace();
			DEdge* e1 = edgeMap[UnorderedPair<int>(indices[i], indices[i + 1])];
			DEdge* e2 = edgeMap[UnorderedPair<int>(indices[i + 1], indices[i + 2])];
			DEdge* e3 = edgeMap[UnorderedPair<int>(indices[i + 2], indices[i])];


			DLoop* l1 = new DLoop(e1->v2, e1, face);
			DLoop* l2 = new DLoop(e2->v2, e2, face);
			DLoop* l3 = new DLoop(e3->v2, e3, face);

			l1->next = l2; l1->prev = l3;
			l2->next = l3; l2->prev = l1;
			l3->next = l1; l3->prev = l2;

			loopMap.emplace(UnorderedPair<int>(indices[i], indices[i + 1]), l1);
			loopMap.emplace(UnorderedPair<int>(indices[i + 1], indices[i + 2]), l2);
			loopMap.emplace(UnorderedPair<int>(indices[i + 2], indices[i]), l3);


			e1->loop = l1;
			e2->loop = l2;
			e3->loop = l3;


			face->loop = l1;

		}
		else
		{
			// bottom circle
			//DEdge* e1 = edgeMap[UnorderedPair<int>(indices[i], indices[i + 1])];
			//DLoop* l1 = new DLoop(e1->v2, e1, bottom);
			//(*vertices)[indices[i]].edge = e1;


		}
	}


	int size = loopVec.size() - 1;
	loopVec[0]->next = loopVec[1];
	loopVec[0]->prev = loopVec[size];


	for (int i = 1; i < size; i++)
	{
		loopVec[i]->next = loopVec[i + 1];
		loopVec[i]->prev = loopVec[i - 1];

	}
	loopVec[size]->next = loopVec[0];
	loopVec[size]->prev = loopVec[size - 1];


	for (int i = 0; i < edgeIndices.size(); i += 2)
	{
		/*
			equal_range returns iterators to the original map
			where iterator points to the "first" element with that key
			and ++iterator to the next element with the same key
		*/

		auto it = loopMap.equal_range(UnorderedPair<int>(edgeIndices[i], edgeIndices[i + 1]));

		DLoop* loop1 = it.first->second;
		DLoop* loop2 = (++it.first)->second;

		loop1->radialNext = loop2;
		loop1->radialPrev = loop2;

		loop2->radialNext = loop1;
		loop2->radialPrev = loop1;

	}

	for (int i = 0; i < vertices->size(); i++)
	{
		for (int j = 0; j < diskEdges[i].size(); j++)
		{
			DEdge* e = diskEdges[i][j];
			if (&(*vertices)[i] == e->v1)
			{

				if (j == 0)
				{
					e->d1.next = diskEdges[i][j + 1];
					e->d1.prev = diskEdges[i][diskEdges[i].size() - 1];
				}
				else if (j == diskEdges[i].size() - 1)
				{
					e->d1.next = diskEdges[i][0];
					e->d1.prev = diskEdges[i][j - 1];
				}
				else
				{
					e->d1.next = diskEdges[i][j + 1];
					e->d1.prev = diskEdges[i][j - 1];
				}
			}
			else if (&(*vertices)[i] == e->v2)
			{
				if (j == 0)
				{
					e->d2.next = diskEdges[i][j + 1];
					e->d2.prev = diskEdges[i][diskEdges[i].size() - 1];
				}
				else if (j == diskEdges[i].size() - 1)
				{
					e->d2.next = diskEdges[i][0];
					e->d2.prev = diskEdges[i][j - 1];
				}
				else
				{
					e->d2.next = diskEdges[i][j + 1];
					e->d2.prev = diskEdges[i][j - 1];
				}
			}
		}



	}


	/*std::cout << "\n NEW  OBJECT\n";
	for (int i = 0; i < indices.size(); i++)
	{
		if (i % 3 == 0)std::cout << "\n";
		if (i % 6 == 0)std::cout << "\n";
		std::cout << indices[i] << " ";
	}*/

	new Mesh(
		"Cone",
		vertices,
		indices,
		edgeIndices
	);
};

void addDoughnut(int majorSegments, int minorSegments, float majorRadius, float minorRadius)
{
	std::vector<DVertex>* vertices = new std::vector<DVertex>();
	std::vector<GLuint> indices;
	std::vector<GLuint> edgeIndices;

	// key should logically be of type UnorderedPair, but it does not really matter
/* used for linking with loop data */
	std::unordered_map<UnorderedPair<int>, DEdge*> edgeMap{};

	/* used for linking disks with edges

	 *	each row holds edges for the vertex with the same index

	*/
	std::vector<std::vector<DEdge*>>diskEdges(majorSegments * minorSegments);

	/* used for linking loop.radial atributes*/
	std::unordered_multimap < UnorderedPair<int>, DLoop*, UnorderedPairHash<int> > loopMap{};

	float x, y, z;
	int topLeft, topRight, bottomLeft, bottomRight;

	float majorStep = 2 * PI / majorSegments;
	float minorStep = 2 * PI / minorSegments;

	float dirX, dirZ;



	for (int i = 0; i < majorSegments; i++)
	{
		dirX = cosf(majorStep * i);
		dirZ = sinf(majorStep * i);

		for (int j = 0; j < minorSegments; j++)
		{
			y = minorRadius * sinf(minorStep * j);

			x = (majorRadius + minorRadius * cosf(minorStep * j)) * dirX; // glup sam za medalje ... scalar * dirVector ...
			z = (majorRadius + minorRadius * cosf(minorStep * j)) * dirZ; // x is found throught front view (XY/ZY plane) .. z is found through top view (XZ plane)



			vertices->push_back(DVertex{ glm::vec3(x,y,z),glm::normalize(glm::vec3(x,y,z)) });

			bottomRight = i * minorSegments + j;
			topRight = (bottomRight + 1) % (minorSegments * majorSegments);
			bottomLeft = ((i + 1) % majorSegments) * minorSegments + j;
			topLeft = (bottomLeft + 1) % (minorSegments * majorSegments);


			if (j == minorSegments - 1)
			{
				topRight = i * minorSegments;
				topLeft = (i + 1) % majorSegments * minorSegments;
			}

			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);

			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);

			edgeIndices.push_back(bottomRight);
			edgeIndices.push_back(topRight);

			DEdge* e1 = new DEdge();
			edgeMap[UnorderedPair<int>(bottomRight, topRight)] = e1;
			//e1->v1 = &(*vertices)[bottomRight];
			//e1->v2 = &(*vertices)[topRight];

			diskEdges[bottomRight].push_back(e1);
			diskEdges[topRight].push_back(e1);


			edgeIndices.push_back(topRight);
			edgeIndices.push_back(topLeft);


			DEdge* e2 = new DEdge();
			edgeMap[UnorderedPair<int>(topLeft, topRight)] = e2;
			//e2->v1 = &(*vertices)[topLeft];
			//e2->v2 = &(*vertices)[topRight];

			diskEdges[topLeft].push_back(e2);
			diskEdges[topRight].push_back(e2);


		}

	}
	/*std::cout << "\n NEW  OBJECT\n";
	for (int i = 0; i < indices.size(); i++)
	{
		if (i % 3 == 0)std::cout << "\n";
		if (i % 6 == 0)std::cout << "\n";
		std::cout << indices[i] << " ";
	}
	std::cout << "number of indices " << edgeMap.size();*/
	for (int i = 0; i < indices.size(); i += 6)
	{
		DFace* face = new DFace();

		DEdge* e1 = edgeMap[UnorderedPair<int>(indices[i], indices[i + 1])];
		DEdge* e2 = edgeMap[UnorderedPair<int>(indices[i + 1], indices[i + 5])];
		DEdge* e3 = edgeMap[UnorderedPair<int>(indices[i + 5], indices[i + 2])];
		DEdge* e4 = edgeMap[UnorderedPair<int>(indices[i + 2], indices[i])];

		e1->v1 = &(*vertices)[indices[i]]; e1->v2 = &(*vertices)[indices[i + 1]];
		e2->v1 = &(*vertices)[indices[i + 1]]; e2->v2 = &(*vertices)[indices[i + 5]];
		e3->v1 = &(*vertices)[indices[i + 5]]; e3->v2 = &(*vertices)[indices[i + 2]];
		e4->v1 = &(*vertices)[indices[i + 2]]; e4->v2 = &(*vertices)[indices[i]];

		(*vertices)[indices[i]].e = e1;
		(*vertices)[indices[i + 1]].e = e2;
		(*vertices)[indices[i + 5]].e = e3;
		(*vertices)[indices[i + 2]].e = e4;


		DLoop* l1 = new DLoop(&(*vertices)[indices[i + 1]], e1, face);
		DLoop* l2 = new DLoop(&(*vertices)[indices[i + 5]], e2, face);
		DLoop* l3 = new DLoop(&(*vertices)[indices[i + 2]], e3, face);
		DLoop* l4 = new DLoop(&(*vertices)[indices[i]], e4, face);

		l1->next = l2;	l1->prev = l4;
		l2->next = l3;	l2->prev = l1;
		l3->next = l4;	l3->prev = l2;
		l4->next = l1;	l4->prev = l3;

		loopMap.emplace(UnorderedPair<int>(indices[i], indices[i + 1]), l1);
		loopMap.emplace(UnorderedPair<int>(indices[i + 1], indices[i + 5]), l2);
		loopMap.emplace(UnorderedPair<int>(indices[i + 5], indices[i + 2]), l3);
		loopMap.emplace(UnorderedPair<int>(indices[i + 2], indices[i]), l4);


		face->loop = l1;


		e1->loop = l1;
		e2->loop = l2;
		e3->loop = l3;
		e4->loop = l4;

	}

	for (int i = 0; i < edgeIndices.size(); i += 2)
	{
		/*
			equal_range returns iterators to the original map
			where iterator points to the "first" element with that key
			and ++iterator to the next element with the same key
		*/

		auto it = loopMap.equal_range(UnorderedPair<int>(edgeIndices[i], edgeIndices[i + 1]));

		DLoop* loop1 = it.first->second;
		DLoop* loop2 = (++it.first)->second;

		loop1->radialNext = loop2;
		loop1->radialPrev = loop2;

		loop2->radialNext = loop1;
		loop2->radialPrev = loop1;

	}

	for (int i = 0; i < vertices->size(); i++)
	{
		for (int j = 0; j < diskEdges[i].size(); j++)
		{
			DEdge* e = diskEdges[i][j];
			if (&(*vertices)[i] == e->v1)
			{

				if (j == 0)
				{
					e->d1.next = diskEdges[i][j + 1];
					e->d1.prev = diskEdges[i][diskEdges[i].size() - 1];
				}
				else if (j == diskEdges[i].size() - 1)
				{
					e->d1.next = diskEdges[i][0];
					e->d1.prev = diskEdges[i][j - 1];
				}
				else
				{
					e->d1.next = diskEdges[i][j + 1];
					e->d1.prev = diskEdges[i][j - 1];
				}
			}
			else if (&(*vertices)[i] == e->v2)
			{
				if (j == 0)
				{
					e->d2.next = diskEdges[i][j + 1];
					e->d2.prev = diskEdges[i][diskEdges[i].size() - 1];
				}
				else if (j == diskEdges[i].size() - 1)
				{
					e->d2.next = diskEdges[i][0];
					e->d2.prev = diskEdges[i][j - 1];
				}
				else
				{
					e->d2.next = diskEdges[i][j + 1];
					e->d2.prev = diskEdges[i][j - 1];
				}
			}
		}



	}

	new Mesh(
		"Doughnut",
		vertices,
		indices,
		edgeIndices
	);
};