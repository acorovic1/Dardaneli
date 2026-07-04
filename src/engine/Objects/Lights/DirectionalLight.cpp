#include "Lights/DirectionalLight.h"
#include <Improved/ObjectModeBVHImproved.h>


DirectionalLight::DirectionalLight(const std::string& name) : Light(name)
{

	float arrowLength = 0.5f;

	// Shaft line
	drawVertices.push_back(glm::vec3(0, 0, 0));
	drawVertices.push_back(glm::vec3(0, -arrowLength, 0));


	drawVertices.push_back(glm::vec3(-0.05f, -arrowLength + 0.15f, 0));
	drawVertices.push_back(glm::vec3(0.05f, -arrowLength + 0.15f, 0));

	drawVertices.push_back(glm::vec3(0.0f, -arrowLength + 0.15f, -0.05f));
	drawVertices.push_back(glm::vec3(0.0f, -arrowLength + 0.15f, 0.05f));



	std::vector<GLuint> indices{
		0,1,  // shaft
		2,1,  // arrowhead line 1
		3,1,   // arrowhead line 2
		4,1,
		5,1
	};

	vao.bind();

	vbo.bufferData(drawVertices);
	ebo.bufferData(indices);
	vao.linkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0); //position

	vao.unbind();

	objectBVHImprovedSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
}

void DirectionalLight::rotate(float degrees, const glm::vec3 axisVector)
{
	rotation = glm::rotate(rotation, glm::radians(degrees), axisVector);

	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(degrees), axisVector);
	direction = glm::normalize(glm::vec3(rot * glm::vec4(direction, 0.0f))); 


	//std::cout << "Direction vec OLDDD: " << direction.x << " " << direction.y << " " << direction.z;


}

void DirectionalLight::rotate(glm::quat quat)
{
	rotation = quat * rotation;
	direction = quat * direction;
	//std::cout << "\nJa sm pauk: " << direction.x << " " << direction.y << " " << direction.z;
}

void DirectionalLight::draw(Shader& shader, Camera& camera, GLenum mode, bool outline) {

	shader.activate();
	if (outline)
		shader.setMat4(true, "model", glm::scale(getModelMatrix(), glm::vec3(1.03f)));
	else
		shader.setMat4(true, "model", getModelMatrix());

	shader.setVector4f(true, "color", glm::vec4(color, 1.0f));


	vao.bind();
	ebo.bind();
	glLineWidth(5.0f);
	glDrawElements(GL_LINES, 10, GL_UNSIGNED_INT, 0);
	glLineWidth(1.0f);

	ebo.unbind();
	vao.unbind();
}