#include "Loader.h"


#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>



bool Loader::obj(const char* file, std::vector<DVertex*>& vertices, std::vector<GLuint>& indices, std::vector<GLuint>& edgeIndices)
{
	static int counter = 0;
	std::ifstream in(file);
	if (!in.is_open()) {
		std::cerr << "Failed to open OBJ file: " << file << "\n";
		return false;
	}



	std::vector<glm::vec3> temp_normals;
	std::vector<glm::vec2> temp_uvs;


	std::string line;
	while (std::getline(in, line)) {
		std::istringstream ss(line);
		std::string prefix;
		ss >> prefix;

		if (prefix == "v") {
			glm::vec3 pos;
			ss >> pos.x >> pos.y >> pos.z;
			vertices.push_back(new DVertex(pos));
			//if (vertices.size() % 100000 == 0)
			//	std::cout << "\nLoaded vertices: " << vertices.size();
		}
		else if (prefix == "vt") {
			glm::vec2 uv;
			ss >> uv.x >> uv.y;
			//temp_uvs.push_back(uv); 
		}
		else if (prefix == "vn") {
			glm::vec3 norm;
			ss >> norm.x >> norm.y >> norm.z;
			//  temp_normals.push_back(norm); // DVertex.normal+=uv .. normalize later
		}
		else if (prefix == "f") {
			std::vector<unsigned int> cornerPos, cornerUV, cornerNorm;
			std::vector<unsigned int> faceEdge;

			std::string vertexStr;
			while (ss >> vertexStr) {
				std::replace(vertexStr.begin(), vertexStr.end(), '/', ' ');
				std::istringstream vs(vertexStr);
				 int p = 0, t = 0, n = 0;
				vs >> p;
				if (vs.peek() != EOF) vs >> t;
				if (vs.peek() != EOF) vs >> n;
				//std::cout << "p=" << p <<"\n";
				p >= 0 ? cornerPos.push_back(p) : cornerPos.push_back(vertices.size()+p);
				//cornerUV.push_back(t);
				//cornerNorm.push_back(n);

			}
			counter++;

			//if (counter % 100000 == 0)
			//	std::cout << "\nLoaded faces: " << counter;

			for (size_t i = 1; i + 1 < cornerPos.size();i++) {
				unsigned int v0 = cornerPos[0] - 1;
				unsigned int v1 = cornerPos[i] - 1;
				unsigned int v2 = cornerPos[i + 1] - 1;

				indices.push_back(v0);
				indices.push_back(v1);
				indices.push_back(v2);

				if (cornerPos.size() == 3)
				{
					edgeIndices.push_back(v0); edgeIndices.push_back(v1);
					edgeIndices.push_back(v1); edgeIndices.push_back(v2);
					edgeIndices.push_back(v2); edgeIndices.push_back(v0);
					break;
				}
				else if (cornerPos.size() == 4)
				{
					unsigned int v3 = cornerPos[i + 2] - 1;

					indices.push_back(v2);
					indices.push_back(v3);
					indices.push_back(v0);

					edgeIndices.push_back(v0); edgeIndices.push_back(v1);
					edgeIndices.push_back(v1); edgeIndices.push_back(v2);
					edgeIndices.push_back(v2); edgeIndices.push_back(v3);
					edgeIndices.push_back(v3); edgeIndices.push_back(v0);
					break;
				}
				else { // ngons

					if (i == 1) // first one
					{
						edgeIndices.push_back(v0);
						edgeIndices.push_back(v1);
					}

					//edgeIndices.push_back(v1); edgeIndices.push_back(v2);

					if (i + 1 == cornerPos.size() - 1) // last one
					{
						edgeIndices.push_back(v2);
						edgeIndices.push_back(v0);
					}
				}
			}
		}
	}

	// Copy temp positions, normals, uvs to output
	//vertices = temp_positions;
	//normals = temp_normals;
	//uvs = temp_uvs;



	return true;

}
