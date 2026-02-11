#pragma once

#include "Object.h"
#include "GeometryUtils.h"
#include "Aliases.h"
#include "unordered_set"
#include "UVVertex.h"

#include "Triangle.h"

//#include "Material.h"
#include "GPUVertex.h"
#include "map"
#include "json/json.h"

using json = nlohmann::json;



class Material;
class Mesh : public Object {



	std::vector<GLuint>indices; // used for drawing faces
	std::vector<GLuint>edgeIndices; // used for drawing edges
	EBO edgeEBO;

	std::vector<int> selectedVertexIndices = std::vector<int>(0);
	std::vector<DFace*>selectedFaces;
	std::vector<DEdge*>selectedEdges;

	// attributes used to render a textured mesh
			// rethink this approach

	std::unordered_map<Material*, std::unordered_set<DFace*>>materials;
	std::unordered_map < Material*, std::tuple < VAO, VBO, EBO, std::vector<GPUVertex>, std::vector<GLuint >> > renderBuffers;

	std::vector<Triangle> triangles; // used for raytracing
	std::vector<TriangleMaterial> triangleMaterialData; // used for raytracing

	



	// attributes used for UV editing
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

	template <typename Container, typename = FaceContainer<Container>>
	void setSelectedVertexIndicesFromFaces(const Container& faces);

	template <typename Container, typename = EdgeContainer<Container>>
	void setSelectedVertexIndicesFromEdges(const Container& edges);

	DEdge* createEdgeForFill(int a, int b, DFace* face);
	DVertex* duplicateVertex(DVertex& vertex);

	// only works for quads
	DEdge* getOpossingEdge(DEdge* edge, DFace* face);
	std::pair<DEdge*, DEdge*> getTwoIncidentEdges(DEdge* edge, DFace* face);


	void lscmFaceIndicesHelper(std::vector<GLuint>& F, int index, DLoop* loop);
public:

	Mesh(std::string&& name, std::vector <DVertex*> vertices, std::vector <GLuint>& indices, const std::vector<GLuint>& edgeIndices = std::vector<GLuint>(), const  std::vector <Texture>& textures = std::vector<Texture>());
	Mesh(const char* file);

	~Mesh();

	void translate(glm::vec3& translateVector)override;
	void translate(float x, float y, float z)override;

	void rotate(float degrees, const glm::vec3& axisVector)override;

	void scale(glm::vec3& scaleVector)override;
	void scale(float x, float y, float z)override;


