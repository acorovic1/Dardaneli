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

class MyGUI  {
	ImGuiIO* io;
	ImGuiIO* gizmoIo;
	GLFWwindow* glfwWindow;

	float position[3] = { 0.0f, 0.0f, 0.0f };
	float positionPrev[3] = { 0.0f, 0.0f, 0.0f };
	float rotation[3] = { 0.0f, 0.0f, 0.0f };
	float rotationPrev[3] = { 0.0f, 0.0f, 0.0f };
	float scale[3] = { 1.0f, 1.0f, 1.0f };
	float scalePrev[3] = { 1.0f, 1.0f, 1.0f };

	DLoop* selectedLoop = nullptr;

	//DFace* selectedFace = nullptr;

	// used only for disk.d1
	DEdge* vertexEdge = nullptr;

	VAO grid3DVAO;
	EBO grid3DEBO;
	std::vector<glm::vec2> gridVertices3D;
	std::vector<GLuint> gridIndices3D;

	VAO grid2DVAO;
	EBO grid2DEBO;
	std::vector<glm::vec2> gridVertices2D;
	std::vector<GLuint> gridIndices2D;


	ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;

	float hoverTime = 0.0f;
	int BVHSubd = 0;
	int eBVHSubd = 0;
	char searchText[32] = "Search";

	bool BVHTree = false;
	bool gizmo = true;
	bool faceCulling = true;

	bool showAddMenuFlag = false;
	bool showDeleteMenuFlag = false;
	bool showExtrudeMenuFlag = false;
	bool showInsetMenuFlag = false;



public:

	MyGUI(GLFWwindow* glfwWindow);
	void init();
	void newFrame();
	void render();
	void shutdown();

	void drawUI();
	void drawObjectModeUI(bool change);
	void drawEditModeUI(bool change);
	void drawUVModeUI(bool change);
	void drawShaderEditorUI(bool change);

	ImGuiIO* getIO() { return io; };
	GLFWwindow* getGLFWwindow() { return glfwWindow; }
	Window* getWindow() { return reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow)); }
	Mode getMode() { return app->mode; };
	SelectMode getSelectMode() { return app->selectMode; };
	bool getFaceCulling() { return faceCulling; }
	std::vector<int>& getObjectIndex() { return app->objectIndices; };

	void showAddMenu();
	void showDeleteMenu();
	void showExtrudeMenu();
	void showInsetMenu();

	void addMenu();
	void deleteMenu();
	void extrudeMenu();
	

	void drawBVH();

	void gizmos();

	void vertexTransform();
	void transformations();
	void selectObject();

	void initializeGrid3D(int width = 20);
	void drawGrid3D();
	void initializeGrid2D(int width = 10);
	void drawGrid2D();

	void setGizmoOperation(ImGuizmo::OPERATION op);

	void modes();

	void dMesh();

	void shaderNodeEditor();
	void addShadingNodes();


	void saveFinalRender(const char* filename, int width, int height);


};
