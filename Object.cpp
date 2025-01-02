#include "Object.h"
#include "ObjectModeBVH.h"




Object::Object(std::string name): VBO(), ebo()
{
	Object::index = objectSingleton->getAvailableIndex();
	objectSingleton->addObject(this);
	Object::name = name;

}

std::string Object::getName() { return name; }
GLuint Object::getIndex()const
{
	return index;
}
int Object::getNumberOfVertices(){	return vertices.size();};
std::vector<Vertex>& Object::getVerticesReference() { return vertices; }
std::vector<Vertex> Object::getVerticesCopy() { return vertices; }
std::vector<glm::vec3> Object::getVertexXmodel()
{
	std::vector<glm::vec3>position(0);
	for (const auto& x : vertices)
		position.push_back(glm::vec3(model * glm::vec4(x.position, 1.0f) ));

	return position;
}

void Object::bindVAO()
{
	VAO.Bind();
}



void Object::UpdateData(int i)
{
	VBO.Bind();
	glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(Vertex), sizeof(Vertex),&vertices[i]);
	
}

void Object::addVertex(Vertex&vertex)
{
	vertices.push_back(vertex);
	VBO.bufferData(vertices);
	
}


//!!!!!!!!!!!!!!!!!!!!1does not return a reference!!!!!!!!!!!!!1
glm::mat4 Object::getModelReference() { return model; }