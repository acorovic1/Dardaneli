#include "BasicObjects.h"
#include <map>

#include "Face.h"

// Vertex vector (or the vertices themselves) need to be dinamically allocated due to the edge.tip being a pointer
// otherwise the vector is destroyed after the function ends and the vector(and its contents) no longer exist so the tip 
// becomes a dangling pointer


void addPlane()
{

	std::vector<Vertex>* vertices = new std::vector<Vertex>{
		Vertex{glm::vec3(-0.5f, 0.0f, 0.5f),glm::vec3(-0.5f, 0.0f, 0.5f)},
		Vertex{glm::vec3(0.5f, 0.0f, 0.5f), glm::vec3(0.5f, 0.0f, 0.5f)},
		Vertex{glm::vec3(0.5f, 0.0f, -0.5f), glm::vec3(0.5f, 0.0f, -0.5f)},
		Vertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(-0.5f, 0.0f, -0.5f)}
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

	Face* face = new Face();

	Edge* e1 = new Edge();
	Edge* e2 = new Edge();
	Edge* e3 = new Edge();
	Edge* e4 = new Edge();

	e1->next = e2;
	e2->next = e3;
	e3->next = e4;
	e4->next = e1;

	e1->face = face;
	e2->face = face;
	e3->face = face;
	e4->face = face;

	e1->tip = &(*vertices)[1];
	e2->tip = &(*vertices)[2];
	e3->tip = &(*vertices)[3];
	e4->tip = &(*vertices)[0];

	(*vertices)[0].edge = e1;
	(*vertices)[1].edge = e2;
	(*vertices)[2].edge = e3;
	(*vertices)[3].edge = e4;

	face->edge = e1;


	new Mesh(
		"Plane",
		vertices,
		indices,
		edgeVertices

	);
}

void addCube()
{
	std::vector<Vertex>* vertices = new std::vector<Vertex>{
	Vertex{ glm::vec3(0.5f, 0.5f, 0.5f) ,glm::vec3(0.5f, 0.5f, 0.5f) }, // V0
	Vertex{ glm::vec3(0.5f, 0.5f, -0.5f) ,glm::vec3(0.5f,  0.5f, -0.5f) }, // V1
	Vertex{ glm::vec3(0.5f, -0.5f,  0.5f) ,glm::vec3(0.5f,  -0.5f,  0.5f) }, // V2
	Vertex{ glm::vec3(0.5f, -0.5f,  -0.5f) ,glm::vec3(0.5f, -0.5f,  -0.5f) }, // V3

	// Back face
	Vertex{ glm::vec3(-0.5f, 0.5f, 0.5f) , glm::vec3(-0.5f, 0.5f, 0.5f) }, // V4
	Vertex{ glm::vec3(-0.5f, 0.5f, -0.5f) , glm::vec3(-0.5f,  0.5f, -0.5f) }, // V5
	Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f) , glm::vec3(-0.5f,  -0.5f,  0.5f) }, // V6
	Vertex{ glm::vec3(-0.5f, -0.5f,  -0.5f) , glm::vec3(-0.5f, -0.5f,  -0.5f) }, // V7
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


	std::map<std::pair<int, int>, Edge*> edgeMap{};

	for (int i = 0;i < edgeIndices.size();i += 2)
	{
		Edge* e1 = new Edge();
		Edge* e2 = new Edge();

		edgeMap[{edgeIndices[i], edgeIndices[i + 1]}] = e1;
		edgeMap[{edgeIndices[i + 1], edgeIndices[i]}] = e2;

		e1->tip = &(*vertices)[edgeIndices[i + 1]];
		e2->tip = &(*vertices)[edgeIndices[i]];

		e1->pair = e2;
		e2->pair = e1;

		if (!(*vertices)[edgeIndices[i]].edge)
			(*vertices)[edgeIndices[i]].edge = e1;


	}

	for (int i = 0;i < indices.size();i += 6)
	{
		Face* face = new Face();

		Edge* e1 = edgeMap[{indices[i], indices[i + 1]}];
		Edge* e2 = edgeMap[{indices[i + 1], indices[i + 5]}];
		Edge* e3 = edgeMap[{indices[i + 5], indices[i + 2]}];
		Edge* e4 = edgeMap[{indices[i + 2], indices[i]}];

		e1->next = e2;
		e2->next = e3;
		e3->next = e4;
		e4->next = e1;

		e1->face = face;
		e2->face = face;
		e3->face = face;
		e4->face = face;

		face->edge = e1;


	}

	new Mesh(
		"Cube",
		vertices,
		indices,
		edgeIndices
	);
}

