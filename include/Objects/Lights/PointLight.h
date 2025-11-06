#pragma once
#include"Light.h"


class PointLight : public Light {

	float radius = 0.2f; // Attenuation


public:
	PointLight(const std::string& name);


	float getRadius() const { return radius; }
	void setRadius(float r) { radius = r; }

	void rotate(float degrees, const glm::vec3& axisVector) override {};
	void scale(glm::vec3& scaleVector)override;
	void scale(float x, float y, float z)override;




	void draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) override;

	


};