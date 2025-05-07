#pragma once

#include"Object.h"
#include "unordered_set"

class Mesh :public Object {

	std::vector<Texture>textures;

	EBO edgeEBO;
	std::vector<GLuint>edgeIndices; // used for drawing edges
	std::vector<int> vertexIndices = std::vector<int>(0); // selected vertices

public:

	Mesh(std::string&& name, std::vector <Vertex>* vertices, std::vector <GLuint>& indices, const std::vector<GLuint>& edgeIndices = std::vector<GLuint>(), const  std::vector <Texture>& textures = std::vector<Texture>());

	~Mesh();

	std::vector<Face*> getFaces(); // logic can/needs to be improved
	std::vector<Edge*> getEdges();

	GLuint extrudeVertex(GLuint vertex);

	void Draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) override;

	void Translate(glm::vec3& translateVector)override;
	void Translate(float x, float y, float z)override;

	void Rotate(float degrees, const glm::vec3& axisVector)override;

	void Scale(glm::vec3& scaleVector)override;
	void Scale(float x, float y, float z)override;

	std::vector<int>& getSelectedVertices(); //


};