void addCircle(int numSegments, float radius)
{
	std::vector<Vertex>* vertices = new std::vector<Vertex>();
	std::vector<GLuint> indices;
	std::vector<GLuint> edgeIndices;

	float angleStep = 2.0f * PI / numSegments;
	float angle, x, z;

	for (int i = 0; i < numSegments; ++i) {
		angle = angleStep * i;
		x = cos(angle) * radius;
		z = sin(angle) * radius;

		vertices->push_back(Vertex{
			glm::vec3(x, 0.0f, z),   // Position
			glm::normalize(glm::vec3(x, 0.0f, z))  // Normal pointing up
			});
	}
	Face* face = new Face();
	Edge* old = nullptr;
	std::map<std::pair<int, int>, Edge*> edgeMap{};
	for (int i = 0; i < numSegments - 2; ++i) {
		indices.push_back(i + 2);       // left most
		indices.push_back(i + 1);       // middle
		indices.push_back(0);           // Anchor 

		edgeIndices.push_back(i);
		edgeIndices.push_back(i + 1);

		Edge* e = new Edge();
		edgeMap[{i, i + 1}] = e;

		(*vertices)[i].edge = e;

		e->tip = &(*vertices)[i + 1];
		e->face = face;
		e->pair = nullptr;

		if (i)
		{
			old->next = e;
		}
		old = e;

	}

	// second to last vertex
	edgeIndices.push_back(numSegments - 2);
	edgeIndices.push_back(numSegments - 1);

	Edge* e = new Edge();
	edgeMap[{numSegments - 2, numSegments - 1}] = e;

	(*vertices)[numSegments - 2].edge = e;

	e->tip = &(*vertices)[numSegments - 1];
	e->face = face;
	e->pair = nullptr;
	old->next = e;

	old = e;

	// last vertex
	edgeIndices.push_back(numSegments - 1);
	edgeIndices.push_back(0);

	e = new Edge();
	edgeMap[{numSegments - 1, 0}] = e;

	(*vertices)[numSegments - 1].edge = e;

	e->tip = &(*vertices)[0];
	e->face = face;
	e->pair = nullptr;
	old->next = e;

	e->next = edgeMap[{0, 1}];
	face->edge = edgeMap[{0, 1}];



	new Mesh(
		"Circle",
		vertices,
		indices,
		edgeIndices
	);
};

