#pragma once



#include"Object.h"




class Mesh :public Object{
	
	EBO edgeEBO;
	std::vector<Texture>textures;
	std::vector<GLuint>edgeIndices;
	

public:



	
	Mesh(std::string&& name, std::vector <Vertex>& vertices, std::vector <GLuint>& indices, const std::vector<GLuint>&edgeIndices=std::vector<GLuint>(), const  std::vector <Texture>& textures = std::vector<Texture>());
	
	
	~Mesh();

	GLuint extrudeVertex(GLuint vertex);
	
	void Draw(Shader& shader,Camera &camera, GLenum mode = GL_TRIANGLES) override;

	void Translate(glm::vec3 &translateVector)override;
	void Translate(float x, float y, float z)override;

	void Rotate(float degrees, const glm::vec3 &axisVector)override;

	void Scale(glm::vec3 &scaleVector)override;
	void Scale(float x, float y, float z)override;
		
	

};

