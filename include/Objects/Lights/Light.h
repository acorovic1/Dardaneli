#pragma once
#include"Object.h"


class Light : public Object
{
protected:

	glm::vec3 color = glm::vec3(1.0f);
	float intensity = 3.0f;

public:
	Light(const std::string& name) : Object(name) {}
	virtual ~Light() = default;

	glm::vec3 getColor() const { return color; }
	void setColor(const glm::vec3& c) { color = c; }

	float getIntensity() const { return intensity; }
	void setIntensity(float i) { intensity = i; }



	virtual void draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) = 0;


	void translate(glm::vec3& translateVector) override { model = glm::translate(model, translateVector); }
	void translate(float x, float y, float z) override { model = glm::translate(model, glm::vec3(x, y, z)); }

	void rotate(float degrees, const glm::vec3& axisVector) override {}


	void scale(glm::vec3& scaleVector) override {}
	void scale(float x, float y, float z) override {}


};