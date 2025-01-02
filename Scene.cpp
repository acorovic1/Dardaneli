#include "Scene.h"



Scene::Scene() {}



void Scene::Init()
{
	
	auto camera = cameraSingleton->getCamera(0);
	camera->setProjectionMatrix(glm::perspective(glm::radians(45.0f), float(camera->getWidth()) / float(camera->getHeight()), 0.1f, 100.0f));
	
	new Shader ("Basic","basic.vert", "basic.frag");
	new Shader ("EditMode", "editMode.vert", "editMode.frag");
	new Shader ("AABB","BVH.vert", "BVH.frag");
	new Shader ("Grid", "grid.vert", "grid.frag");
	new Shader ("Select","selectObject.vert", "selectObject.frag");
	new Shader ("SelectEdit","selectEdit.vert", "selectEdit.frag");
	
	addCube();
	




}


