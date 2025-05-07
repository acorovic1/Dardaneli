#pragma once

#define GLFW_INCLUDE_NONE

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ImGUIZMO/ImGuizmo.h"

#include <glad/glad.h>
#include "ObjectManager.h"
#include "CameraManager.h"
#include "ObjectModeBVH.h"
#include "VertexBVH.h"
#include "EdgeBVH.h"
#include "Object.h"
#include "BasicObjects.h"
#include "Application.h"

class MyGUI /* :public Dardaneli...varijable pod protected staviti*/ {
	ImGuiIO* io;
	ImGuiIO* gizmoIo;
	Window* window;
	ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;

	float hoverTime = 0.0f;

	bool BVHTree = false;
	int BVHSubd = 0;
	int eBVHSubd = 0;

	char searchText[32] = "Search";

	bool gizmo = true;

	Edge* selectedEdge = nullptr;
	Face* selectedFace = nullptr;

	std::vector<glm::vec2> gridVertices;
	std::vector<GLuint> gridIndices;

	VAO gridVAO;

public:
	bool showAddMenu = false;
	MyGUI(Window* window);
	void Init();
	void NewFrame();
	void Render();
	void Shutdown();
	void DrawUI();
	ImGuiIO* getIO();
	Mode getMode();
	SelectMode getSelectMode();

	std::vector<int>& getObjectIndex();

	void Add();
	void AddMenu();

	void DrawBVH();

	void Gizmos();

	void VertexTransform();
	void Transformations();
	void SelectObject();
	void Grid();
	void InitializeGrid(int width = 20);

	void setGizmoOperation(ImGuizmo::OPERATION op);

	void Modes();
};
