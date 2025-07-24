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
	void setWindingOrder(std::vector<int>& verts);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DFace*>::value
	>::type>
	void setSelectedVertexIndicesFromFaces(const Container& faces);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DEdge*>::value
	>::type>
	void setSelectedVertexIndicesFromEdges(const Container& edges);


	DEdge* createEdgeForFill(int a, int b, DFace* face);


	
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

	std::vector<glm::vec3> getSlideClampMax( std::unordered_set<DVertex*> neighbours);
	std::vector<glm::vec3> getSlideDirections(DVertex* vert, std::unordered_set<DVertex*> neighbours);
	std::vector<glm::vec2> getSlideUnprojectedDirections(DVertex* vert, std::unordered_set<DVertex*> neighbours);

	void extrudeVertices(std::vector<int>&verts,bool update = false);
	void extrudeEdges(std::vector<DEdge*>&edges, bool update = false);
	void extrudeFaces(std::vector<DFace*>&faces,bool update = false);
	void extrudeIndividualFaces(std::vector<DFace*>&faces,bool update = false);

	// !!! NOT IMPLEMENTED !!!
	void extrudeManifold();
	// !!! NOT IMPLEMENTED !!!
	void extrudeAlongNormals();
	// !!! NOT IMPLEMENTED !!!
	void extrudeRepeat();
	// !!! NOT IMPLEMENTED !!!
	void spin();

	std::vector<DFace*> separate(std::vector<DFace*>faces);



	std::vector<DVertex*> duplicateVertices(std::vector<int>&verts, bool update = false);
	std::vector<DEdge*> duplicateEdges(std::vector<DEdge*>&edges, bool update = false);
	std::vector<DFace*> duplicateFaces(std::vector<DFace*>&faces, bool update = false);


	// test all of these thoroughly 
	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, int>::value
	>::type>
	void deleteVertices(Container& vertIndices, bool update = false);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DEdge*>::value
	>::type>
	void deleteEdges(Container& edges, bool update = false);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DFace*>::value
	>::type>
	void deleteFaces(Container& faces, bool update = false);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DEdge*>::value
	>::type>
	void deleteOnlyEdgesAndFaces(Container& edges, bool update = false);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DFace*>::value
	>::type>
	void deleteOnlyFaces(Container& faces, bool update = false);


	void dissolveVertices();
	void dissolveEdges();
	void dissolveFaces();


	DEdge* edgeFill(std::vector<int>& verts);
	DFace* faceFill(std::vector<int>& verts,bool windingOrderSet=false,bool update=false);


	std::vector<int>& getSelectedVertices();
	std::vector<DEdge*>& getSelectedEdges();
	std::vector<DFace*>& getSelectedFaces();


	std::vector<int> getFaceIndices(DFace* face);
	std::pair<int, int> getEdgeIndices(DEdge* edge);

	std::vector<GLuint> formTrianglesForDrawing();




};
