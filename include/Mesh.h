#pragma once

#include"Object.h"
#include "GeometryUtils.h"
#include "unordered_set"
#include "UVVertex.h"


class Mesh : public Object {

	std::vector<Texture>textures;



	std::vector<GLuint>indices; // used for drawing faces
	EBO edgeEBO;
	std::vector<GLuint>edgeIndices; // used for drawing edges

	std::vector<int> selectedVertexIndices = std::vector<int>(0);
	std::vector<DFace*>selectedFaces;
	std::vector<DEdge*>selectedEdges;



	std::vector<std::shared_ptr<UVVertex>>uvCoords;
	std::vector<GLuint>uvEdgeindices;
	std::vector<GLuint>uvIndices; // used for drawing faces

	std::vector<std::shared_ptr<UVVertex>> selectedUVVertices;
	std::unordered_map<UVVertex*, int> uvIndexMap;


	// !!! needs an EBO update !!!
	// erases indices of the face inside the mesh and updates DMesh structure accordingly
	void eraseFace(DFace* face);
	// !!! needs an EBO update !!!
	// erases indices of the edge inside the mesh and updates DMesh structure accordingly

	void eraseEdge(DEdge* edge, bool vertex = true);

	// !!! needs a VBO update !!!
	// erases the vertex from vertices vector and updates the indices of the mesh
	void eraseVertex(DVertex* v);


	glm::vec3 setWindingOrder(std::vector<int>& verts);
	glm::vec3 setWindingOrder(std::vector<DVertex*>& verts);

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

	// only works for quads
	DEdge* getOpossingEdge(DEdge* edge, DFace* face);
	std::pair<DEdge*, DEdge*> getTwoIncidentEdges(DEdge* edge, DFace* face);


	void lscmFaceIndicesHelper(std::vector<GLuint>& F, int index, DLoop* loop);
public:
	void spitUVsAlongSeams();
	void mergeUVs();
	void formUVTopology();
	// this method is commicaly inneficient, but due to time constraints it will have to do for now
	void lscmUVUnwrap();

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
	DEdge* getEdge(DVertex* start, DVertex* end); // returns the common edge of vertices 


	void Draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) override;

	void Translate(glm::vec3& translateVector)override;
	void Translate(float x, float y, float z)override;

	void Rotate(float degrees, const glm::vec3& axisVector)override;

	void Scale(glm::vec3& scaleVector)override;
	void Scale(float x, float y, float z)override;

	void edgeScale(DEdge* edge, float delta, bool update = false);

	void inset(std::vector<DFace*> faces);
	void insetIndividual(std::vector<DFace*> faces);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DFace*>::value
	>::type>
	void pokeFaces(Container& faces, bool update = false);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DFace*>::value
	>::type>
	void triangulateFaces(Container& faces, bool update = false);

	void bridgeFaces(DFace* faceA, DFace* faceB, bool update = false);

	void trisToQuads(std::unordered_set < DFace*>& faces, bool update = false);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DFace*>::value
	>::type>
	void flipFaceNormals(Container& faces);

	std::unordered_set<DVertex*> linearSubdivision();

	void loopCut(DEdge* edge, int numberOfCuts);
	void mergeVertices(std::vector<int>& verts);

	std::vector<glm::vec3> getSlideClampMax(std::unordered_set<DVertex*> neighbours);
	std::vector<glm::vec3> getSlideDirections(DVertex* vert, std::unordered_set<DVertex*> neighbours);
	std::vector<glm::vec2> getSlideUnprojectedDirections(DVertex* vert, std::unordered_set<DVertex*> neighbours);

	void extrudeVertices(std::vector<int>& verts, bool update = false);

	// returns middle edges
	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DEdge*>::value
	>::type>
	std::unordered_set<DEdge*>  extrudeEdges(Container& edges, bool update = false);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DFace*>::value
	>::type>
	void extrudeFaces(Container& faces, bool update = false);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DFace*>::value
	>::type>
	void extrudeIndividualFaces(Container& faces, bool update = false);

	// !!! NOT IMPLEMENTED !!!
	void extrudeManifold();
	// !!! NOT IMPLEMENTED !!!
	void extrudeAlongNormals();
	// !!! NOT IMPLEMENTED !!!
	void extrudeRepeat();
	// !!! NOT IMPLEMENTED !!!
	void spin();


	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DFace*>::value
	>::type>
	std::vector<DFace*> separate(Container faces);



	std::vector<DVertex*> duplicateVertices(std::vector<int>& verts, bool update = false);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DEdge*>::value
	>::type>
	std::vector<DEdge*> duplicateEdges(Container& edges, bool update = false);

	template <typename Container,
		typename = typename std::enable_if<
		std::is_same<typename Container::value_type, DFace*>::value
	>::type>
	std::vector<DFace*> duplicateFaces(Container& faces, bool update = false);


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


	DEdge* edgeFill(std::vector<int>& verts, bool update = false);


	DFace* faceFill(std::vector<int>& verts, bool windingOrderSet = false, bool update = false);
	//DFace* faceFill(std::unordered_set<int>& verts, bool windingOrderSet = false, bool update = false);


	std::vector<int>& getSelectedVertices();
	std::vector<DEdge*>& getSelectedEdges();
	std::vector<DFace*>& getSelectedFaces();

	std::vector<std::shared_ptr<UVVertex>>& getSelectedUVs() { return selectedUVVertices; }

	std::vector<std::shared_ptr<UVVertex>>& getUVCoords() { return uvCoords; }
	std::shared_ptr<UVVertex> getUVVertex(int index) { return uvCoords[index]; }
	void findUVIslands(std::vector<std::unordered_set<DFace*>>& islands);

	std::vector<GLuint>& getUVEdgeIndices() { return uvEdgeindices; }


	std::vector<int> getFaceIndices(DFace* face);
	std::pair<int, int> getEdgeIndices(DEdge* edge);

	std::vector<GLuint> formTrianglesForDrawing();




};
