#include "Scene.h"
#include "DFace.h"
#include "Window.h"
#include "Lights/PointLight.h"
#include "Lights/DirectionalLight.h"
#include "Lights/SpotLight.h"
Scene::Scene() {}

void Scene::Init(Window&window)
{

	

	new Camera (800, 600, glm::vec3(-2.0f, 3.0f, 6.0f), "Viewport");
	new Camera (800, 600, glm::vec3( 0.5f, 0.5f, 1.0f), "UV");
	new Camera (800, 600, glm::vec3( 0.0f, 0.0f, 6.0f), "Shader");



	auto camera = cameraSingleton->getCamera("Viewport");
	camera->setPerspectiveProjection(45, float(camera->getWidth()) / float(camera->getHeight()), 0.1f, 100.0f);

	camera = cameraSingleton->getCamera("UV");
	
	camera->setOrientation(glm::vec3(0.0f, 0.0f, -1.0f)); // looking down the -Z axis

	float aspect = float(window.getWidth()) / float(window.getHeight());
	camera->setProjection(glm::ortho(aspect * -1.0f, aspect * 1.0f, -1.0f, 1.0f, -2000.0f, 300000.0f));

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

	new Shader("UV", "src/shaders/UV.vert", "src/shaders/UV.frag");
	new Shader("Light", "src/shaders/light.vert", "src/shaders/light.frag");
	new Shader("TextureTest", "src/shaders/textureTest.vert", "src/shaders/textureTest.frag");

	//new PointLight("test");
	new SpotLight("test");

	


	// napravljen output za color 
	// compilirati ostale outpute kada je render preview, ali prvo napravi svjetla


	// svjetla napravljena ostalo jos SCALE I ROTATE metode da se naprave, koristi model matricu
	
	
	//objectSingleton->getObject(0)->Rotate(45.0f, glm::vec3(1.0f, 0.0f, 1.0f));



}