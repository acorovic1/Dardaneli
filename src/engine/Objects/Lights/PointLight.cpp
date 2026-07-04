#include "Lights/PointLight.h"
#include <Improved/ObjectModeBVHImproved.h>


PointLight::PointLight(const std::string& name) : Light(name)
{

	int latSegments = 16;   // parallels
	int longSegments = 8;  // meridians
	float radius = 0.1f;


	std::vector<GLuint> indices;
	int segments = 16;
	drawVertices.resize(segments * 2);
	indices.resize(segments * 4);
	//  circles
	for (int i = 0; i < segments; i++) {
		float angle = i / (float)segments * 2.0f * glm::pi<float>();

		drawVertices[i] = glm::vec3(radius * cos(angle), radius * sin(angle), 0.0f);
		drawVertices[segments + i] = glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));

		int base = 4 * i;
		indices[base] = i;
		indices[base + 1] = (i + 1) % segments;              // XY circle

		indices[base + 2] = i + segments;
		indices[base + 3] = (i + 1) % segments + segments;   // XZ circle

	}


	vao.bind();

	vbo.bufferData(drawVertices);
	ebo.bufferData(indices);
	vao.linkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0); //position

	vao.unbind();

	objectBVHImprovedSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
}

void PointLight::draw(Shader& shader, Camera& camera, GLenum mode, bool outline)
{

	shader.activate();
	if (outline)
		shader.setMat4(true, "model", glm::scale(getModelMatrix(), glm::vec3(1.03f)));
	else
		shader.setMat4(true, "model", getModelMatrix());

	shader.setVector4f(true, "color", glm::vec4(color, 1.0f));

	camera.cameraUniform(true, shader, "cameraMatrix");

	vao.bind();
	ebo.bind();
	glLineWidth(5.0f);
	glDrawElements(GL_LINES, 64, GL_UNSIGNED_INT, 0);
	glLineWidth(1.0f);

	ebo.unbind();
	vao.unbind();
}