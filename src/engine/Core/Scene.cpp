#include "Scene.h"

#include "Mesh/DFace.h"
#include "Window.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "Lights/SpotLight.h"


#include "RaytracingBVH.h"
#include <stack>
#include <Improved/ObjectModeBVHImproved.h>


// U MATERIAL PREVIEW CURI MEMORIJA



Scene::Scene() {}

void Scene::init(Window& window)
{



	new Camera(800, 600, glm::vec3(-2.0f, 3.0f, 5.0f), "Viewport");
	new Camera(800, 600, glm::vec3(0.5f, 0.5f, 1.0f), "UV");
	new Camera(800, 600, glm::vec3(0.0f, 0.0f, 6.0f), "Shader");




	auto camera = cameraSingleton->getCamera("Viewport");
	camera->setPerspectiveProjection(45, float(camera->getWidth()) / float(camera->getHeight()), 0.1f, 10000.0f);

	camera = cameraSingleton->getCamera("UV");

	camera->setOrientation(glm::vec3(0.0f, 0.0f, -1.0f)); // looking down the -Z axis

	float aspect = float(window.getWidth()) / float(window.getHeight());
	camera->setProjection(glm::ortho(aspect * -1.0f, aspect * 1.0f, -1.0f, 1.0f, -2000.0f, 300000.0f));



	new Shader("Basic", "basic.vert", "basic.frag");
	new Shader("BVH", "BVH.vert", "BVH.frag");
	new Shader("UV", "UV.vert", "UV.frag");
	new Shader("Grid", "grid.vert", "grid.frag");
	new Shader("ComputeShader", "computeTest.comp");
	new Shader("ComputeOutput", "computeOutput.vert", "computeOutput.frag");

	app->activeMaterial = new Material("Default");

	// U buildGPUvertices.. samo quadovi mogu trenutno.. napraviti i za trokut


	int N = 1;                 // cubes per axis → generates N³ cubes
	float spacing = 2.0f;      // distance between cube centers

	//for (int x = 0; x < N; ++x)
	//{
	//	for (int y = 0; y < N; ++y)
	//	{
	//		//if (y % 2 == 0)continue;
	//		for (int z = 0; z < N; ++z)
	//		{
	//			int id = objectSingleton->getNumberOfObjects();
	//			addCube();

	//			Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getObject(id));
	//			mesh->assignMaterial(app->activeMaterial);

	//			auto& selectedEdges = mesh->getSelectedEdges();


	//			auto selectedEdgesSet = mesh->getAllEdges();
	//			selectedEdges.assign(selectedEdgesSet.begin(), selectedEdgesSet.end());


	//			for (DEdge* edge : mesh->getSelectedEdges())
	//				edge->isSeam = true;

	//			mesh->lscmUVUnwrap();

	//			std::vector<int>& vertexIndices = mesh->getSelectedVertices();
	//			//vertexIndices()

	//			// place them centered
	//			float fx = (x - (N - 1) * 0.5f) * spacing;
	//			float fy = (y - (N - 1) * 0.5f) * spacing;
	//			float fz = (z - (N - 1) * 0.5f) * spacing;

	//			glm::vec3 translateVector = glm::vec3(fx, fy, fz);

	//			mesh->translate(translateVector);

	//			//std::cout << "\nAdded cube at (" << fx << "," << fy << "," << fz << ")";
	//		}
	//	}
	//}
	//
	


	//addPlane();
	addCube();
	//addSphere();
	//addCylinder();
	//addCone();
	//addDoughnut();

	Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getObject(0));
	mesh->assignMaterial(app->activeMaterial);


	//new PointLight("Point Light");
	//new DirectionalLight("DirLight");
	//new SpotLight("Spot");


	//Mesh* mesh1 = dynamic_cast<Mesh*>(objectSingleton->getObject(0));
	//Mesh* mesh2 = dynamic_cast<Mesh*>(objectSingleton->getObject(2));
	//Mesh* mesh3 = dynamic_cast<Mesh*>(objectSingleton->getObject(3));
	//Mesh* mesh4 = dynamic_cast<Mesh*>(objectSingleton->getObject(4));
	//Mesh* mesh5 = dynamic_cast<Mesh*>(objectSingleton->getObject(5));


	//glm::vec3 translateVector1 = glm::vec3(4.0f,0.0f,0.0f);
	//glm::vec3 translateVector2 = glm::vec3(0.0f,0.0f,-4.0f);
	//glm::vec3 translateVector3 = glm::vec3(4.0f,0.0f,-4.0f);
	//glm::vec3 translateVector4 = glm::vec3(-4.0f,0.0f,0.0f);
	//glm::vec3 translateVector5 = glm::vec3(-4.0f,0.0f,-4.0f);

	//mesh1->translate(translateVector1);
	//mesh2->translate(translateVector2);
	//mesh3->translate(translateVector3);
	//mesh4->translate(translateVector4);
	//mesh5->translate(translateVector5);






	//addCube();
	//addCube();
	//addCube();
	//dynamic_cast<Mesh*>(objectSingleton->getObject(0))->rotate(90, glm::vec3(1.0f, 0.0f, 0.0f));
	//dynamic_cast<Mesh*>(objectSingleton->getObject(0))->assignMaterial(app->activeMaterial);
	//dynamic_cast<Mesh*>(objectSingleton->getObject(1))->assignMaterial(app->activeMaterial);
	//dynamic_cast<Mesh*>(objectSingleton->getObject(2))->assignMaterial(app->activeMaterial);
	//glm::vec3 t1(-3.0f, 3.0f, 0.0f);
	//glm::vec3 t2(3.0f, -3.0f, 0.0f);
	//dynamic_cast<Mesh*>(objectSingleton->getObject(0))->translate(t1);
	//dynamic_cast<Mesh*>(objectSingleton->getObject(1))->translate(t2);
	//dynamic_cast<Mesh*>(objectSingleton->getObject(2))->translate(t2);

	//objectBVHSingleton->Refit();




	objectBVHImprovedSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());




	// svjetla napravljena ostalo jos SCALE I ROTATE metode da se naprave, koristi model matricu



	// postavi nove BVHove svugdje i stavi logn max dubinu u edit modu

	//RaytracingBVHSingleton->Build();
	//RaytracingBVHSingleton->init(window.getWidth(), window.getHeight());

}