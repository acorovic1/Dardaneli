#include "Object.h"
#include "ObjectModeBVH.h"

Object::Object(std::string name) : VBO(), ebo()
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
int Object::getNumberOfVertices() { return vertices->size(); };
std::vector<Vertex>& Object::getVerticesReference() { return *vertices; }
std::vector<Vertex> Object::getVerticesCopy() { return *vertices; }
std::vector<glm::vec3> Object::getVerticesXmodel()
{
	std::vector<glm::vec3>position(0);
	for (const auto& x : *vertices)
		position.push_back(glm::vec3(model * glm::vec4(x.getPositionCopy(), 1.0f)));

	return position;
}
glm::vec3 Object::getVertexXmodel(GLuint vertexIndex)
{
	return glm::vec3(model * glm::vec4((*vertices)[vertexIndex].getPositionCopy(), 1.0f));
}

glm::vec3 Object::getPosition()
{
	return glm::vec3(model[3][0], model[3][1], model[3][2]);
}

void Object::bindVAO()
{
	VAO.Bind();
}

void Object::UpdateVertexBuffer(int i)
{
	VBO.Bind();
	glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(Vertex), sizeof(Vertex), &((*vertices)[i]));
}

void Object::addVertex(Vertex& vertex)
{
	vertices->push_back(vertex);
	VBO.bufferData(*vertices);
}

//!!!!!!!!!!!!!!!!!!!!does not return a reference!!!!!!!!!!!!!1
glm::mat4 Object::getModelReference() { return model; }