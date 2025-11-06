#pragma once
#include"Light.h"

class DirectionalLight : public Light {

	glm::vec3 direction{ 0.0f, -1.0f, 0.0f };


public:
	DirectionalLight(const std::string& name);

	glm::vec3 getDirection() const { return direction; }
	void setDirection(const glm::vec3& d) { direction = d; }


	void rotate(float degrees, const glm::vec3& axisVector);
	void draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) override;

};