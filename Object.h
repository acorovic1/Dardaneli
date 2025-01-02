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

	VAO VAO;
	VBO VBO;
	EBO ebo;
	
	GLuint index{};
	glm::mat4 model;

	std::string name;
	std::vector<Vertex>vertices;
	std::vector<GLuint>indices;


public:
	
	Object(std::string name);		// call the object bvh in the constructor of the child class
	std::string getName();
	GLuint getIndex()const;
	int getNumberOfVertices();
	glm::mat4 getModelReference();
	std::vector<Vertex>& getVerticesReference();
	std::vector<Vertex> getVerticesCopy();
	std::vector<glm::vec3> getVertexXmodel();
	void bindVAO();


	void UpdateData(int i);
	void addVertex(Vertex& vertex);


	virtual void Draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) = 0;

	virtual void Translate(glm::vec3& translateVector) =0;
	virtual void Translate(float x, float y, float z) =0;

	virtual void Rotate(float degrees,const glm::vec3& axisVector) =0;

	virtual void Scale(glm::vec3& scaleVector) =0;
	virtual void Scale(float x, float y, float z) =0;

};
	