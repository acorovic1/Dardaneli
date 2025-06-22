#include "Object.h"
#include "ObjectModeBVH.h"

Object::Object(std::string name) : VBO(), ebo()
{
	Object::index = objectSingleton->getAvailableIndex();
	objectSingleton->addObject(this);
	Object::name = name;
}

std::string Object::getName() { return name; }
GLuint Object::getIndex()const { return index; }

//!!!!!!!!!!!!!!!!!!!!does not return a reference!!!!!!!!!!!!!1
glm::mat4 Object::getModelReference() { return model; }

glm::vec3 Object::getPosition() { return glm::vec3(model[3][0], model[3][1], model[3][2]); }

void Object::bindVAO(){	VAO.Bind();}


void Object::UpdateVertexBuffer(int i)
{
	VBO.Bind();
	glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(DVertex), sizeof(DVertex), &((*vertices)[i]));
}

int Object::getNumberOfVertices() { return vertices->size(); };
std::vector<DVertex>& Object::getVertices() { return *vertices; }
const std::vector<DVertex>& Object::getVertices()const { return *vertices; }
std::vector<DVertex> Object::getVerticesCopy() { return *vertices; }
std::vector<glm::vec3> Object::getModelXVertices()
{
	std::vector<glm::vec3>position(0);
	for (const auto& x : *vertices)
		position.push_back(glm::vec3(model * glm::vec4(x.position, 1.0f)));

	return position;
}
glm::vec3 Object::getModelXVertex(GLuint vertexIndex)
{
	return glm::vec3(model * glm::vec4((*vertices)[vertexIndex].position, 1.0f));
}

glm::vec3 Object::getModelXVertex(DVertex* vertex)
{
	return getModelXVertex(this->getVertexIndex(vertex));
}

int Object::getVertexIndex(DVertex* v)
{
	auto it = std::find_if(vertices->begin(), vertices->end(), [v](const DVertex& vert) {return &vert == v;});
	if (it != vertices->end()) {
		return static_cast<int>(std::distance(vertices->begin(), it));
	}
	else {
		std::cerr << "\n\n		ERROR \n	Mesh.getVertexIndex.. DVertex does not exist";
		return -1; // Not found
	}
}
