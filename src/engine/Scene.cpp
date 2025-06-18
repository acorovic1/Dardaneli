#include "Scene.h"
#include "Face.h"
Scene::Scene() {}

void Scene::Init()
{
	auto camera = cameraSingleton->getCamera(0);
	camera->setProjectionMatrix(45, float(camera->getWidth()) / float(camera->getHeight()), 0.1f, 100.0f);

	// uljepsaj ovo svega ti
	// nek ide samo name.vert/name.frag a onda u konstruktoru dodaj ovu putanju ostalu
	new Shader("EditMode", "src/shaders/editMode.vert", "src/shaders/editMode.frag");
	new Shader("AABB", "src/shaders/BVH.vert", "src/shaders/BVH.frag");
	new Shader("Grid", "src/shaders/grid.vert", "src/shaders/grid.frag");

	new Shader("Basic", "src/shaders/basic.vert", "src/shaders/basic.frag");

	new Shader("Select", "src/shaders/selectObject.vert", "src/shaders/selectObject.frag");
	new Shader("ActiveSelect", "src/shaders/activeObject.vert", "src/shaders/activeObject.frag");

	new Shader("SelectEdit", "src/shaders/selectEdit.vert", "src/shaders/selectEdit.frag");
	new Shader("ActiveEdit", "src/shaders/activeEdit.vert", "src/shaders/activeEdit.frag");



	addCube();



	Mesh* m1 = static_cast<Mesh*>(objectSingleton->getObject(0));

	auto indices = m1->getIndices();


	std::cout << "\n Start \n";
	for (int k = 0;k < indices.size();k++)
	{
		if (k && k % 3 == 0)std::cout << "\n";
		std::cout << indices[k] << " ";
	}




}