#include "Scene.h"
#include "Mesh/DFace.h"
#include "Window.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "Lights/SpotLight.h"

#include "RaytracingBVH.h"
#include <stack>






Scene::Scene() {}

void Scene::init(Window& window)
{



	new Camera(800, 600, glm::vec3(-2.0f, 3.0f, 6.0f), "Viewport");
	new Camera(800, 600, glm::vec3(0.5f, 0.5f, 1.0f), "UV");
	new Camera(800, 600, glm::vec3(0.0f, 0.0f, 6.0f), "Shader");



	auto camera = cameraSingleton->getCamera("Viewport");
	camera->setPerspectiveProjection(45, float(camera->getWidth()) / float(camera->getHeight()), 0.1f, 100.0f);

	camera = cameraSingleton->getCamera("UV");

	camera->setOrientation(glm::vec3(0.0f, 0.0f, -1.0f)); // looking down the -Z axis

	float aspect = float(window.getWidth()) / float(window.getHeight());
	camera->setProjection(glm::ortho(aspect * -1.0f, aspect * 1.0f, -1.0f, 1.0f, -2000.0f, 300000.0f));



	new Shader("Basic", "basic.vert", "basic.frag");
	new Shader("UV", "UV.vert", "UV.frag");
	new Shader("Grid", "grid.vert", "grid.frag");



	app->activeMaterial = new Material("Default");



	addCube();

	dynamic_cast<Mesh*>(objectSingleton->getObject(0))->assignMaterial(app->activeMaterial);








	// svjetla napravljena ostalo jos SCALE I ROTATE metode da se naprave, koristi model matricu




}