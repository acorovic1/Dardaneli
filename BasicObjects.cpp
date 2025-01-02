#include "BasicObjects.h"




void addPlane()
{
	// Plane vertex data (positions, normals, colors, and UVs)
	std::vector<Vertex> cubeVertices{
		Vertex{glm::vec3(-0.5f, 0.0f, 0.5f),glm::vec3(-0.5f, 0.0f, 0.5f)},
		Vertex{glm::vec3(0.5f, 0.0f, 0.5f), glm::vec3(0.5f, 0.0f, 0.5f)},
		Vertex{glm::vec3(0.5f, 0.0f, -0.5f), glm::vec3(0.5f, 0.0f, -0.5f)},
		Vertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(-0.5f, 0.0f, -0.5f)}
	};
	std::vector<GLuint> cubeIndices{
		0, 1, 2,
		2, 3, 0
	};

		new Mesh(
			"Plane",
			cubeVertices,
			cubeIndices	
	
		);
	

}

void addCube()
{
	std::vector<Vertex> cubeVertices{ 
		Vertex{ glm::vec3(-0.5f, -0.5f, 0.5f) ,glm::vec3(-0.5f, -0.5f, 0.5f) }, // V0
		Vertex{ glm::vec3(0.5f,  -0.5f, 0.5f) ,glm::vec3(0.5f,  -0.5f, 0.5f) }, // V1
		Vertex{ glm::vec3(0.5f,  0.5f,  0.5f) ,glm::vec3(0.5f,  0.5f,  0.5f) }, // V2
		Vertex{ glm::vec3(-0.5f, 0.5f,  0.5f) ,glm::vec3(-0.5f, 0.5f,  0.5f) }, // V3

		// Back face
		Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f) , glm::vec3(-0.5f, -0.5f, -0.5f) }, // V4
		Vertex{ glm::vec3(0.5f,  -0.5f, -0.5f) , glm::vec3(0.5f,  -0.5f, -0.5f) }, // V5
		Vertex{ glm::vec3(0.5f,  0.5f,  -0.5f) , glm::vec3(0.5f,  0.5f,  -0.5f) }, // V6
		Vertex{ glm::vec3(-0.5f, 0.5f,  -0.5f) , glm::vec3(-0.5f, 0.5f,  -0.5f) }, // V7
	};
	std::vector<GLuint> cubeIndices{
		// Front face
		0, 1, 2,
		2, 3, 0,
		// Back face
		4, 5, 6,
		6, 7, 4,
		// Left face
		0, 3, 7,
		7, 4, 0,
		// Right face
		1, 2, 6,
		6, 5, 1,
		// Top face
		3, 2, 6,
		6, 7, 3,
		// Bottom face
		0, 1, 5,
		5, 4, 0
	};

	std::vector<GLuint>edgeIndices{
		0,1,
		1,2,
		2,3,
		3,0,

		4,5,
		5,6,
		6,7,
		7,4,

		0,4,
		3,7,
		1,5,
		2,6,
	};

	 new Mesh (
		"Cube",
		cubeVertices,
		cubeIndices,
		edgeIndices
		);

	
}

void addCircle(int numSegments,float radius)
{
	std::vector<Vertex> circleVertices;
	std::vector<GLuint> circleIndices;

	float angleStep = 2.0f * PI / float(numSegments);
	float angle, x, z;

	for (int i = 0; i < numSegments; ++i) {
		 angle = angleStep * i;
		 x = cos(angle) * radius; 
		 z = sin(angle) * radius;

		// Add vertex for this point
		circleVertices.push_back(Vertex{
			glm::vec3(x, 0.0f, z),   // Position
			glm::normalize(glm::vec3(x, 0.0f, z))  // Normal pointing up
			
			});
		std::cout << "x = " << x << "   z = "<<z<<"\n";

	}
		std::cout << "indices\n";

	for (int i = 0; i < numSegments - 2; ++i) {
		circleIndices.push_back(0);           // Anchor point (first vertex)
		circleIndices.push_back(i + 1);       // Current vertex
		circleIndices.push_back(i + 2);       // Next vertex
		std::cout << 0 << " " << i + 1 << " " << i + 2 << "\n";
	}

	new Mesh(
		"Circle",
		circleVertices,
		circleIndices
	);
	 
};

