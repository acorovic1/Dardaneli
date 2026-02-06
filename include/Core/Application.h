#pragma once
#pragma once


#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


#include "FaceBVH.h"
#include "EditorModes.h"

//#include "Material.h"

#define radian 180/3.14159265358979323846f
#define myEpsilon 0.00001



class MyGUI;
class Window;
class Material;
// cut off one head two more shall take its place
class Application { 
	static Application* instance;
	Application() {};

	Mode mode = Mode::OBJECT;
	SelectMode selectMode = SelectMode::VERTEX;

	RenderMode renderMode = RenderMode::RENDER;


	std::vector<int> objectIndices = std::vector<int>(1);



	float vertexPosition[3] = { 0.0f,0.0f,0.0f };
	float vertexPrevPosition[3] = { 0.0f,0.0f,0.0f };



	bool slide = false;
	bool slideFirstClick=false;
	double slideStartX=0, slideStartY=0;
	std::vector< std::vector<glm::vec3>>slideLengths;
	std::vector< std::vector<glm::vec3>> slideDirections;
	std::vector< std::vector<glm::vec2>> slideUnProjectedDirections;
	std::vector<glm::vec3> bestDirection;
	std::vector<glm::vec3> startPositions;

public:
	Material* activeMaterial;

	static Application* getInstance();

	Application(const Application& copy) = delete;
	void operator=(const Application& copy) = delete;


	Mode getMode() { return mode; }
	RenderMode getRenderMode() { return renderMode; }

	void setSelectMode(SelectMode mode);

	void updateTranslate(glm::vec3 offset);//Updates translate for gui

	void updateVertexPosition(glm::vec3 offset);

	void objectMode(Window* window); //Inputs
	void editMode(Window* window); //Inputs
	void uVMode(Window* window); //Inputs
	void materialEditor(Window* window); //Inputs
	void inputs(Window* window); //Inputs

	Object* getActiveObject() {
		if (!objectIndices.size() /*|| objectIndices.back() < 0 || objectIndices.back() >= objectSingleton->getNumberOfObjects()*/)
			return nullptr;
		return objectSingleton->getObject(objectIndices.back());
	}

	void deleteObjects();



	friend class MyGUI;
};

extern Application* app;