	void draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES, bool outline = false) override;

	void materialDraw(Camera& camera);
	void renderDraw(Camera& camera);

	void bindEBO();
	void updateEBO();
	void updateEdgeEBO();

	std::vector<GLuint>& getIndices() { return indices; }
	int getNumberOfEdges() { return edgeIndices.size() * 0.5; }
	// rethink getAllFaces approach later
	int getNumberOfFaces() { return getAllFaces().size(); };
	std::vector<GLuint> formTrianglesForDrawing();




	// MESH EDITING (test all of these thoroughly)

			// DELETE

	template <typename Container, typename = IntContainer<Container>>
	void deleteVertices(Container& vertIndices, bool update = false);

	template <typename Container, typename = EdgeContainer<Container>>
	void deleteEdges(Container& edges, bool update = false);

	template <typename Container, typename = FaceContainer<Container>>
	void deleteFaces(Container& faces, bool update = false);

	template <typename Container, typename = EdgeContainer<Container>>
	void deleteOnlyEdgesAndFaces(Container& edges, bool update = false);

	template <typename Container, typename = FaceContainer<Container>>
	void deleteOnlyFaces(Container& faces, bool update = false);


	void dissolveVertices();
	void dissolveEdges();
	void dissolveFaces();




	// EXTRUDE


	void extrudeVertices(std::vector<int>& verts, bool update = false);

	// returns middle edge(s)
	template <typename Container, typename = EdgeContainer<Container>>
	std::unordered_set<DEdge*>  extrudeEdges(Container& edges, bool update = false);

	template <typename Container, typename = FaceContainer<Container>>
	void extrudeFaces(Container& faces, bool update = false);

	template <typename Container, typename = FaceContainer<Container>>
	void extrudeIndividualFaces(Container& faces, bool update = false);

	// !!! NOT IMPLEMENTED !!!
	void extrudeManifold();
	// !!! NOT IMPLEMENTED !!!
	void extrudeAlongNormals();
	// !!! NOT IMPLEMENTED !!!
	void extrudeRepeat();
	// !!! NOT IMPLEMENTED !!!
	void spin();




	// DUPLICATE


	std::vector<DVertex*> duplicateVertices(std::vector<int>& verts, bool update = false);

	template <typename Container, typename = EdgeContainer<Container>>
	std::vector<DEdge*> duplicateEdges(Container& edges, bool update = false);

	template <typename Container, typename = FaceContainer<Container>>
	std::vector<DFace*> duplicateFaces(Container& faces, bool update = false);




	// FILL


	DEdge* edgeFill(std::vector<int>& verts, bool update = false);
	DFace* faceFill(std::vector<int>& verts, bool windingOrderSet = false, bool update = false);


	// INSET


	void inset(std::vector<DFace*> faces);
	void insetIndividual(std::vector<DFace*> faces);


	// OTHER OPERATIONS

	template <typename Container, typename = FaceContainer<Container>>
	void pokeFaces(Container& faces, bool update = false);

	template <typename Container, typename = FaceContainer<Container>>
	void triangulateFaces(Container& faces, bool update = false);

	void trisToQuads(std::unordered_set < DFace*>& faces, bool update = false);

	void bridgeFaces(DFace* faceA, DFace* faceB, bool update = false);

	template <typename Container, typename = FaceContainer<Container>>
	std::vector<DFace*> separate(Container faces);

	template <typename Container, typename = FaceContainer<Container>>
	void flipFaceNormals(Container& faces);

	std::unordered_set<DVertex*> linearSubdivision();

	void loopCut(DEdge* edge, int numberOfCuts);

	void mergeVertices(std::vector<int>& verts);

	void edgeScale(DEdge* edge, float delta, bool update = false);

	std::vector<glm::vec3> getSlideClampMax(std::unordered_set<DVertex*> neighbours);
	std::vector<glm::vec3> getSlideDirections(DVertex* vert, std::unordered_set<DVertex*> neighbours);
	std::vector<glm::vec2> getSlideUnprojectedDirections(DVertex* vert, std::unordered_set<DVertex*> neighbours);





	// UV MAPPING


	void spitUVsAlongSeams();
	void mergeUVs();
	void findUVIslands(std::vector<std::unordered_set<DFace*>>& islands);
	void formUVTopology();

	// this method is commicaly inneficient, but due to time constraints it will have to do for now
	void lscmUVUnwrap();


	std::vector<std::shared_ptr<UVVertex>>& getUVCoords() { return uvCoords; }
	std::shared_ptr<UVVertex> getUVVertex(int index) { return uvCoords[index]; }
	std::vector<GLuint>& getUVEdgeIndices() { return uvEdgeindices; }
	std::vector<std::shared_ptr<UVVertex>>& getSelectedUVs() { return selectedUVVertices; }




	// MATERIALS

	std::unordered_map<Material*, std::unordered_set<DFace*>>* getAllMaterials() { return &materials; }
	void addMaterial(Material* mat);
	void assignMaterial(Material* mat);
	void removeMaterial(Material* mat);


	void buildGPUVertices();
	//void updateGpuVBO() { gpuVBO.bufferData(renderVerts); }




	// DMESH HELPER FUNCTIONS

	std::unordered_set<DFace*> getAllFaces();
	std::unordered_set<DEdge*> getAllEdges();

	DFace* getFace(std::unordered_set<int> indices); // returns the common face of indices 
	DEdge* getEdge(int start, int end); // returns the common edge of indices 
	DEdge* getEdge(DVertex* start, DVertex* end); // returns the common edge of vertices 

	std::vector<int> getFaceIndices(DFace* face);
	std::pair<int, int> getEdgeIndices(DEdge* edge);




	// SELECTION
	std::vector<int>& getSelectedVertices() { return selectedVertexIndices; };
	std::vector<DEdge*>& getSelectedEdges() { return selectedEdges; };
	std::vector<DFace*>& getSelectedFaces() { return selectedFaces; };





	// raytracing

	void formTrianglesForRaytracing(int rnd);

	std::vector<Triangle>& getRaytracingTriangleData() { return triangles; };
	std::vector<TriangleMaterial>& getRaytracingMaterialData() { return triangleMaterialData; };




};
