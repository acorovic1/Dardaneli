#pragma once

#include"Light.h"

class SpotLight : public Light {

	glm::vec3 direction{ 0.0f, -1.0f, 0.0f };
	float innerCutoff = 12.5f; // degrees
	float outerCutoff = 17.5f;
	float coneLength = 1.0f;

public:
	SpotLight(const std::string& name);
	

	void draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) override;

	glm::vec3 getDirection() const { return direction; }
	void setDirection(const glm::vec3& d) { direction = d; }

	float getInnerCutoff() const { return innerCutoff; }
	float getOuterCutoff() const { return outerCutoff; }

	void setInnerCutoff(float c) { innerCutoff = c; }
	void setOuterCutoff(float c) { outerCutoff = c; }


	void rotate(float degrees, const glm::vec3& axisVector);


	void scale(glm::vec3& scaleVector);
	void scale(float x, float y, float z);

};