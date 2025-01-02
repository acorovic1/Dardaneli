#include "DrawableAABB.h"






DrawableAABB::DrawableAABB(glm::vec3 min, glm::vec3 max){

	DrawableAABB::vertices = std::vector<Vertex>{    //VRATI REFERENCE U MESH.H
			Vertex{ glm::vec3(min.x, min.y, min.z),glm::vec3()},
			Vertex{ glm::vec3(max.x, min.y, min.z),glm::vec3()},
			Vertex{ glm::vec3(max.x, max.y, min.z),glm::vec3()},
			Vertex{ glm::vec3(min.x, max.y, min.z),glm::vec3()},
			Vertex{ glm::vec3(min.x, min.y, max.z),glm::vec3()},
			Vertex{ glm::vec3(max.x, min.y, max.z),glm::vec3()},
			Vertex{ glm::vec3(max.x, max.y, max.z),glm::vec3()},
			Vertex{ glm::vec3(min.x, max.y, max.z),glm::vec3()}
	};
	DrawableAABB::indices = std::vector<GLuint>{
		0, 1, 1, 2, 2, 3, 3, 0,  // Bottom face
			4, 5, 5, 6, 6, 7, 7, 4,  // Top face
			0, 4, 1, 5, 2, 6, 3, 7   // Vertical lines
	};
	

	VAO.Bind();
	VBO VBO(vertices);
	EBO EBO(indices);

	VAO.LinkAttribute(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttribute(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));


	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void DrawableAABB::Draw(Camera& camera,Shader&shader){

	shader.Activate();
	VAO.Bind();
	camera.CameraUniform(shader, "cameraMatrix");
	
	
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
}