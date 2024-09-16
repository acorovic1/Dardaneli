#pragma once

#define MESH_CLASS_H

#include"Object.h"




class Mesh :public Object{
	
	VAO VAO;

	glm::mat4 model;

public:
	std::vector<Vertex>vertices; 
	std::vector<GLuint>indices;
	std::vector<Texture>textures;


	
	Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

	
	void Draw(Shader& shader,Camera &camera);

	void Translate(glm::vec3 &translateVector)override;
	void Translate(float x, float y, float z)override;

	void Rotate(float degrees, glm::vec3 &axisVector)override;

	void Scale(glm::vec3 &scaleVector)override;
	void Scale(float x, float y, float z)override;
		
	
};