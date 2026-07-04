#pragma once
#include"Light.h"


class PointLight : public Light {

	float radius = 0.2f; // Attenuation


public:
	PointLight(const std::string& name);


	float getRadius() const { return radius; }
	void setRadius(float r) { radius = r; }






	void draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES, bool outline = false) override;

	


};