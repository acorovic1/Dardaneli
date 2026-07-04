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
//#include "Utilities/FileSystem.h"


#include "FaceBVH.h"
#include "Enums.h"

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

	friend class MyGUI;

	Mode mode = Mode::OBJECT;
	SelectMode selectMode = SelectMode::VERTEX;

	RenderMode renderMode = RenderMode::SOLID;


	std::vector<int> selectedObjects = std::vector<int>(1);







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

	void updateGUI(glm::vec3 offset, MyGUI& gui, Operation op);

	std::vector<int>& getSelectedObjects() { return selectedObjects; }



	void objectMode(Window* window); //Inputs
	void editMode(Window* window); //Inputs
	void uVMode(Window* window); //Inputs
	void materialEditor(Window* window); //Inputs
	void inputs(Window* window); //Inputs

	

	void deleteObjects();



};

extern Application* app;