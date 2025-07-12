#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"
#include"Texture.h"

#include "ObjectManager.h"

#include <vector>

class Object {
protected:

	std::vector<DVertex*> vertices;

	// used for updating VBO
	std::vector<DVertex> gpuVertices;

	VAO VAO;
	VBO VBO;
	EBO ebo;

	GLuint index{};
	std::string name;
	glm::mat4 model;


	

public:

	Object(std::string name);		// call the object bvh in the constructor of the child class

	std::string getName();
	GLuint getIndex()const;
	glm::mat4 getModelReference();
	glm::vec3 getPosition();


	void bindVAO();

	// may be unstable !!
	void UpdateVertexBuffer(int i);

	int getNumberOfVertices();
	std::vector<DVertex*>& getVertices();
	const std::vector<DVertex*>& getVertices()const;
	std::vector<DVertex> getVerticesCopy();
	std::vector<glm::vec3> getModelXVertices();
	glm::vec3 getModelXVertex(GLuint vertexIndex);
	glm::vec3 getModelXVertex(DVertex* vertex);

	int getVertexIndex(DVertex* v);

	virtual void Draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) = 0;

	virtual void Translate(glm::vec3& translateVector) = 0;
	virtual void Translate(float x, float y, float z) = 0;

	virtual void Rotate(float degrees, const glm::vec3& axisVector) = 0;

	virtual void Scale(glm::vec3& scaleVector) = 0;
	virtual void Scale(float x, float y, float z) = 0;





};
