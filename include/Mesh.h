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
	// erases indices of the face inside the mesh and updates the DMesh structure accordingly
	void eraseFace(DFace* face);
	// !!! needs an EBO update !!!
	// erases indices of the edge inside the mesh
	void eraseEdge(DEdge* edge);

	// !!! needs a VBO update !!!
	// erases the vertex from vertices vector and updates the indices of the mesh
	void eraseVertex(DVertex* v);


	// sets the winding order for the SELECTED vertices !!!
	void setWindingOrder();


	DEdge* createEdgeForFill(int a,int b,DFace* face);


	// not implemented yet!!!
	DVertex* duplicateVertex(DVertex& vertex);

public:

	Mesh(std::string&& name, std::vector <DVertex*> vertices, std::vector <GLuint>& indices, const std::vector<GLuint>& edgeIndices = std::vector<GLuint>(), const  std::vector <Texture>& textures = std::vector<Texture>());

	~Mesh();

	std::vector<GLuint>& getIndices() { return indices; }

	void bindEBO();
	void updateEBO();
	void updateEdgeEBO();


	
	std::unordered_set<DFace*> getAllFaces(); 
	std::unordered_set<DEdge*> getAllEdges();

	DFace* getFace(std::unordered_set<int> indices); // returns the common face of indices 

	DEdge* getEdge(int start, int end); // returns the common edge of indices 


	void Draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) override;

	void Translate(glm::vec3& translateVector)override;
	void Translate(float x, float y, float z)override;

	void Rotate(float degrees, const glm::vec3& axisVector)override;

	void Scale(glm::vec3& scaleVector)override;
	void Scale(float x, float y, float z)override;


	
	void extrudeVertex(DVertex* vertex, bool update = false);
	void extrudeEdge(DEdge* edge,bool update = false);
	void extrudeFace(DFace* face,bool update = false);


	// test all of this thoroughly 

	// if there is only one edge connected to the vertex, deletes the edge(both vertices)
	void deleteVertices();

	void deleteEdges();
	void deleteFaces();
	void deleteOnlyEdgesAndFaces();
	void deleteOnlyFaces();


	void dissolveVertices();
	void dissolveEdges();
	void dissolveFaces();


	void fill();


	std::vector<int>& getSelectedVertices(); 
	std::vector<DEdge*>& getSelectedEdges();
	std::vector<DFace*>& getSelectedFaces();

	
	std::vector<int> getFaceIndices(DFace* face);
	std::pair<int,int> getEdgeIndices(DEdge* edge);

	std::vector<GLuint> formTrianglesForDrawing();


};
