#pragma once



#include <glm/glm.hpp>
#include<iostream>


struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;

	//glm::vec2 texUV;
	


	glm::vec3& getPosition() { return position; }
	

	void Translate(glm::vec3 offset) { position += offset;
		// calculateNormals();
		};

	void Translate(float* offset) {
		position.x += offset[0];
		position.y += offset[1];
		position.z += offset[2];
		std::cout << "\nV.translate "<<position.x;
		// calculateNormals();
	};
};

