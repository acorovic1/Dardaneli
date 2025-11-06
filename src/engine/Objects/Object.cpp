#include "Object.h"
#include "ObjectModeBVH.h"

Object::Object(std::string name) : vbo(), ebo()
{
	Object::index = objectSingleton->getAvailableIndex();
	objectSingleton->addObject(this);
	Object::name = name;
}

std::string Object::getName() { return name; }
GLuint Object::getIndex()const { return index; }





void Object::fillModel()
{
	
	float x = rotation.x, y = rotation.y, z = rotation.z, w = rotation.w;

	float r00 = 1 - 2 * (y * y + z * z);
	float r01 = 2 * (x * y - z * w);
	float r02 = 2 * (x * z + y * w);

	float r10 = 2 * (x * y + z * w);
	float r11 = 1 - 2 * (x * x + z * z);
	float r12 = 2 * (y * z - x * w);

	float r20 = 2 * (x * z - y * w);
	float r21 = 2 * (y * z + x * w);
	float r22 = 1 - 2 * (x * x + y * y);


	model[0][0] = r00 * scaling.x;  model[0][1] = r10 * scaling.x;  model[0][2] = r20 * scaling.x;  model[0][3] = 0.0f;
	model[1][0] = r01 * scaling.y;  model[1][1] = r11 * scaling.y;  model[1][2] = r21 * scaling.y;  model[1][3] = 0.0f;
	model[2][0] = r02 * scaling.z;  model[2][1] = r12 * scaling.z;  model[2][2] = r22 * scaling.z;  model[2][3] = 0.0f;

	model[3][0] = position.x;
	model[3][1] = position.y;
	model[3][2] = position.z;
	model[3][3] = 1.0f;

}

void Object::bindVAO() { vao.bind(); }


void Object::updateVertexBuffer(int i)
{
	vbo.bind();
	glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(DVertex), sizeof(DVertex), vertices[i]);
}

int Object::getNumberOfVertices() { return vertices.size(); };
std::vector<DVertex*>& Object::getVertices() { return vertices; }
const std::vector<DVertex*>& Object::getVertices()const { return vertices; }
std::vector<DVertex> Object::getVerticesCopy() {

	std::vector<DVertex> copy;
	for (const auto v : vertices) {
		copy.push_back(*v);
	}

	return copy;
}
std::vector<glm::vec3> Object::getModelXVertices()
{
	std::vector<glm::vec3>position(0);
	for (const auto x : vertices)
		position.push_back(glm::vec3(model * glm::vec4(x->position, 1.0f)));

	return position;
}
glm::vec3 Object::getModelXVertex(GLuint vertexIndex)
{
	return glm::vec3(model * glm::vec4(vertices[vertexIndex]->position, 1.0f));
}

glm::vec3 Object::getModelXVertex(DVertex* vertex)
{
	return getModelXVertex(this->getVertexIndex(vertex));
}

int Object::getVertexIndex(DVertex* v)
{
	auto it = std::find_if(vertices.begin(), vertices.end(), [v](const DVertex* vert) {return vert == v;});
	if (it != vertices.end()) {
		return static_cast<int>(std::distance(vertices.begin(), it));
	}
	else {
		std::cerr << "\n\n		ERROR \n	Mesh.getVertexIndex.. DVertex does not exist \t function returns -1\t";
		std::cout << v->position.x << " " << v->position.y << " " << v->position.z;
		return -1; // Not found
	}
}
