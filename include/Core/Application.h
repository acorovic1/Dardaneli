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
class Viewport;
class Material;
// cut off one head two more shall take its place
class Application { 
	static Application* instance;
	Application() {};

	friend class MyGUI;

	//Mode mode = Mode::OBJECT;
	//SelectMode selectMode = SelectMode::VERTEX;
	//RenderMode renderMode = RenderMode::SOLID;

	std::vector<int> selectedObjects = std::vector<int>(1);


public:
	Material* activeMaterial;

	static Application* getInstance();

	Application(const Application& copy) = delete;
	void operator=(const Application& copy) = delete;



	//void setSelectMode(SelectMode mode);

	void updateGUI(glm::vec3 offset, MyGUI& gui, Operation op);

	std::vector<int>& getSelectedObjects() { return selectedObjects; }

	void objectMode(Window* window,Viewport* viewport); //Inputs
	void editMode(Window* window,Viewport* viewport); //Inputs
	void uVMode(Window* window,Viewport* viewport); //Inputs
	void materialEditor(Window* window,Viewport* viewport); //Inputs
	void inputs(Window* window,Viewport* viewport); //Inputs


	void init();
	void newFrame();

};

extern Application* app;