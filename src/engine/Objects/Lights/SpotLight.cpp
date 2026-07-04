#include "Lights/SpotLight.h"
#include <Improved/ObjectModeBVHImproved.h>


SpotLight::SpotLight(const std::string& name) : Light(name)
{
	float radiusOuter = tan(glm::radians(outerCutoff)) * coneLength;
	float radiusInner = tan(glm::radians(innerCutoff)) * coneLength;

	int segments = 16;
	std::vector<GLuint> indices;
	for (int i = 0; i < segments; i++) {
		float angle = i / (float)segments * 2.0f * 3.1415926f;
		float nextAngle = (i + 1) / (float)segments * 2.0f * 3.1415926f;

		// Outer circle vertices
		drawVertices.push_back(glm::vec3(radiusOuter * cos(angle), 0, radiusOuter * sin(angle)));
		drawVertices.push_back(glm::vec3(radiusOuter * cos(nextAngle), 0, radiusOuter * sin(nextAngle)));

		// Inner circle vertices
		drawVertices.push_back(glm::vec3(radiusInner * cos(angle), 0, radiusInner * sin(angle)));
		drawVertices.push_back(glm::vec3(radiusInner * cos(nextAngle), 0, radiusInner * sin(nextAngle)));

		int base = 4 * i;

		indices.push_back(base);     // start of line
		indices.push_back(base + 1); // end of line

		indices.push_back(base + 2); // start of line
		indices.push_back(base + 3); // end of line
	}


	drawVertices.push_back(glm::vec3(0, coneLength, 0));

	indices.push_back(drawVertices.size() - 1); // tip index
	indices.push_back(0);

	indices.push_back(drawVertices.size() - 1); // tip index
	indices.push_back(16);

	indices.push_back(drawVertices.size() - 1); // tip index
	indices.push_back(32);

	indices.push_back(drawVertices.size() - 1); // tip index
	indices.push_back(48);

	
	drawVertices.push_back( glm::vec3(0, -coneLength * 2, 0));
	
	indices.push_back(drawVertices.size() - 2); // tip index
	indices.push_back(drawVertices.size() - 1);

	vao.bind();

	vbo.bufferData(drawVertices);
	ebo.bufferData(indices);
	vao.linkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0); //position

	vao.unbind();

	objectBVHImprovedSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
}


void SpotLight::rotate(float degrees, const glm::vec3 axisVector)
{
	rotation = glm::rotate(rotation, glm::radians(degrees), axisVector);

	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(degrees), axisVector);
	direction = glm::normalize(glm::vec3(rot * glm::vec4(direction, 0.0f))); // w = 0 ? treat as direction


	//std::cout << "Direction vec: " << direction.x << " " << direction.y << " " << direction.z;

}

void SpotLight::rotate(glm::quat quat)
{
	rotation = quat * rotation;
	direction = glm::normalize(glm::vec3(quat * glm::vec4(direction, 0.0f))); // w = 0 ? treat as direction
	//std::cout << "Direction vec: " << direction.x << " " << direction.y << " " << direction.z;
}




void SpotLight::draw(Shader& shader, Camera& camera, GLenum mode, bool outline) {
	shader.activate();
	if (outline)
		shader.setMat4(true, "model", glm::scale(getModelMatrix(), glm::vec3(1.03f)));
	else
		shader.setMat4(true, "model", getModelMatrix());
	shader.setVector4f(true, "color", glm::vec4(color, 1.0f));



	vao.bind();
	ebo.bind();
	glLineWidth(5.0f);
	glDrawElements(GL_LINES, drawVertices.size() * 2 + 8 + 2, GL_UNSIGNED_INT, 0);
	glLineWidth(1.0f);

	ebo.unbind();
	vao.unbind();
}
