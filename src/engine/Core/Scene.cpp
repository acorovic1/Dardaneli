#include "Scene.h"

#include "Mesh/DFace.h"
#include "Window.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "Lights/SpotLight.h"


#include "RaytracingBVH.h"
#include <stack>
#include <Improved/ObjectModeBVHImproved.h>






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


	// for n = 4 or higher driver malfunctions lol?

	int N = 3;                 // cubes per axis → generates N³ cubes
	float spacing = 2.0f;      // distance between cube centers

	for (int x = 0; x < N; ++x)
	{
		for (int y = 0; y < N; ++y)
		{
			//if (y % 2 == 0)continue;
			for (int z = 0; z < N; ++z)
			{
				int id = objectSingleton->getNumberOfObjects();
				addCube();

				Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getObject(id));
				mesh->assignMaterial(app->activeMaterial);

				// place them centered
				float fx = (x - (N - 1) * 0.5f) * spacing;
				float fy = (y - (N - 1) * 0.5f) * spacing;
				float fz = (z - (N - 1) * 0.5f) * spacing;

				glm::vec3 translateVector = glm::vec3(fx, fy, fz);

				mesh->translate(translateVector);
			}
		}
	}

	//addCube();
	//addCube();
	//addCube();
	//dynamic_cast<Mesh*>(objectSingleton->getObject(0))->assignMaterial(app->activeMaterial);
	//dynamic_cast<Mesh*>(objectSingleton->getObject(1))->assignMaterial(app->activeMaterial);
	//dynamic_cast<Mesh*>(objectSingleton->getObject(2))->assignMaterial(app->activeMaterial);
	//glm::vec3 t1(-3.0f, 3.0f, 0.0f);
	//glm::vec3 t2(3.0f, -3.0f, 0.0f);
	//dynamic_cast<Mesh*>(objectSingleton->getObject(0))->translate(t1);
	//dynamic_cast<Mesh*>(objectSingleton->getObject(1))->translate(t2);
	//dynamic_cast<Mesh*>(objectSingleton->getObject(2))->translate(t2);

	//objectBVHSingleton->Refit();


	// u mesh constructoru zamijeni ove BVHove.. kao i svugdje ostalo

	objectBVHImprovedSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());




	// svjetla napravljena ostalo jos SCALE I ROTATE metode da se naprave, koristi model matricu



	// postavi nove BVHove svugdje i stavi logn max dubinu u edit modu



}