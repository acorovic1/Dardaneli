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

	std::vector<DVertex*> vertices;

	glm::vec3 position = glm::vec3(0.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scaling = glm::vec3(1.0f);
	glm::mat4 model;

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

	glm::mat4& getModel() { return model; };
	const glm::mat4& getModel() const { return model; };

	glm::vec3& getPosition() { return position; }
	const glm::vec3& getPosition() const { return position; }

	glm::quat& getRotation() { return rotation; }
	const glm::vec3 getRotationVec() const { return glm::degrees(glm::eulerAngles(rotation)); }

	glm::vec3& getScale() { return scaling; }
	const glm::vec3& getScale() const { return scaling; }

	// rotiranje vraca na 0 sve elemente u guiu.. mesh.rotate i gui.selectObject pogledati

	void fillModel();


	void bindVAO();

	// may be unstable !!
	void updateVertexBuffer(int i);

	int getNumberOfVertices();
	std::vector<DVertex*>& getVertices();
	const std::vector<DVertex*>& getVertices()const;
	std::vector<DVertex> getVerticesCopy();
	std::vector<glm::vec3> getModelXVertices();
	glm::vec3 getModelXVertex(GLuint vertexIndex);
	glm::vec3 getModelXVertex(DVertex* vertex);

	int getVertexIndex(DVertex* v);

	virtual void draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES, bool outline = false) = 0;

	virtual void translate(glm::vec3& translateVector) = 0;
	virtual void translate(float x, float y, float z) = 0;

	virtual void rotate(float degrees, const glm::vec3& axisVector) = 0;

	virtual void scale(glm::vec3& scaleVector) = 0;
	virtual void scale(float x, float y, float z) = 0;





};
