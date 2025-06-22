#pragma once

#include"Object.h"
#include "unordered_set"

class Mesh :public Object {


	
	std::vector<Texture>textures;

	std::vector<GLuint>indices; // used for drawing faces
	EBO edgeEBO;
	std::vector<GLuint>edgeIndices; // used for drawing edges

	std::vector<int> selectedVertexIndices = std::vector<int>(0);
	std::vector<DFace*>selectedFaces;
	std::vector<DEdge*>selectedEdges;

	// !!! needs an EBO update !!!
// erases indices of the face inside the mesh
	void eraseFace(DFace* face);
	// !!! needs an EBO update !!!
	// erases indices of the edge inside the mesh
	void eraseEdge(DEdge* edge);

	void duplicateVertex(DVertex& vertex);

public:

	Mesh(std::string&& name, std::vector <DVertex>* vertices, std::vector <GLuint>& indices, const std::vector<GLuint>& edgeIndices = std::vector<GLuint>(), const  std::vector <Texture>& textures = std::vector<Texture>());

	~Mesh();

	std::vector<GLuint>& getIndices() { return indices; }

	void bindEBO();
	void updateEBO();
	void updateEdgeEBO();


	
	std::unordered_set<DFace*> getAllFaces(); 
	std::unordered_set<DEdge*> getAllEdges();

	DFace* getFace(std::vector<int> indices); // returns the common face of indices 

	DEdge* getEdge(int start, int end); // returns the common edge of indices 

	

	void Draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) override;

	void Translate(glm::vec3& translateVector)override;
	void Translate(float x, float y, float z)override;

	void Rotate(float degrees, const glm::vec3& axisVector)override;

	void Scale(glm::vec3& scaleVector)override;
	void Scale(float x, float y, float z)override;


	
	GLuint extrudeVertex(GLuint vertex);

	void deleteVertices();
	void deleteEdges();
	void deleteFaces();
	void deleteOnlyEdgesAndFaces();
	void deleteOnlyFaces();
	void dissolveVertices();
	void dissolveEdges();
	void dissolveFaces();




	std::vector<int>& getSelectedVertices(); 
	std::vector<DEdge*>& getSelectedEdges();
	std::vector<DFace*>& getSelectedFaces();

	
	std::vector<int> getFaceIndices(DFace* face);
	std::pair<int,int> getEdgeIndices(DEdge* edge);

	std::vector<GLuint> formTrianglesForDrawing();


};
