#include "DrawableAABB.h"

DrawableAABB::DrawableAABB(glm::vec3 min, glm::vec3 max) {
	DrawableAABB::vertices = std::vector<DVertex>{    //VRATI REFERENCE U MESH.H
			DVertex{ glm::vec3(min.x, min.y, min.z),glm::vec3()},
			DVertex{ glm::vec3(max.x, min.y, min.z),glm::vec3()},
			DVertex{ glm::vec3(max.x, max.y, min.z),glm::vec3()},
			DVertex{ glm::vec3(min.x, max.y, min.z),glm::vec3()},
			DVertex{ glm::vec3(min.x, min.y, max.z),glm::vec3()},
			DVertex{ glm::vec3(max.x, min.y, max.z),glm::vec3()},
			DVertex{ glm::vec3(max.x, max.y, max.z),glm::vec3()},
			DVertex{ glm::vec3(min.x, max.y, max.z),glm::vec3()}
	};
	DrawableAABB::indices = std::vector<GLuint>{
		0, 1, 1, 2, 2, 3, 3, 0,  // Bottom face
			4, 5, 5, 6, 6, 7, 7, 4,  // Top face
			0, 4, 1, 5, 2, 6, 3, 7   // Vertical lines
	};

	VAO.bind();
	VBO VBO(vertices);
	EBO EBO(indices);

	VAO.linkAttribute(VBO, 0, 3, GL_FLOAT, sizeof(DVertex), (void*)0);
	VAO.linkAttribute(VBO, 1, 3, GL_FLOAT, sizeof(DVertex), (void*)(3 * sizeof(float)));

	VAO.unbind();
	VBO.unbind();
	EBO.unbind();

	VBO.Delete();
	EBO.Delete();
}

DrawableAABB::DrawableAABB(const DrawableAABB& a)
{
	vertices = a.vertices;
	indices = a.indices;

	VAO.bind();
	VBO VBO(vertices);
	EBO EBO(indices);

	VAO.linkAttribute(VBO, 0, 3, GL_FLOAT, sizeof(DVertex), (void*)0);
	VAO.linkAttribute(VBO, 1, 3, GL_FLOAT, sizeof(DVertex), (void*)(3 * sizeof(float)));

	VAO.unbind();
	VBO.unbind();
	EBO.unbind();

	VBO.Delete();
	EBO.Delete();
}

DrawableAABB DrawableAABB::operator=(const DrawableAABB& a)
{
	if (this != &a)
	{
		vertices = a.vertices;
		indices = a.indices;
		VAO.generate();

		VAO.bind();
		VBO VBO(vertices);
		EBO EBO(indices);

		VAO.linkAttribute(VBO, 0, 3, GL_FLOAT, sizeof(DVertex), (void*)0);
		VAO.linkAttribute(VBO, 1, 3, GL_FLOAT, sizeof(DVertex), (void*)(3 * sizeof(float)));

		VAO.unbind();
		VBO.unbind();
		EBO.unbind();

		VBO.Delete();
		EBO.Delete();
	}

	return *this;
}

void DrawableAABB::Draw(Camera& camera, Shader& shader) {
	shader.activate();

	VAO.bind();
	camera.cameraUniform(true,shader, "cameraMatrix");

	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
}