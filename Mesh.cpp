#include "Mesh.h"
#include "ObjectModeBVH.h"
#include "EditModeBVH.h"

Mesh::Mesh(std::string&& name, std::vector <Vertex>& vertices,
	std::vector <GLuint>& indices, const std::vector<GLuint>& edgeIndices , const  std::vector <Texture>& textures) :Object(name) {

	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::edgeIndices = edgeIndices;
	Mesh::textures = textures;

	VAO.Bind();
	VBO.bufferData(Mesh::vertices);
	ebo.bufferData(Mesh::indices);
	edgeEBO.bufferData(Mesh::edgeIndices);
	
	
	VAO.LinkAttribute(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttribute(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttribute(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	VAO.LinkAttribute(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
	
	VAO.Unbind();
	VBO.Unbind();
	ebo.Unbind();

	objectBVHSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
	

}


Mesh::~Mesh () {}

GLuint Mesh::extrudeVertex(GLuint vertex)
{
	this->addVertex(vertices.at(vertex));
		// recalculate normals potentialy

	edgeIndices.push_back(vertex);
	edgeIndices.push_back(vertices.size()-1);

	edgeEBO.bufferData(edgeIndices);

	return vertices.size() - 1;
}



void Mesh::Draw(Shader& shader, Camera& camera,GLenum mode) {

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
	shader.setVector3f(true, "camPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z );
	shader.setMat4(true, "model", model);

	if (mode == GL_TRIANGLES)
	{
		ebo.Bind();
		glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else if (mode == GL_LINES)
	{
		edgeEBO.Bind();
		glDrawElements(mode, edgeIndices.size(), GL_UNSIGNED_INT, 0);

	}
	else if (mode == GL_POINTS)
	{
		VBO.Bind();
		glDrawArrays(mode, 0,vertices.size());
	}

	VAO.Unbind();
	

}



void Mesh::Translate(glm::vec3 &translateVector)
{
	model = glm::translate(model, translateVector);
}
void Mesh::Translate(float x, float y, float z)
{
	model = glm::translate(model, glm::vec3(x,y,z));
}

void Mesh::Rotate(float degrees,const glm::vec3 &axisVector)
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










