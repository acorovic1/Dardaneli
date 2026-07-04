#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"
#include"ShadingNodes/Texture/Texture.h"

#include "ObjectManager.h"

#include <vector>

class Object {
protected:

	

	glm::vec3 position = glm::vec3(0.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scaleFac = glm::vec3(1.0f);


	// used for updating VBO
	//std::vector<DVertex> vboverts;

	VAO vao;
	VBO vbo;
	EBO ebo;

	GLuint index{};
	std::string name;




public:

	Object(std::string name);		// call the object bvh in the constructor of the child class

	std::string getName();
	GLuint getIndex()const;

	virtual void draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES, bool outline = false) = 0;

	void translate(glm::vec3 translateVector);
	void translate(float x, float y, float z);

	virtual void rotate(float degrees, const glm::vec3 axisVector);
	virtual void rotate(glm::quat quat);

	void scale(glm::vec3 scaleVector);
	void scale(float x, float y, float z);


	void setPosition(glm::vec3 pos);
	void setPosition(float x, float y, float z);

	void setRotation(glm::vec3 rot);
	void setRotation(float x, float y, float z);
	void setRotationRad(float radiansX, float radiansY, float radiansZ);

	void setScale(glm::vec3 scale);
	void setScale(float x, float y, float z);

	glm::vec3& getPosition() { return position; }
	const glm::vec3& getPosition() const { return position; }

	glm::quat& getRotation() { return rotation; }
	const glm::vec3 getRotationVec() const { return glm::degrees(glm::eulerAngles(rotation)); }

	glm::vec3& getScale() { return scaleFac; }
	const glm::vec3& getScale() const { return scaleFac; }


		
	

	
	glm::mat4 getModelMatrix();

	void bindVAO();

	







};