void addSphere(int longitude , int latitude , float radius )
{
	std::vector<Vertex> sphereVertices;
	std::vector<GLuint> sphereIndices;

	float divLongitude = PI/longitude;
	float divLatitude = PI/latitude;
	float latAngle;

	for (int i = 0; i <= longitude; ++i) {
		 latAngle = 3.14159265358979323846f / 2 - i * divLongitude; // starting from pi/2 to -pi/2
		float xy = radius * cosf(latAngle);  // r * cos(u)
		float z = radius * sinf(latAngle);   // r * sin(u)

		// Add vertices per sector
		for (int j = 0; j <= latitude; ++j) {
			float longAngle = j * 2.0f *  divLongitude; // 0 to 2pi

			// Vertex position
			float x = xy * cosf(longAngle); // x = r * cos(u) * cos(v)
			float y = xy * sinf(longAngle); // y = r * cos(u) * sin(v)

			// Add vertex data
			sphereVertices.push_back(Vertex{
				glm::vec3(x, y, z),   // Position
				glm::normalize(glm::vec3(x, y, z))  // Normal

				});
		}
	}

	// Indices for sphere (triangles)
	for (int i = 0; i < longitude; ++i) {
		int k1 = i * (latitude + 1); // Beginning of current stack
		int k2 = k1 + latitude + 1;  // Beginning of next stack

		for (int j = 0; j < latitude; ++j, ++k1, ++k2) {
			// Two triangles per quad on sphere
			if (i != 0) {
				sphereIndices.push_back(k1);
				sphereIndices.push_back(k2);
				sphereIndices.push_back(k1 + 1);
			}

			if (i != (longitude - 1)) {
				sphereIndices.push_back(k1 + 1);
				sphereIndices.push_back(k2);
				sphereIndices.push_back(k2 + 1);
			}
		}
	}

	new Mesh(
		"Sphere",
		sphereVertices,
		sphereIndices
	);

	
}

void addCylinder(int numSegments, float height, float radius)
{
	std::vector<Vertex> cylinderVertices;
	std::vector<GLuint> cylinderIndices;

	float angleStep = 2.0f * PI / numSegments;
	float angle, x, z;

	height /= 2;

	// Bottom circle vertices
	for (int i = 0; i < numSegments; ++i) {
		angle = angleStep * float(i);
		 x = cos(angle) * radius;
		 z = sin(angle) * radius;

		cylinderVertices.push_back(Vertex{
			glm::vec3(x, -height , z),    // Position
			glm::normalize(glm::vec3(x, -height, z))   // Normal for bottom

			});

		// Top circle vertices
		cylinderVertices.push_back(Vertex{
			glm::vec3(x, height , z),   // Top circle position
			glm::normalize(glm::vec3(x,height,z))     // Normal pointing up

			});
	}

	// Create the sides of the cylinder
	for (int i = 0; i < numSegments; ++i) {
		int next = (i + 1) % numSegments;

		// Side triangles
		cylinderIndices.push_back(i * 2);          // Bottom vertex i
		cylinderIndices.push_back(i * 2 + 1);      // Top vertex i
		cylinderIndices.push_back(next * 2 + 1);   // Top vertex next

		cylinderIndices.push_back(i * 2);          // Bottom vertex i
		cylinderIndices.push_back(next * 2 + 1);   // Top vertex next
		cylinderIndices.push_back(next * 2);       // Bottom vertex next
	}

	// Create the top and bottom caps (without center vertex)
	for (int i = 0; i < numSegments - 2; ++i) {
		// Bottom cap
		cylinderIndices.push_back(0);              // First bottom vertex (anchor)
		cylinderIndices.push_back((i + 2) * 2);    // Next bottom vertex
		cylinderIndices.push_back((i + 1) * 2);    // Current bottom vertex

		// Top cap
		cylinderIndices.push_back(1);               // First top vertex (anchor)
		cylinderIndices.push_back((i + 1) * 2 + 1); // Current top vertex
		cylinderIndices.push_back((i + 2) * 2 + 1); // Next top vertex
	}

	new Mesh(
		"Cylinder",
		cylinderVertices,
		cylinderIndices
	);

	 
};