void addSphere(int segments, int rings, float radius)
{
	std::vector<Vertex>* vertices = new std::vector<Vertex>();
	std::vector<GLuint> indices;
	std::vector<GLuint> edgeIndices;

	float segmentStep = 2 * PI / segments;
	float ringStep = PI / rings;

	float x, y, z;
	float radiusStep;
	int topLeftCorner, topRightCorner, bottomLeftCorner, bottomRightCorner;
	std::map<std::pair<int, int>, Edge*> edgeMap;

	// top vertex
	vertices->push_back(Vertex{ glm::vec3(0.0f,radius,0.0f),glm::normalize(glm::vec3(0.0f,radius,0.0f)) });

	for (int i = 0;i < rings - 1;i++)
	{
		y = radius * sinf(halfPI - ringStep * (i + 1)); // found from the XY/ZY plane (side view)

		radiusStep = radius * cosf(halfPI - ringStep * (i + 1));  // found from the XY/ZY plane (side view)


		for (int j = 0;j < segments;j++)
		{

			x = radiusStep * cosf(segmentStep * j); // found from the XZ plane (top down view)
			z = radiusStep * sinf(segmentStep * j);

			vertices->push_back(Vertex{ glm::vec3(x,y,z),glm::normalize(glm::vec3(x,y,z)) });

			if (i == 0) // 1st ring of faces (triangles)
			{
				edgeIndices.push_back(0);
				edgeIndices.push_back(j + 1);

				edgeMap[{0, j + 1}] = new Edge();
				edgeMap[{j + 1, 0}] = new Edge();

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


				edgeMap[{topRightCorner, topLeftCorner}] = new Edge();
				edgeMap[{topLeftCorner, topRightCorner}] = new Edge();

				edgeMap[{topRightCorner, bottomRightCorner}] = new Edge();
				edgeMap[{bottomRightCorner, topRightCorner}] = new Edge();


			}

		}

	}
	float bottomVertex = (rings - 1) * segments + 1;
	float currentVertex;
	for (int j = 0;j < segments;j++)// last ring of faces (triangles)
	{
		currentVertex = (rings - 2) * (segments)+1 + j;

		edgeIndices.push_back(bottomVertex);
		edgeIndices.push_back(currentVertex);
		edgeMap[{bottomVertex, currentVertex}] = new Edge();
		edgeMap[{currentVertex, bottomVertex}] = new Edge();

		edgeIndices.push_back(currentVertex);

		if (j == segments - 1)
		{
			indices.push_back(currentVertex - j);
			edgeIndices.push_back(currentVertex - j);

			edgeMap[{currentVertex, currentVertex - j}] = new Edge();
			edgeMap[{currentVertex - j, currentVertex}] = new Edge();
		}
		else
		{
			edgeMap[{currentVertex, currentVertex + 1}] = new Edge();
			edgeMap[{currentVertex + 1, currentVertex}] = new Edge();
			edgeIndices.push_back(currentVertex + 1);
			indices.push_back(currentVertex + 1); // next vertex
		}

		indices.push_back(bottomVertex);
		indices.push_back(currentVertex);

	}

	vertices->push_back(Vertex{ glm::vec3(0.0f,-radius,0.0f),glm::normalize(glm::vec3(0.0f,-radius,0.0f)) });

	(*vertices)[0].edge = edgeMap[{0, 2}];


	for (int i = 0;i < indices.size();)
	{
		Face* face = new Face();
		if (i < 3 * segments) // triangles
		{
			// indices[i] left vertex
			// indices[i+1] right vertex
			// indices[i+2]  top vertex
			//std::cout << indices[i] << " " << indices[i + 1] << " " << indices[i + 2] << "\n";

			Edge* e1 = edgeMap[{indices[i], indices[i + 1]}];
			Edge* e2 = edgeMap[{indices[i + 1], indices[i + 2]}];
			Edge* e3 = edgeMap[{indices[i + 2], indices[i]}];


			if (!(*vertices)[indices[i + 1]].edge)
				(*vertices)[indices[i + 1]].edge = e2;

			e1->tip = &(*vertices)[indices[i + 1]];
			e2->tip = &(*vertices)[indices[i + 2]];
			e3->tip = &(*vertices)[indices[i]];

			e1->next = e2;
			e2->next = e3;
			e3->next = e1;

			e1->pair = edgeMap[{indices[i + 1], indices[i]}];
			e2->pair = edgeMap[{indices[i + 2], indices[i + 1]}];
			e3->pair = edgeMap[{indices[i], indices[i + 2]}];

			e1->face = face;
			e2->face = face;
			e3->face = face;


			face->edge = e1;

			i += 3;
		}
		else if (i < 3 * segments + 6 * segments * (rings - 2)) // 3 * segments = number of indices for the top tris + 6*segments*(rings-2) = number of indices for the quads
		{
			// indices[i] is the top left index
			// indices[i+2] is the top right index
			// indices[i+1] is the bottom left index
			// indices[i+5] is the bottom right index

			//std::cout << indices[i] << " " << indices[i + 1] << " " << indices[i + 2]<<"\n";

			Edge* e1 = edgeMap[{indices[i + 2], indices[i]}];     // <--
			Edge* e2 = edgeMap[{indices[i], indices[i + 1]}];	  // |
			Edge* e3 = edgeMap[{indices[i + 1], indices[i + 5]}]; // -->
			Edge* e4 = edgeMap[{indices[i + 5], indices[i + 2]}]; //	|


			if (!(*vertices)[indices[i + 5]].edge)
				(*vertices)[indices[i + 5]].edge = e4;

			e1->tip = &(*vertices)[indices[i]];
			e2->tip = &(*vertices)[indices[i + 1]];
			e3->tip = &(*vertices)[indices[i + 5]];
			e4->tip = &(*vertices)[indices[i + 2]];

			e1->next = e2;
			e2->next = e3;
			e3->next = e4;
			e4->next = e1;

			e1->pair = edgeMap[{indices[i], indices[i + 2]}];
			e2->pair = edgeMap[{indices[i + 1], indices[i]}];
			e3->pair = edgeMap[{indices[i + 5], indices[i + 1]}];
			e4->pair = edgeMap[{indices[i + 2], indices[i + 5]}];

			e1->face = face;
			e2->face = face;
			e3->face = face;
			e4->face = face;

			face->edge = e1;

			i += 6;
		}
		else
		{
			// indices[i] left vertex
			// indices[i+1] bottom vertex
			// indices[i+2]  right vertex
			//std::cout << indices[i] << " " << indices[i + 1] << " " << indices[i + 2] << "\n";

			Edge* e1 = edgeMap[{indices[i], indices[i + 1]}];
			Edge* e2 = edgeMap[{indices[i + 1], indices[i + 2]}];
			Edge* e3 = edgeMap[{indices[i + 2], indices[i]}];

			e1->tip = &(*vertices)[indices[i + 1]];
			e2->tip = &(*vertices)[indices[i + 2]];
			e3->tip = &(*vertices)[indices[i]];

			e1->next = e2;
			e2->next = e3;
			e3->next = e1;

			e1->pair = edgeMap[{indices[i + 1], indices[i]}];
			e2->pair = edgeMap[{indices[i + 2], indices[i + 1]}];
			e3->pair = edgeMap[{indices[i], indices[i + 2]}];

			e1->face = face;
			e2->face = face;
			e3->face = face;

			face->edge = e1;

			i += 3;
		}
	}

	(*vertices)[vertices->size() - 1].edge = edgeMap[{vertices->size() - 1, vertices->size() - 2}];

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
	std::vector<Vertex>* vertices = new std::vector<Vertex>();
	std::vector<GLuint> indices;
	std::vector<GLuint> edgeIndices;

	std::map<std::pair<int, int>, Edge*> edgeMap;

	float angleStep = 2.0f * PI / numSegments;
	float angle, x, z;

	height /= 2;


	for (int i = 0; i < numSegments; ++i) {
		angle = angleStep * float(i);
		x = cos(angle) * radius;
		z = sin(angle) * radius;

		// Bottom circle vertices
		vertices->push_back(Vertex{
			glm::vec3(x, -height , z),
			glm::normalize(glm::vec3(x, -height, z))
			});

		// Top circle vertices
		vertices->push_back(Vertex{
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

		std::cout << topLeft << " " << topRight<< " " << bottomLeft<< " " << bottomRight<< "\n";

		indices.push_back(topLeft);
		indices.push_back(bottomLeft);
		indices.push_back(topRight);

		indices.push_back(topRight);
		indices.push_back(bottomLeft);
		indices.push_back(bottomRight);



		edgeIndices.push_back(topLeft);			// |   left edge
		edgeIndices.push_back(bottomLeft);		// V

		edgeMap[{topLeft, bottomLeft}] = new Edge();
		edgeMap[{bottomLeft, topLeft}] = new Edge();


		edgeIndices.push_back(topRight);		// <--
		edgeIndices.push_back(topLeft);			// top edge	

		edgeMap[{topRight, topLeft }] = new Edge();
		edgeMap[{topLeft, topRight}] = new Edge();


		edgeIndices.push_back(bottomRight);			//  bottom edge
		edgeIndices.push_back(bottomLeft);			//  <--

		edgeMap[{bottomRight, bottomLeft}] = new Edge();
		edgeMap[{bottomLeft, bottomRight }] = new Edge();
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

	Face* top = new Face();
	Face* bottom = new Face();

	for (auto x : edgeMap)
	{
		if (x.second)
		{
			std::cout << "\nFollowing edge HAS an edge!!!! " << x.first.first << " --> " << x.first.second;
			continue;
		}
		else std::cout << "\nFollowing edge doesnt have an EDGE!!!! " << x.first.first << " --> " << x.first.second;
		/*if (x.second->pair)
			continue;

		std::cout << "\nFollowing edge has no pair " << x.first.first << " --> " << x.first.second;*/
	}

	for (int i = 0;i < indices.size();i += 6)
	{
		


		// quads
		if (i < 6 * numSegments)
		{

			Face* face = new Face();

			Edge* e1 = edgeMap[{indices[i], indices[i + 1]}];
			Edge* e2 = edgeMap[{indices[i + 1], indices[i + 5]}];
			Edge* e3 = edgeMap[{indices[i + 5], indices[i + 2]}];
			Edge* e4 = edgeMap[{indices[i + 2], indices[i]}];

			if (e3==nullptr)
				std::cout << "\n e3 " << indices[i + 5] << " " << indices[i + 2];

			e1->tip = &(*vertices)[indices[i + 1]];
			e2->tip = &(*vertices)[indices[i + 5]];
			e3->tip = &(*vertices)[indices[i + 2]];
			e4->tip = &(*vertices)[indices[i]];

			e1->next = e2;
			e2->next = e3;
			e3->next = e4;
			e4->next = e1;

			e1->pair = edgeMap[{indices[i + 1], indices[i]}];
			e2->pair = edgeMap[{indices[i + 5], indices[i + 1]}];
			
			e3->pair = edgeMap[{indices[i + 2], indices[i + 5] }]; // jebem ti mater 
			e4->pair = edgeMap[{indices[i], indices[i + 2]}];

			if (!e3->pair)
				std::cout << "\n pair " << indices[i + 2] << " " << indices[i + 5];

			e1->face = face;
			e2->face = face;
			e3->face = face;
			e4->face = face;

			face->edge = e1;

		}
		else
		{
			// top circle 
			Edge* e1 = edgeMap[{indices[i], indices[i + 1]}];


			(*vertices)[indices[i]].edge = e1;

			e1->tip = &(*vertices)[indices[i + 1]];
			if (i == 6 * numSegments)
				e1->next = edgeMap[{indices[i + 1], indices[i + 2]}];
			else e1->next = edgeMap[{indices[i + 1], indices[i - 5]}];
			e1->pair = edgeMap[{indices[i + 1], indices[i]}];
			e1->face = top;

			// bottom circle
			e1 = edgeMap[{indices[i + 3], indices[i + 4]}];

			(*vertices)[indices[i + 3]].edge = e1;

			e1->tip = &(*vertices)[indices[i + 4]];
			if (i == indices.size() - 6)
			{
				e1->next = edgeMap[{indices[indices.size() - 2], 0}];
				//std::cout << " \n\n Edge " << indices[i + 3] << " " << indices[i + 4] << " .next" << indices.size() - 2 << " 0\n";
			}
			if (i != 6 * numSegments)
			{
				edgeMap[{indices[i - 3], indices[i - 2]}]->next = e1;
				
			}
			e1->pair = edgeMap[{indices[i + 4], indices[i + 3]}];
			e1->face = bottom;

		}

		if (edgeMap[{0, 1}]->pair)
		{
			std::cout << "\n\n its there  in iteration " << i;
		}
		else std::cout << "\n\n its MISSING  in iteration " << i;
	}

	// finishing up the top circle
	Edge* e31 = edgeMap[{3, 1}];
	Edge* e1last = edgeMap[{1, vertices->size() - 1}];


	(*vertices)[3].edge = e31;

	e31->tip = &(*vertices)[1];
	e31->next = e1last;
	e31->pair = edgeMap[{1, 3}];
	e31->face = top;


	(*vertices)[1].edge = e1last;

	e1last->tip = &(*vertices)[vertices->size() - 1];
	e1last->next = edgeMap[{vertices->size() - 1, vertices->size() - 3}];
	e1last->pair = edgeMap[{vertices->size() - 1, 1}];
	e1last->face = top;

	top->edge = e1last;

	// finishing up the bottom circle

	Edge* e02 = edgeMap[{0, 2}];
	Edge* elast0 = edgeMap[{vertices->size() - 2, 0}];

	(*vertices)[0].edge = e02;

	e02->tip = &(*vertices)[2];
	e02->next = edgeMap[{2, 4}];
	e02->pair = edgeMap[{2, 0}];
	e02->face = bottom;


	(*vertices)[vertices->size() - 2].edge = elast0;

	elast0->tip = &(*vertices)[0];
	elast0->next = e02;
	elast0->pair = edgeMap[{0, vertices->size() - 2}];
	elast0->face = bottom;

	bottom->edge = elast0;


	//std::cout << "\n NEW  OBJECT\n";
	//for (int i = 0; i < indices.size(); i++)
	//{
	//	if (i % 3 == 0)std::cout << "\n";
	//	if (i % 6 == 0)std::cout << "\n";
	//	if (i == 54)std::cout << "*";
	//	std::cout << indices[i] << " ";
	//}
	//std::cout << "\n\n size of vector: " << indices.size() << " map size = " << edgeMap.size();

	for (auto x : edgeMap)
	{
		if (!x.second)
		{
			std::cout << "\nFollowing edge has no EDGE!!!! " << x.first.first << " --> " << x.first.second;
			continue;
		}
		if (x.second->pair)
			continue;

		std::cout << "\nFollowing edge has no pair " << x.first.first << " --> " << x.first.second;
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
	std::vector<Vertex>* vertices = new std::vector<Vertex>();
	std::vector<GLuint> indices;
	std::vector<GLuint> edgeIndices;

	std::map<std::pair<int, int>, Edge*> edgeMap;

	float angleStep = 2.0f * PI / numSegments;
	float angle, x, z;
	height /= 2;

	for (int i = 0; i < numSegments; ++i) {
		angle = angleStep * float(i);
		x = cos(angle) * radius;
		z = sin(angle) * radius;

		vertices->push_back(Vertex{
			glm::vec3(x, -height, z),
			glm::normalize(glm::vec3(x,height,z))
			});
	}

	// top vertex
	vertices->push_back(Vertex{
		glm::vec3(0.0f, height, 0.0f),
		glm::normalize(glm::vec3(0.0f, height, 0.0f))
		});

	// Tris
	int topVert = vertices->size() - 1;
	for (int i = 0; i < numSegments; i++) {

		indices.push_back(topVert);					// top
		indices.push_back((i + 1) % numSegments);   // left
		indices.push_back(i);                       // right

		edgeIndices.push_back(topVert);
		edgeIndices.push_back(i);

		edgeMap[{topVert, i}] = new Edge(); // vertical edges
		edgeMap[{i, topVert}] = new Edge();

		edgeIndices.push_back((i + 1) % numSegments);
		edgeIndices.push_back(i);

		edgeMap[{(i + 1) % numSegments, i}] = new Edge(); // base edges
		edgeMap[{i, (i + 1) % numSegments}] = new Edge();
	}

	// bottom circle
	for (int i = 0; i < numSegments - 2; ++i) {
		indices.push_back(i + 1);          // middle
		indices.push_back(i + 2);          // left most
		indices.push_back(0);              // anchor
	}

	(*vertices)[vertices->size() - 1].edge = edgeMap[{vertices->size() - 1, 1}];

	Face* bottom = new Face();
	for (int i = 0;i < indices.size();i += 3)
	{
		if (i < numSegments * 3)
		{
			Face* face = new Face();
			Edge* e1 = edgeMap[{indices[i], indices[i + 1]}];
			Edge* e2 = edgeMap[{indices[i + 1], indices[i + 2]}];
			Edge* e3 = edgeMap[{indices[i + 2], indices[i]}];

			(*vertices)[indices[i + 1]].edge = e2;

			e1->tip = &(*vertices)[indices[i + 1]];
			e2->tip = &(*vertices)[indices[i + 2]];
			e3->tip = &(*vertices)[indices[i]];

			e1->next = e2;
			e2->next = e3;
			e3->next = e1;

			e1->pair = edgeMap[{indices[i + 1], indices[i]}];
			e2->pair = edgeMap[{indices[i + 2], indices[i + 1]}];
			e3->pair = edgeMap[{indices[i], indices[i + 2]}];

			e1->face = face;
			e2->face = face;
			e3->face = face;

			face->edge = e1;

		}
		else
		{
			// bottom circle
			Edge* e1 = edgeMap[{indices[i], indices[i + 1]}];

			//(*vertices)[indices[i]].edge = e1;

			e1->tip = &(*vertices)[indices[i + 1]];
			if (i == indices.size() - 3)
				e1->next = edgeMap[{indices[indices.size() - 2], 0}];

			if (i != 3 * numSegments)
				edgeMap[{indices[i - 3], indices[i - 2]}]->next = e1;
			e1->pair = edgeMap[{indices[i + 1], indices[i]}];
			e1->face = bottom;
		}
	}
	//finishing the circle
	Edge* elast0 = edgeMap[{vertices->size() - 2, 0}];
	Edge* e01 = edgeMap[{0, 1}];

	elast0->tip = &(*vertices)[0];
	elast0->next = e01;
	elast0->pair = edgeMap[{0, vertices->size() - 2}];
	elast0->face = bottom;

	e01->tip = &(*vertices)[1];
	e01->next = edgeMap[{1, 2}];
	e01->pair = edgeMap[{1, 0}];
	e01->face = bottom;

	bottom->edge = e01;

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
	std::vector<Vertex>* vertices = new std::vector<Vertex>();
	std::vector<GLuint> indices;
	std::vector<GLuint> edgeIndices;

	float x, y, z;
	int topLeft, topRight, bottomLeft, bottomRight;

	float majorStep = 2 * PI / majorSegments;
	float minorStep = 2 * PI / minorSegments;

	float dirX, dirZ;

	std::map < std::pair<int, int>, Edge* > edgeMap;

	for (int i = 0;i < majorSegments;i++)
	{
		dirX = cosf(majorStep * i);
		dirZ = sinf(majorStep * i);

		for (int j = 0;j < minorSegments;j++)
		{
			y = minorRadius * sinf(minorStep * j);

			x = (majorRadius + minorRadius * cosf(minorStep * j)) * dirX; // glup sam za medalje ... scalar * dirVector ...
			z = (majorRadius + minorRadius * cosf(minorStep * j)) * dirZ; // x is found throught front view (XY/ZY plane) .. z is found through top view (XZ plane)



			vertices->push_back(Vertex{ glm::vec3(x,y,z),glm::normalize(glm::vec3(x,y,z)) });

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

			edgeMap[{bottomRight, topRight}] = new Edge();
			edgeMap[{topRight, bottomRight }] = new Edge();

			edgeIndices.push_back(topRight);
			edgeIndices.push_back(topLeft);

			edgeMap[{topLeft, topRight}] = new Edge();
			edgeMap[{topRight, topLeft }] = new Edge();


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
	for (int i = 0;i < indices.size();i += 6)
	{
		Face* face = new Face();

		Edge* e1 = edgeMap[{indices[i], indices[i + 1]}];
		Edge* e2 = edgeMap[{indices[i + 1], indices[i + 5]}];
		Edge* e3 = edgeMap[{indices[i + 5], indices[i + 2]}];
		Edge* e4 = edgeMap[{indices[i + 2], indices[i]}];

		(*vertices)[indices[i]].edge = e1;

		e1->tip = &(*vertices)[indices[i + 1]];
		e2->tip = &(*vertices)[indices[i + 5]];
		e3->tip = &(*vertices)[indices[i + 2]];
		e4->tip = &(*vertices)[indices[i]];

		e1->next = e2;
		e2->next = e3;
		e3->next = e4;
		e4->next = e1;

		e1->pair = edgeMap[{indices[i + 1], indices[i]}];
		e2->pair = edgeMap[{indices[i + 5], indices[i + 1]}];
		e3->pair = edgeMap[{indices[i + 2], indices[i + 5]}];
		e4->pair = edgeMap[{indices[i], indices[i + 2]}];

		e1->face = face;
		e2->face = face;
		e3->face = face;
		e4->face = face;

		face->edge = e4;

	}

	new Mesh(
		"Doughnut",
		vertices,
		indices,
		edgeIndices
	);
};