#pragma once
#include"Object.h"
#include <glm/gtx/string_cast.hpp>

class Light : public Object
{
protected:


	// vertices for drawing the light representation
	std::vector<glm::vec3> drawVertices;  
	
	glm::vec3 color = glm::vec3(1.0f);
	float intensity = 3.0f;

public:
	Light(const std::string& name) : Object(name) {}
	virtual ~Light() = default;

	std::vector<glm::vec3> getDrawVertices() const { return drawVertices; }

	glm::vec3 getColor() const { return color; }
	void setColor(const glm::vec3& c) { color = c; }

	float getIntensity() const { return intensity; }
	void setIntensity(float i) { intensity = i; }



	virtual void draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES, bool outline = false) = 0;





};