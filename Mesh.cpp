#include "Mesh.h"

Mesh::Mesh(std::vector <Vertex>& vertices,
	std::vector <GLuint>& indices, std::vector <Texture>& textures):Object() {


	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;

	VAO.Bind();
	VBO VBO(vertices);
	EBO EBO(indices);

	VAO.LinkAttribute(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttribute(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttribute(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	VAO.LinkAttribute(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();

}


void Mesh::Draw(Shader& shader, Camera& camera) {

	shader.Activate();
	VAO.Bind();

	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;
	
	for (unsigned int i = 0; i < textures.size();i++) {
		std::string num;
		std::string type = textures[i].type;
	
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}
	
		textures[i].textureUniform(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}
	//shader.setVector3f(false, "camPos", camera.Position);
	camera.CameraUniform(shader, "cameraMatrix");

	shader.setMat4(true, "model", model);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}



void Mesh::Translate(glm::vec3 &translateVector)
{
	model = glm::translate(model, translateVector);
}
void Mesh::Translate(float x, float y, float z)
{
	model = glm::translate(model, glm::vec3(x,y,z));
}

void Mesh::Rotate(float degrees,glm::vec3 &axisVector)
{
	model = glm::rotate(model,glm::radians(degrees), axisVector);

}

void Mesh::Scale(glm::vec3 &scaleVector)
{
	model = glm::scale(model, scaleVector);
}
void Mesh::Scale(float x, float y, float z) {
	model = glm::scale(model, glm::vec3(x, y, z));
}
