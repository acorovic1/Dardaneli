#include "Lights/DirectionalLight.h"
#include <ObjectModeBVH.h>


DirectionalLight::DirectionalLight(const std::string& name) : Light(name)
{

	float arrowLength = 0.5f;

	// Shaft line
	DVertex* start = new DVertex(); start->position = glm::vec3(0, 0, 0);
	DVertex* end = new DVertex(); end->position = glm::vec3(0, -arrowLength, 0);
	vertices.push_back(start);
	vertices.push_back(end);

	DVertex* tip1 = new DVertex(); tip1->position = glm::vec3(-0.05f, -arrowLength + 0.15f, 0);
	DVertex* tip2 = new DVertex(); tip2->position = glm::vec3(0.05f, -arrowLength + 0.15f, 0);

	vertices.push_back(tip1);
	vertices.push_back(tip2);


	tip1 = new DVertex(); tip1->position = glm::vec3(0.0f, -arrowLength + 0.15f, -0.05f);
	tip2 = new DVertex(); tip2->position = glm::vec3(0.0f, -arrowLength + 0.15f, 0.05f);

	vertices.push_back(tip1);
	vertices.push_back(tip2);

	std::vector<GLuint> indices{
		0,1,  // shaft
		2,1,  // arrowhead line 1
		3,1,   // arrowhead line 2
		4,1,
		5,1
	};

	vao.bind();

	vbo.bufferData(vertices);
	ebo.bufferData(indices);
	vao.linkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(DVertex), (void*)0); //position

	vao.unbind();

	objectBVHSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
}


void DirectionalLight::rotate(float degrees, const glm::vec3& axisVector)
{
	model = glm::rotate(model, glm::radians(degrees), axisVector);
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(degrees), axisVector);
	direction = glm::vec3(rot * glm::vec4(direction, 0.0f)); // w = 0 ? treat as direction
	direction = glm::normalize(direction);

	std::cout << "Direction vec: " << direction.x << " " << direction.y << " " << direction.z;


}

void DirectionalLight::draw(Shader& shader, Camera& camera, GLenum mode,bool outline)  {

	shader.activate();
	if (outline)
		shader.setMat4(true, "model", glm::scale(model, glm::vec3(1.03f)));
	else
		shader.setMat4(true, "model", model);
	shader.setVector3f(true, "color", color);

	auto view = camera.getViewMatrix();
	auto proj = camera.getProjectionMatrix();
	shader.setMat4(true, "view", view);
	shader.setMat4(true, "projection", proj);
	shader.setFloat(true, "size", 1);

	vao.bind();
	ebo.bind();
	glLineWidth(5.0f);
	glDrawElements(GL_LINES, 10, GL_UNSIGNED_INT, 0);
	glLineWidth(1.0f);

	ebo.unbind();
	vao.unbind();
}