void addCone(int numSegments , float height , float radius )
{
	std::vector<Vertex> coneVertices;
	std::vector<GLuint> coneIndices;

	float angleStep = 2.0f * PI / numSegments;
	float angle, x, z;
	// Base circle vertices
	for (int i = 0; i < numSegments; ++i) {
		 angle = angleStep * float(i) ;
		 x = cos(angle) * radius;
		 z = sin(angle) * radius;

		coneVertices.push_back(Vertex{
			glm::vec3(x, 0.0f, z),    // Position
			glm::normalize(glm::vec3(x,0.0f,z))  // Normal

			});
	}

	// Apex vertex (top point of the cone)
	glm::vec3 apexPos = glm::vec3(0.0f, height, 0.0f);  // Apex position
	coneVertices.push_back(Vertex{
		apexPos,   // Position
		glm::normalize(apexPos)  // Normal

		});

	// Create the side faces of the cone
	for (int i = 0; i < numSegments; ++i) {
		 

		// Side triangles (apex and base vertices)
		coneIndices.push_back(coneVertices.size() - 1); // Apex
		coneIndices.push_back((i + 1) % numSegments);   // Next base vertex
		coneIndices.push_back(i);                       // Current base vertex
	}

	// Create the bottom cap (without center vertex)
	for (int i = 0; i < numSegments - 2; ++i) {
		coneIndices.push_back(0);              // First base vertex (anchor)
		coneIndices.push_back(i + 2);          // Next base vertex
		coneIndices.push_back(i + 1);          // Current base vertex
	}

	 new Mesh (
		"Cone",
		coneVertices,
		coneIndices
	);

	 
};

void addDoughnut(int numSegmentsU , int numSegmentsV,
	float majorRadius, float minorRadius)
{
	std::vector<Vertex> torusVertices;
	std::vector<GLuint> torusIndices;

	float uStep = 2 * PI / numSegmentsU;
	float vStep = 2 * PI / numSegmentsV;
	
	float u, cosU, sinU;
	float v, cosV, sinV;
	float x, y, z;

	// Generate vertices for the torus
	for (int i = 0; i <= numSegmentsU; ++i) {
		 u = uStep * float(i) ;
		 cosU = cos(u);
		 sinU = sin(u);

		for (int j = 0; j <= numSegmentsV; ++j) {
			 v = vStep * float(j) ;
			 cosV = cos(v);
			 sinV = sin(v);

			// Calculate the vertex position
			 x = (majorRadius + minorRadius * cosV) * cosU;
			 y = minorRadius * sinV;
			 z = (majorRadius + minorRadius * cosV) * sinU;

			// Normal for the torus (pointing outwards)
			glm::vec3 normal = glm::normalize(glm::vec3(cosU * cosV, sinV, sinU * cosV));

			// UV coordinates
			glm::vec2 uv = glm::vec2(float(i) / float(numSegmentsU), float(j) / float(numSegmentsV));

			// Add the vertex
			torusVertices.push_back(Vertex{
				glm::vec3(x, y, z),   // Position
				normal             // Normal

				});
		}
	}

	// Generate indices for the torus
	for (int i = 0; i < numSegmentsU; ++i) {
		for (int j = 0; j < numSegmentsV; ++j) {
			int first = i * (numSegmentsV + 1) + j;
			int second = first + numSegmentsV + 1;

			// Two triangles per quad
			torusIndices.push_back(first);
			torusIndices.push_back(second);
			torusIndices.push_back(first + 1);

			torusIndices.push_back(second);
			torusIndices.push_back(second + 1);
			torusIndices.push_back(first + 1);
		}
	}

	 new Mesh (
		"Doughnut",
		torusVertices,
		torusIndices
	);
	
};