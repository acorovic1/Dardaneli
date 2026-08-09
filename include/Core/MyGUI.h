#pragma once

#define GLFW_INCLUDE_NONE

#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_glfw.h>
#include <ImGUI/imgui_impl_opengl3.h>

#include "ImGUIZMO/ImGuizmo.h"

// Required for ImGuiDockBuilder functions
 // Include if using ImHashStr, or use the integer trick below
#include <ImGui/imgui_internal.h>

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
#include "Material.h"

class Viewport;
class MyGUI  {
	ImGuiIO* io;
	ImGuiIO* gizmoIo;
	GLFWwindow* glfwWindow;

	friend class Application;

	
	// gui data
	static float position[3];
	static float rotation[3];
	static float scale[3];

	static ImGuizmo::OPERATION operation;

	// used for gizmo operations
	static glm::mat4 transform;
	static glm::mat4 previousTransform;





	// used only for disk.d1
	DEdge* vertexEdge = nullptr;
	DLoop* selectedLoop = nullptr;
	//DFace* selectedFace = nullptr;

	VAO grid3DVAO;
	EBO grid3DEBO;
	std::vector<glm::vec2> gridVertices3D;
	std::vector<GLuint> gridIndices3D;

	VAO grid2DVAO;
	EBO grid2DEBO;
	std::vector<glm::vec2> gridVertices2D;
	std::vector<GLuint> gridIndices2D;



	float hoverTime = 0.0f;
	int BVHSubd = 0;
	int eBVHSubd = 0;
	char searchText[32] = "Search";

	bool BVHTree = false;
	bool BVHRayVisualize = false;
	bool gizmo = false;
	bool faceCulling = true;

	

	bool showAddMenuFlag = false;
	bool showDeleteMenuFlag = false;
	bool showExtrudeMenuFlag = false;
	bool showImportDialog = false;
	bool showViewportActions = false;
	bool showViewportModes = false;

	bool viewportAdjust = true;
	ViewportBoundary viewportAdjustBoundary = ViewportBoundary::LEFT;



public:

	MyGUI(GLFWwindow* glfwWindow);
	void init();
	void newFrame();
	void render();
	void shutdown();



	ImGuiIO* getIO() { return io; };
	GLFWwindow* getGLFWwindow() { return glfwWindow; }
	Window* getWindow() { return reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow)); }
	//Mode getMode() { return app->mode; };
	//SelectMode getSelectMode() { return app->selectMode; };
	bool getFaceCulling() { return faceCulling; }
	std::vector<int>& getObjectIndex() { return app->selectedObjects; };



	void drawGeneral(Viewport* viewport);

	void drawTopBar();
	void drawObjectTools(Viewport* viewport);
	void drawEditTools(Viewport* viewport);
	void drawUVTools(Viewport* viewport);
	void drawNodeTools(Viewport* viewport);

	void drawHierarchy(Viewport* viewport);
	void drawRenderModeOptions(Viewport* viewport);
	void drawBVH(Viewport* viewport);

	void showAddMenu();
	void showDeleteMenu();
	void showExtrudeMenu();
	void showViewportActionsMenu();
	void showViewportModesMenu();
	

	void addMenu();
	void deleteMenu();
	void extrudeMenu();
	void viewportActionsMenu();

	void viewportModeMenu();

	bool isViewportAdjusted();
	void stopViewportAdjustment();

	void displayBVH(Camera* camera);
	void BVHRayInteraction(Viewport* viewport);

	void gizmos();

	void vertexTransform();
	void transformations();
	void selectObject();

	void initializeGrid3D(int width = 20);
	void drawGrid3D(Viewport* viewport);
	void initializeGrid2D(int width = 10);
	void drawGrid2D(Viewport* viewport);

	void setGizmoOperation(ImGuizmo::OPERATION op);

	void modes(Viewport* viewport);

	void dMesh();

	void shaderNodeEditor(Viewport* viewport);
	void addShadingNodes();


	void pbrRender(const char* filename, int width, int height);
	void raytraceRender(const char* filename, int width, int height);



	void importObject();



};
