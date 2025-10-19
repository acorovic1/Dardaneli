#pragma once

#define GLFW_INCLUDE_NONE

#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_glfw.h>
#include <ImGUI/imgui_impl_opengl3.h>

#include "ImGUIZMO/ImGuizmo.h"

#include <glad/glad.h>
#include "ObjectManager.h"
#include "CameraManager.h"
#include "ObjectModeBVH.h"
#include "VertexBVH.h"
#include "UVVertexBVH.h"
#include "EdgeBVH.h"
#include "Object.h"
#include "BasicObjects.h"
#include "Application.h"
#include "ImGUI/imnodes.h"
#include "ShadingNodes/Material.h"
//#include "ImNODES/imnodes.cpp"
//#include "ImNODES/imnodes_internal.h"

class MyGUI /* :public Dardaneli...varijable pod protected staviti*/ /* mozda app i gui da su odvojene (kao sto su i do sada) i obje da budu singleton(samo jos gui napravit da je singleton )*/ {
	ImGuiIO* io;
	ImGuiIO* gizmoIo;
	Window* window;
	ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;

	float hoverTime = 0.0f;

	bool BVHTree = false;
	bool faceCulling = true;
	int BVHSubd = 0;
	int eBVHSubd = 0;

	char searchText[32] = "Search";

	bool gizmo = true;



	DLoop* selectedLoop= nullptr;
	//DFace* selectedFace = nullptr;

	// used only for disk.d1
	DEdge* vertexEdge = nullptr;

	std::vector<glm::vec2> gridVertices3D;
	std::vector<GLuint> gridIndices3D;
	std::vector<glm::vec2> gridVertices2D;
	std::vector<GLuint> gridIndices2D;

	VAO grid3DVAO;
	EBO grid3DEBO;

	VAO grid2DVAO;
	EBO grid2DEBO;

	

public:
	void SaveFinalRender(const char* filename, int width, int height);
	bool showAddMenu = false;
	bool showDeleteMenu = false;
	bool showExtrudeMenu = false;
	bool showInsetMenu = false;
	MyGUI(Window* window);
	void Init();
	void NewFrame();
	void Render();
	void Shutdown();
	void DrawUI();
	ImGuiIO* getIO();
	Mode getMode();
	SelectMode getSelectMode();
	bool getFaceCulling() { return faceCulling; }

	std::vector<int>& getObjectIndex();

	void AddMenu();
	void Add();
	void DeleteMenu();
	void Delete();
	void ExtrudeMenu();
	void Extrude();
	void InsetMenu();
	void Inset();

	void DrawBVH();

	void Gizmos();

	void VertexTransform();
	void Transformations();
	void SelectObject();

	void InitializeGrid3D(int width = 20);
	void Grid3D();
	void InitializeGrid2D(int width = 10);
	void Grid2D();

	void setGizmoOperation(ImGuizmo::OPERATION op);

	void Modes();

	void DMesh();

	void ShowShaderEditor();
	void addShadingNodes();


};
