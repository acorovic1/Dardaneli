#pragma once

#define GLFW_INCLUDE_NONE

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include "ImGUIZMO/ImGuizmo.h"

#include <glad/glad.h>
#include "ObjectManager.h"
#include "CameraManager.h"
#include "ObjectModeBVH.h"
#include "EditModeBVH.h"
#include "Object.h"
#include "BasicObjects.h"

#define radian 180/3.14159265358979323846f
#define epsilon 0.00001

enum class Mode{OBJECT,EDIT,SCULPT,WEIGHT_PAINT,TEXTURE_PAINT};
enum class SelectMode{VERTEX,EDGE,FACE};


class MyGUI /* :public Dardaneli...varijable pod protected staviti*/ {

	ImGuiIO* io;
	ImGuiIO* gizmoIo;
	ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
	Mode mode = Mode::OBJECT;
	SelectMode selectMode = SelectMode::VERTEX;

	 float hoverTime = 0.0f;
	 

	 bool BVHTree = false;
	 int BVHSubd = 0;
	 int eBVHSubd = 0;

	 char searchText[32] = "Search";

	 bool gizmo = true;
	 std::vector<glm::vec2> vertices;
	 std::vector<GLuint> indices;
	 VAO VAO;
	
	int objectIndex = 0;
	int vertexIndex = 0;

	float translate[3] = { 0.0f, 0.0f, 0.0f };
	float translatePrev[3] = { 0.0f, 0.0f, 0.0f };
	float rotate[3] = { 0.0f, 0.0f, 0.0f };
	float rotatePrev[3] = { 0.0f, 0.0f, 0.0f };
	float scale[3] = { 1.0f, 1.0f, 1.0f };
	float scalePrev[3] = { 1.0f, 1.0f, 1.0f };

	float vertexPosition[3] = { 0.0f,0.0f,0.0f };
	float vertexPrevPosition[3] = { 0.0f,0.0f,0.0f };

	glm::mat4 model = glm::mat4(1.0f);



public:
	bool showAddMenu = false;
	MyGUI();
	void Init(GLFWwindow* window);
	void NewFrame();
	void Render();
	void Shutdown();
	void DrawUI();
	ImGuiIO* getIO();
	Mode getMode();
	SelectMode getSelectMode();

	int& getObjectIndex();
	int& getVertexIndex();

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

