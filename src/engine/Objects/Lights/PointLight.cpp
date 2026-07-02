#include "Lights/PointLight.h"
#include <Improved/ObjectModeBVHImproved.h>


PointLight::PointLight(const std::string& name) : Light(name)
{

	int latSegments = 16;   // parallels
	int longSegments = 8;  // meridians
	float radius = 0.1f;


	std::vector<GLuint> indices;
	int segments = 16;
	// XY circle
	for (int i = 0; i < segments; i++) {
		float angle = i / (float)segments * 2.0f * glm::pi<float>();
		DVertex* v = new DVertex();
		v->position = glm::vec3(radius * cos(angle), radius * sin(angle), 0.0f);
		vertices.push_back(v);
	}

	// XZ circle
	for (int i = 0; i < segments; i++) {
		float angle = i / (float)segments * 2.0f * glm::pi<float>();
		DVertex* v = new DVertex();
		v->position = glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
		vertices.push_back(v);
	}

	// Indices for lines (wrap around)
	for (int i = 0; i < segments; i++) {
		indices.push_back(i);
		indices.push_back((i + 1) % segments);              // XY circle

		indices.push_back(i + segments);
		indices.push_back((i + 1) % segments + segments);   // XZ circle
	}
	vao.bind();

	vbo.bufferData(vertices);
	ebo.bufferData(indices);
	vao.linkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(DVertex), (void*)0); //position

	vao.unbind();

	objectBVHImprovedSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
}


void PointLight::scale(glm::vec3& scaleVector) { model = glm::scale(model, glm::vec3(scaleVector.x)); }
void PointLight::scale(float x, float y, float z) { model = glm::scale(model, glm::vec3(x, x, x)); }

void PointLight::draw(Shader& shader, Camera& camera, GLenum mode, bool outline)
{

	shader.activate();
	if (outline)
		shader.setMat4(true, "model", glm::scale(model, glm::vec3(1.03f)));
	else
		shader.setMat4(true, "model", model);
	
	shader.setVector4f(true, "color", glm::vec4(color,1.0f));

	//auto view = camera.getViewMatrix();
	//auto proj = camera.getProjectionMatrix();
	//shader.setMat4(true, "view", view);
	//shader.setMat4(true, "projection", proj);
	//shader.setFloat(true, "size", 1);

	camera.cameraUniform(true, shader, "cameraMatrix");

	vao.bind();
	ebo.bind();
	glLineWidth(5.0f);
	glDrawElements(GL_LINES, 64, GL_UNSIGNED_INT, 0);
	glLineWidth(1.0f);

	ebo.unbind();
	vao.unbind();
}
