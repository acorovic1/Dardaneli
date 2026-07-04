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

void Object::setPosition(glm::vec3 pos) { position = pos; }
void Object::setPosition(float x, float y, float z) {
	position.x = x;
	position.y = y;
	position.z = z;
}
void Object::setRotation(glm::vec3 rot) { rotation = glm::quat(glm::radians(rot)); }

void Object::setRotation(float x, float y, float z) { rotation = glm::quat(glm::radians(glm::vec3(x, y, z))); }

void Object::setRotationRad(float radiansX, float radiansY, float radiansZ)
{
	rotation = glm::quat(glm::vec3(radiansX, radiansY, radiansZ));
	std::cout << "\n" << glm::to_string(rotation);
}

void Object::setScale(glm::vec3 scale) { scaleFac = scale; }

void Object::setScale(float x, float y, float z) {
	scaleFac.x = x;
	scaleFac.y = y;
	scaleFac.z = z;
}


void Object::translate(glm::vec3 translateVector) { position += translateVector; }
void Object::translate(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
}

void Object::rotate(float degrees, const glm::vec3 axisVector) {
	rotation = glm::rotate(rotation, glm::radians(degrees), axisVector);
}

void Object::rotate(glm::quat quat)
{
	rotation = quat * rotation;
}


void Object::scale(glm::vec3 scaleVector) { scaleFac *= scaleVector; }
void Object::scale(float x, float y, float z)
{
	scaleFac.x *= x;
	scaleFac.y *= y;
	scaleFac.z *= z;
}


glm::mat4 Object::getModelMatrix()
{
	glm::mat4 model(1.0f);
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


	model[0][0] = r00 * scaleFac.x;  model[0][1] = r10 * scaleFac.x;  model[0][2] = r20 * scaleFac.x;  model[0][3] = 0.0f;
	model[1][0] = r01 * scaleFac.y;  model[1][1] = r11 * scaleFac.y;  model[1][2] = r21 * scaleFac.y;  model[1][3] = 0.0f;
	model[2][0] = r02 * scaleFac.z;  model[2][1] = r12 * scaleFac.z;  model[2][2] = r22 * scaleFac.z;  model[2][3] = 0.0f;

	model[3][0] = position.x;
	model[3][1] = position.y;
	model[3][2] = position.z;
	model[3][3] = 1.0f;

	return model;

}

void Object::bindVAO() { vao.bind(); }


