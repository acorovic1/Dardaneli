#pragma once


#include <iostream>
#include <algorithm>
#include <vector>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Window.h"
#include "FaceBVH.h"

#define radian 180/3.14159265358979323846f
#define epsilon 0.00001

enum class Mode { OBJECT, EDIT, SCULPT, WEIGHT_PAINT, TEXTURE_PAINT };
enum class SelectMode { VERTEX, EDGE, FACE };

class MyGUI;

// cut off one head two more shall take its place
class Application {
	static Application* instance;
	Application() {};

	Mode mode = Mode::OBJECT;
	SelectMode selectMode = SelectMode::VERTEX;

	std::vector<int> objectIndices = std::vector<int>(1);

	float translate[3] = { 0.0f, 0.0f, 0.0f };
	float translatePrev[3] = { 0.0f, 0.0f, 0.0f };
	float rotate[3] = { 0.0f, 0.0f, 0.0f };
	float rotatePrev[3] = { 0.0f, 0.0f, 0.0f };
	float scale[3] = { 1.0f, 1.0f, 1.0f };
	float scalePrev[3] = { 1.0f, 1.0f, 1.0f };

	float vertexPosition[3] = { 0.0f,0.0f,0.0f };
	float vertexPrevPosition[3] = { 0.0f,0.0f,0.0f };

	glm::mat4 model = glm::mat4(1.0f); // treba GUIu... nez zasto je ovde.. pogledaj nekad



public:

	static Application* getInstance();

	Application(const Application& copy) = delete;
	void operator=(const Application& copy) = delete;


	void setSelectMode(SelectMode mode);

	void updateTranslate(glm::vec3 offset);//Updates translate for gui

	void updateVertexPosition(glm::vec3 offset);

	void ObjectMode(GLFWwindow* window, MyGUI& gui); //Inputs
	void EditMode(GLFWwindow* window, MyGUI& gui); //Inputs
	void Inputs(GLFWwindow* window, MyGUI& gui); //Inputs



	void deleteObjects();

	

	friend class MyGUI;
};

extern Application* app;