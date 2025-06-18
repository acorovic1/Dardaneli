#pragma once

#include"Object.h"
#include "unordered_set"

class Mesh :public Object {

	std::vector<Texture>textures;

	std::vector<GLuint>indices; // used for drawing faces
	EBO edgeEBO;
	std::vector<GLuint>edgeIndices; // used for drawing edges

	std::vector<int> selectedVertexIndices = std::vector<int>(0);
	std::vector<Face*>selectedFaces;
	std::vector<Edge*>selectedEdges;

	// !!! needs an EBO update !!!
// erases indices of the face inside the mesh
	void eraseFace(Face* face);
	// !!! needs an EBO update !!!
	// erases indices of the edge inside the mesh
	void eraseEdge(Edge* edge);

public:

	Mesh(std::string&& name, std::vector <Vertex>* vertices, std::vector <GLuint>& indices, const std::vector<GLuint>& edgeIndices = std::vector<GLuint>(), const  std::vector <Texture>& textures = std::vector<Texture>());

	~Mesh();

	std::vector<GLuint>& getIndices() { return indices; }

	void bindEBO();
	void updateEBO();
	void updateEdgeEBO();

	// gets all faces
	std::vector<Face*> getAllFaces(); // logic can/needs to be improved
	// gets all edges
	std::vector<Edge*> getAllEdges();

	Face* getFace(std::vector<int> indices); // returns the common face of indices 

	Edge* getEdge(int start, int end);

	GLuint extrudeVertex(GLuint vertex);

	void Draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) override;

	void Translate(glm::vec3& translateVector)override;
	void Translate(float x, float y, float z)override;

	void Rotate(float degrees, const glm::vec3& axisVector)override;

	void Scale(glm::vec3& scaleVector)override;
	void Scale(float x, float y, float z)override;


	void deleteVertices();
	void deleteEdges();
	void deleteFaces();
	void deleteOnlyEdgesAndFaces();
	void deleteOnlyFaces();
	void dissolveVertices();
	void dissolveEdges();
	void dissolveFaces();




	std::vector<int>& getSelectedVertices(); //
//	std::vector<int>& getSelectedEdgeIndices() { return selectedEdges; }; //
//	std::vector<int>& getSelectedFaceIndices() { return selectedFaces; };  //in the form of n1 xyz n2 abcd n3 klmn | where n = number of vertices of the face 
//	void setSelectedEdges(std::vector<int>& vec) { selectedEdges = vec; }
//	void setSelectedFaces(std::vector<int>& vec) { selectedFaces = vec; } //in the form of n1 xyz n2 abcd n3 klmn | where n = number of vertices of the face 

	std::vector<Edge*>& getSelectedEdges();
	std::vector<Face*>& getSelectedFaces();

	int getVertexIndex(Vertex* v);
	std::vector<int> getFaceIndices(Face* face);
	std::pair<int,int> getEdgeIndices(Edge* edge);

	std::vector<GLuint> formTrianglesForDrawing();


};
