#include "MyGUI.h"


#include <iomanip>
#include <chrono>
#include <unordered_set>

#include "glad/glad.h"
#include "stb/stb_image_write.h"

#include "glm/gtx/euler_angles.hpp"

#include "Window.h"
#include "Mesh/DFace.h"
#include "Mesh/DLoop.h"
#include "MaterialManager.h"
#include "RaytracingBVH.h"
#include "Utilities/FragColor.h"
#include "Utilities/FileSystem.h"


#include "ShadingNodes/Texture/TextureNode.h"
#include "ShadingNodes/Math/MathNode.h"
#include "ShadingNodes/Input/ValueNode.h"
#include "ShadingNodes/Output/ColorOutputNode.h"
#include "ShadingNodes/Output/NormalOutputNode.h"
#include "ShadingNodes/Output/RoughnessOutputNode.h"
#include "ShadingNodes/Output/MetallicOutputNode.h"
#include "ShadingNodes/Output/AmbientOcclusionOutputNode.h"
#include "ShadingNodes/Input/ColorNode.h"
#include "ShadingNodes/Color/ColorMixNode.h"


#include "Lights/Light.h"
#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"
#include "Lights/SpotLight.h"

#if defined(_WIN32)
#include <windows.h>
#endif

#include <stack>
#include <Improved/ObjectModeBVHImproved.h>
#include <Improved/EdgeBVHImproved.h>
#include <Improved/VertexBVHImproved.h>
#include <Improved/FaceBVHImproved.h>

#include "Viewport.h"



// gui data
float MyGUI::position[3] = { 0.0f, 0.0f, 0.0f };
float MyGUI::rotation[3] = { 0.0f, 0.0f, 0.0f };
float MyGUI::scale[3] = { 1.0f, 1.0f, 1.0f };

ImGuizmo::OPERATION MyGUI::operation = ImGuizmo::OPERATION::TRANSLATE;

// used for gizmo operations
glm::mat4 MyGUI::transform = glm::mat4(1.0f);
glm::mat4 MyGUI::previousTransform = glm::mat4(1.0f);



MyGUI::MyGUI(GLFWwindow* glfwWindow) : glfwWindow(glfwWindow) { MyGUI::init(); }

void MyGUI::init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	gizmoIo = &ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(this->glfwWindow, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	ImNodes::CreateContext();

	initializeGrid2D();
	initializeGrid3D();
}
void MyGUI::newFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}
void MyGUI::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void MyGUI::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImNodes::DestroyContext();
	ImGui::DestroyContext();
}



void MyGUI::drawUI(Viewport* viewport)
{
	ImGui::Begin("##1");
	

	if (ImGui::Button("File "))
	{
		ImGui::OpenPopup("FileMenu");
	}

	if (ImGui::BeginPopup("FileMenu"))
	{
		if (ImGui::MenuItem("Import"))
			showImportDialog = true;


		ImGui::EndPopup();
	}
	if (showImportDialog)
		importObject();


	//Mode pastMode = viewport->getMode();

	//modes(viewport);
	//ImGui::SameLine();

	//Mode& currentMode = viewport->getMode();


	if (ImGui::Button("Render Scene"))
	{
		ImGui::OpenPopup("RenderSceneMenu");
	}

	if (ImGui::BeginPopup("RenderSceneMenu"))
	{
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(glfwWindow, &fbWidth, &fbHeight);

		if (ImGui::MenuItem("PBR"))
			pbrRender("final_render.png", fbWidth, fbHeight);

		if (ImGui::MenuItem("Raytrace"))
			raytraceRender("final_render.png", fbWidth, fbHeight);

		ImGui::EndPopup();
	}


	ImGui::End();


	if (showViewportActions)
		viewportActionsMenu();

	if (showViewportModes)
		viewportModeMenu();

	if (viewportAdjust)
	{
		Window* window = getWindow();
		Viewport* viewport = window->getViewportAtCursor();
		viewport->resize(window, viewportAdjustBoundary);
	}




}

void MyGUI::drawObjectModeUI(Viewport* viewport)
{

	static int choice = 0;
	ImGui::RadioButton("Default", &choice, 0);ImGui::SameLine();
	ImGui::RadioButton("BVH", &choice, 1);ImGui::SameLine();
	ImGui::RadioButton("Ray Interaction", &choice, 2);
	ImGui::InputInt("BVH Depth", &BVHSubd);
	if (BVHSubd > log(objectSingleton->getNumberOfObjects()) / log(2))
		BVHSubd = static_cast<int>(log(objectSingleton->getNumberOfObjects()) / log(2));
	if (BVHSubd < 0)
		BVHSubd = 0;
	ImGui::Checkbox("FaceCulling", &faceCulling);
	ImGui::Checkbox("Gizmo", &gizmo);

	if (gizmo)
		gizmos();

	if (showAddMenuFlag)
		addMenu();





	/*if (choice == 1)
	{
		drawBVH();
	}
	else if (choice == 2)
	{
		BVHRayInteraction();
	}*/

	if (app->selectedObjects.size())
	{

		int& lastIndex = app->selectedObjects.back();

		if (ImGui::InputInt("Index", &lastIndex))
		{
			(ImGui::InputInt("Index", &lastIndex));
			if (lastIndex < 0) lastIndex = 0;
			if (lastIndex >= objectSingleton->getNumberOfObjects()) lastIndex = objectSingleton->getNumberOfObjects() - 1;

			selectObject();
		}
	}

	transformations();

}

void MyGUI::drawEditModeUI(Viewport* viewport)
{
	Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getActiveObject());

	SelectMode& selectMode = viewport->getSelectMode();

	Camera* camera = viewport->getCamera(CameraTypes::VIEWPORT);


	static int choice = 0;
	ImGui::RadioButton("Default", &choice, 0);ImGui::SameLine();
	ImGui::RadioButton("BVH", &choice, 1);ImGui::SameLine();
	ImGui::RadioButton("Ray Interaction", &choice, 2);

	if (choice == 1)
	{
		static Shader& bvh = shaderSingleton->getShader("BVH");
		bvh.setVector4f(false, "color", FragColor::BVH);

		//basic.setBool(true, "BVH", true);
		//if (app->selectMode == SelectMode::VERTEX)
		//	VertexBVHSingleton->DrawLeaves(VertexBVHSingleton->getRoot(), *cameraSingleton->getCamera(0), basic);
		//else if (app->selectMode == SelectMode::EDGE)
		//	EdgeBVHSingleton->DrawLeaves(EdgeBVHSingleton->getRoot(), *cameraSingleton->getCamera(0), basic);
		//else if (app->selectMode == SelectMode::FACE)
		//	FaceBVHSingleton->DrawLeaves(FaceBVHSingleton->getRoot(), *cameraSingleton->getCamera(0), basic);

		if (selectMode == SelectMode::VERTEX)
			VertexBVHImprovedSingleton->Draw(*camera, bvh, eBVHSubd);
		else if (selectMode == SelectMode::EDGE)
			EdgeBVHImprovedSingleton->Draw(*camera, bvh, eBVHSubd);
		else if (selectMode == SelectMode::FACE)
			FaceBVHImprovedSingleton->Draw(*camera, bvh, eBVHSubd);

		//basic.setBool(true, "BVH", false);


	}
	else if (choice == 2)
	{


		BVHRayInteraction();
	}

	if (showDeleteMenuFlag)
		deleteMenu();

	if (showExtrudeMenuFlag)
		extrudeMenu();



	ImGui::InputInt("BVH Height", &eBVHSubd);
	int height;
	if (selectMode == SelectMode::VERTEX)
		height = log(mesh->getNumberOfVertices()) / log(2);
	else if (selectMode == SelectMode::EDGE)
		height = log(mesh->getNumberOfEdges()) / log(2);
	else
		height = log(mesh->getNumberOfFaces()) / log(2);

	if (eBVHSubd > height)
		eBVHSubd = height;
	if (eBVHSubd < 0)
		eBVHSubd = 0;

	ImGui::Checkbox("FaceCulling", &faceCulling);

	ImGui::Button("Mark seam");
	if (ImGui::IsItemClicked())
	{
		ImGui::SetTooltip("Mark seam for selected edges");

		std::cout << "\nSeams marked";

		for (DEdge* edge : mesh->getSelectedEdges())
			edge->isSeam = true;

		mesh->lscmUVUnwrap();
	}


	ImGui::Button("Clear seam");
	if (ImGui::IsItemClicked())
	{
		ImGui::SetTooltip("Clear seam for selected edges");

		for (DEdge* edge : mesh->getSelectedEdges())
			edge->isSeam = false;

		mesh->mergeUVs();
	}

	int e = (int)selectMode;

	ImGui::RadioButton("DVertex select", &e, 0); ImGui::SameLine();
	ImGui::RadioButton("Edge select", &e, 1); ImGui::SameLine();
	ImGui::RadioButton("DFace select", &e, 2);

	if (e == 0)selectMode = SelectMode::VERTEX;
	else if (e == 1)selectMode = SelectMode::EDGE;
	else if (e == 2)selectMode = SelectMode::FACE;


	std::vector<int>& vertexIndicesTemp = mesh->getSelectedVertices();
	int size = vertexIndicesTemp.size();

	if (size)
	{
		int& lastIndex = vertexIndicesTemp.back();
		(ImGui::InputInt("Index", &lastIndex));
		if (lastIndex < 0) lastIndex = 0;
		if (lastIndex >= mesh->getVertices().size()) lastIndex = mesh->getVertices().size() - 1;
	}



	vertexTransform();
	dMesh();


}

void MyGUI::drawUVModeUI(Viewport* viewport)
{
	ImGui::Checkbox("BVHTree", &BVHTree);

	Mesh* mesh = static_cast<Mesh*>(objectSingleton->getObject(app->selectedObjects.back()));
	UVVertexBVHSingleton->BuildBottomUp(*mesh); // prebaci ovo na unwrap funkciju
	static Shader basic = shaderSingleton->getShader("Basic");

	Camera* camera = viewport->getCamera(CameraTypes::UV);

	if (BVHTree)
	{
		basic.setVector4f(false, "color", FragColor::BVH);
		UVVertexBVHSingleton->DrawLeaves(UVVertexBVHSingleton->getRoot(), *camera, basic);
	}

}

void MyGUI::drawShaderEditorUI(Viewport* viewport)
{
	if (showAddMenuFlag) addShadingNodes();


	Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getActiveObject());


	//ImGui::Begin("Materials");

	static int current = 0;



	//**************
	// extremely inefficient code, however materials rarely have a large count so it should be fine 
	std::vector<Material*> materials;
	if (current == 0)
	{
		if (mesh)
			for (auto mat : *mesh->getAllMaterials())
				materials.push_back(mat.first);
	}
	else
		materials = materialSingleton->getAllMaterials();

	std::sort(materials.begin(), materials.end(),
		[](Material* a, Material* b) {
			return a->getName() < b->getName();
		});
	//**************


	ImGui::RadioButton("All Materials", &current, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Object Materials", &current, 0);

	if (mesh)
	{
		if (ImGui::Button("Assign Material")) mesh->assignMaterial(app->activeMaterial);
		ImGui::SameLine();

		if (ImGui::Button("Remove Material")) mesh->removeMaterial(app->activeMaterial);
		ImGui::SameLine();
	}

	if (ImGui::Button("Add new material")) mesh->addMaterial(new Material("Material " + std::to_string(materials.size())));


	static int selectedIndex = -1;
	static int renameIndex = -1; // index of the material being renamed
	static char renameBuf[128] = {};

	for (int i = 0; i < materials.size(); ++i) {
		bool isSelected = (selectedIndex == i);

		if (ImGui::Selectable(materials[i]->getName().c_str(), isSelected)) {

			if (selectedIndex != i) {
				renameIndex = -1;
			}
			selectedIndex = i;

			app->activeMaterial->getNodePositions();   // save old material node positions
			app->activeMaterial = materials[i];
			app->activeMaterial->setNodePositions();   // set new material node positions
		}


		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
			renameIndex = i;
			std::string currentName = materials[i]->getName();
			strncpy_s(renameBuf, sizeof(renameBuf), currentName.c_str(), _TRUNCATE);
		}

		if (isSelected) {
			ImGui::SetItemDefaultFocus();
		}


		if (renameIndex == i) {
			ImGui::Indent();
			if (ImGui::InputText("##Rename", renameBuf, IM_ARRAYSIZE(renameBuf),
				ImGuiInputTextFlags_EnterReturnsTrue)) {
				materials[i]->setName(std::string(renameBuf));
				renameIndex = -1;
			}
			ImGui::Unindent();
		}
	}




}



void MyGUI::showAddMenu() { showAddMenuFlag = true; }
void MyGUI::showDeleteMenu() { showDeleteMenuFlag = true; }
void MyGUI::showExtrudeMenu() { showExtrudeMenuFlag = true; }
void MyGUI::showViewportActionsMenu() { showViewportActions = true; }
void MyGUI::showViewportModesMenu() { showViewportModes = true; }



void MyGUI::addMenu() {
	hoverTime = glfwGetTime();

	ImGui::OpenPopup("Add popup");

	if (ImGui::BeginPopup("Add popup"))
	{
		ImGui::SeparatorText("Add");
		ImGui::Separator();
		ImGui::InputText("WIP", searchText, IM_ARRAYSIZE(searchText));
		ImGui::Separator();

		if (ImGui::BeginMenu("Mesh"))
		{
			ImGui::SeparatorText("Primitives");

			if (ImGui::MenuItem("Plane"))
			{
				addPlane();
				std::cout << "Plane added \n";
			}
			if (ImGui::MenuItem("Cube"))
			{
				addCube();
				std::cout << "Cube added \n";
			}
			if (ImGui::MenuItem("Circle"))
			{
				addCircle();
				std::cout << "Circle added \n";
			}
			if (ImGui::MenuItem("Sphere"))
			{
				addSphere();
				std::cout << "Sphere added \n";
			}
			if (ImGui::MenuItem("Cylinder"))
			{
				addCylinder();
				std::cout << "Cylinder added \n";
			}
			if (ImGui::MenuItem("Cone"))
			{
				addCone();
				std::cout << "Cone added \n";
			}
			if (ImGui::MenuItem("Doughnut"))
			{
				addDoughnut();
				std::cout << "Doughnut added \n";
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Curve"))
		{
			ImGui::MenuItem("Cruve test");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Metaball"))
		{
			ImGui::MenuItem("Metaball test");
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Armature"))
		{
			ImGui::MenuItem("Armature test");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Lattice"))
		{
			ImGui::MenuItem("Lattice test");
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Empty"))
		{
			ImGui::MenuItem("Empty test");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Image")) {
			ImGui::MenuItem("Image test");
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Light"))
		{
			if (ImGui::MenuItem("Point light"))
			{
				new PointLight("Point Light");
				std::cout << "\n\nPoint light added";
			}
			if (ImGui::MenuItem("Directional light"))
			{

				new DirectionalLight("Directional Light");
				std::cout << "\n\nDirectional light added";
			}
			if (ImGui::MenuItem("Spot light"))
			{

				new SpotLight("Spot Light");
				std::cout << "\n\nSpot light added";
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Force Field")) {
			ImGui::MenuItem("Force Field test");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Camera")) {
			ImGui::MenuItem("Camera test");
			ImGui::EndMenu();
		}

		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && hoverTime > 1.4)
		{
			glfwSetTime(0);
			hoverTime = 0;

			//getWindow()->getViewportAtCursor()->getKeys()[GLFW_KEY_Q] = 0;
			//std::cout << "HEHEHAHA ";
			showAddMenuFlag = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
void MyGUI::deleteMenu()
{

	hoverTime = glfwGetTime();
	static int selected_option = -1;
	const char* options[] = { "Vertices", "Edges", "Faces", "Only Edges & Faces", "Only Faces", "Dissolve Vertices | WIP","Dissolve Edges | WIP","Dissolve Faces | WIP" };

	ImGui::OpenPopup("Delete popup");

	if (ImGui::BeginPopup("Delete popup"))
	{
		ImGui::SeparatorText("Delete");
		ImGui::Separator();

		Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getObject(app->selectedObjects.back()));
		if (!mesh)return;

		for (int i = 0; i < 8; ++i) {
			if (i == 5) {
				ImGui::Separator();
			}

			if (ImGui::Selectable(options[i])) {
				selected_option = i;
			}
		}


		if (selected_option == 0)
			mesh->deleteVertices(mesh->getSelectedVertices(), true);
		else if (selected_option == 1)
			mesh->deleteEdges(mesh->getSelectedEdges(), true);
		else if (selected_option == 2)
			mesh->deleteFaces(mesh->getSelectedFaces(), true);
		else if (selected_option == 3)
			mesh->deleteOnlyEdgesAndFaces(mesh->getSelectedEdges(), true);
		else if (selected_option == 4)
			mesh->deleteOnlyFaces(mesh->getSelectedFaces(), true);
		else if (selected_option == 5)
			mesh->dissolveVertices();
		else if (selected_option == 6)
			mesh->dissolveEdges();
		else if (selected_option == 7)
			mesh->dissolveFaces();

		selected_option = -1;

		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && hoverTime > 1.4)
		{
			glfwSetTime(0);
			hoverTime = 0;

			getWindow()->getViewportAtCursor()->getKeys()[GLFW_KEY_X] = 0;
			//std::cout << "HEHEHAHA ";
			showDeleteMenuFlag = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
void MyGUI::extrudeMenu()
{
	//std::cout << "\n\n\tEXTRUDEEEE";

	hoverTime = glfwGetTime();
	static int selected_option = -1;
	const char* options[] = { "Vertices", "Edges", "Faces","Individual Faces","Manifold | WIP","Along Normals | WIP","Repeat | WIP", "Spin | WIP" };

	ImGui::OpenPopup("Extrude popup");

	if (ImGui::BeginPopup("Extrude popup"))
	{
		ImGui::SeparatorText("Extrude");
		ImGui::Separator();

		Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getObject(app->selectedObjects.back()));
		if (!mesh)return;

		for (int i = 0; i < 8; ++i) {
			if (i == 6) {
				ImGui::Separator();
			}

			if (ImGui::Selectable(options[i])) {
				selected_option = i;
			}
		}


		if (selected_option == 0)
			mesh->extrudeVertices(mesh->getSelectedVertices(), true);
		else if (selected_option == 1)
			mesh->extrudeEdges(mesh->getSelectedEdges(), true);
		else if (selected_option == 2)
			mesh->extrudeFaces(mesh->getSelectedFaces(), true);
		else if (selected_option == 3)
			mesh->extrudeIndividualFaces(mesh->getSelectedFaces(), true);
		else if (selected_option == 4)
			mesh->extrudeManifold();
		else if (selected_option == 5)
			mesh->extrudeAlongNormals();
		else if (selected_option == 6)
			mesh->extrudeRepeat();
		else if (selected_option == 7)
			mesh->spin();


		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && hoverTime > 1.4)
		{
			glfwSetTime(0);
			hoverTime = 0;
				std::cout << "\n\t HEllo";

			if (selected_option != -1)
			{
				getWindow()->getViewportAtCursor()->getKeys()[GLFW_KEY_G] = 1;
				std::cout << "\n\t [Operation] Move extruded elements";
			}
			//getWindow()->getViewportAtCursor()->getKeys()[GLFW_KEY_E] = 0;
			showExtrudeMenuFlag = false;
			ImGui::CloseCurrentPopup();
		}

		selected_option = -1;
		ImGui::EndPopup();
	}


}
void MyGUI::viewportActionsMenu()
{
	hoverTime = glfwGetTime();

	Window* window = getWindow();
	ImGui::OpenPopup("Viewport popup");
	//std::cout << "\n3\n";

	if (ImGui::BeginPopup("Viewport popup"))
	{
		ImVec2 pos = ImGui::GetWindowPos();
		ImGui::SeparatorText("Viewport");


		if (ImGui::BeginMenu("Split"))
		{
			if (ImGui::MenuItem("Horizontal"))
				window->addViewport(window->getViewportAtCursor(pos.x, pos.y), pos.x, pos.y, false);

			if (ImGui::MenuItem("Vertical"))
				window->addViewport(window->getViewportAtCursor(pos.x, pos.y), pos.x, pos.y, true);
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Join"))
		{
			if (ImGui::MenuItem("Left"))
				window->getViewportAtCursor(pos.x, pos.y)->joinViewport(window, ViewportBoundary::LEFT);

			if (ImGui::MenuItem("Right"))
				window->getViewportAtCursor(pos.x, pos.y)->joinViewport(window, ViewportBoundary::RIGHT);

			if (ImGui::MenuItem("Top"))
				window->getViewportAtCursor(pos.x, pos.y)->joinViewport(window, ViewportBoundary::TOP);

			if (ImGui::MenuItem("Bottom"))
				window->getViewportAtCursor(pos.x, pos.y)->joinViewport(window, ViewportBoundary::BOTTOM);

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Adjust"))
		{
			if (ImGui::MenuItem("Left"))
			{
				viewportAdjustBoundary = ViewportBoundary::LEFT;
				viewportAdjust = true;
			}

			if (ImGui::MenuItem("Bottom"))
			{
				viewportAdjustBoundary = ViewportBoundary::BOTTOM;
				viewportAdjust = true;
			}

			if (ImGui::MenuItem("Right"))
			{
				viewportAdjustBoundary = ViewportBoundary::RIGHT;
				viewportAdjust = true;
			}


			if (ImGui::MenuItem("Top"))
			{
				viewportAdjustBoundary = ViewportBoundary::TOP;
				viewportAdjust = true;
			}





			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Preset"))
		{
			if (ImGui::MenuItem("1x1"))
			{
				auto& vps = window->getViewports();
				vps.clear();

				vps.push_back(std::make_unique<Viewport>(0.0, 0.0, 1.0, 1.0, window->getGLFWwindow()));

			}


			if (ImGui::MenuItem("2x1"))
			{
				auto& vps = window->getViewports();
				vps.clear();

				vps.push_back(std::make_unique<Viewport>(0.0, 0.0, 1.0, 0.5, window->getGLFWwindow()));
				vps.push_back(std::make_unique<Viewport>(0.0, 0.5, 1.0, 1.0, window->getGLFWwindow()));
			}


			if (ImGui::MenuItem("1x2"))
			{
				auto& vps = window->getViewports();
				vps.clear();


				vps.push_back(std::make_unique<Viewport>(0.0, 0.0, 0.5, 1.0, window->getGLFWwindow()));
				vps.push_back(std::make_unique<Viewport>(0.5, 0.0, 1.0, 1.0, window->getGLFWwindow()));
			}

			if (ImGui::MenuItem("2x2"))
			{
				auto& vps = window->getViewports();
				vps.clear();


				vps.push_back(std::make_unique<Viewport>(0.0, 0.0, 0.5, 0.5, window->getGLFWwindow()));
				vps.push_back(std::make_unique<Viewport>(0.5, 0.0, 1.0, 0.5, window->getGLFWwindow()));
				vps.push_back(std::make_unique<Viewport>(0.0, 0.5, 0.5, 1.0, window->getGLFWwindow()));
				vps.push_back(std::make_unique<Viewport>(0.5, 0.5, 1.0, 1.0, window->getGLFWwindow()));
			}




			ImGui::EndMenu();
		}




		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && hoverTime > 1.4)
		{
			glfwSetTime(0);
			hoverTime = 0;

			showViewportActions = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
void MyGUI::viewportModeMenu()
{
	hoverTime = glfwGetTime();

	Window* window = getWindow();
	Viewport* viewport = window->getViewportAtCursor();

	ImGui::OpenPopup("Viewport Mode");

	if (ImGui::BeginPopup("Viewport Mode"))
	{
		ImGui::SeparatorText("Viewport");


		Mode& mode = viewport->getMode();
		SelectMode& selectMode = viewport->getSelectMode();
		RenderMode& renderMode = viewport->getRenderMode();


		if (ImGui::BeginMenu("Mode"))
		{
			if (ImGui::MenuItem("Object", nullptr, mode == Mode::OBJECT))
			{
				mode = Mode::OBJECT;
				viewport->setActiveCamera(CameraTypes::VIEWPORT);
				objectBVHImprovedSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
				std::cout << "\n\n OBJECT BVH BUILT ...";
			}
			if (ImGui::MenuItem("Edit", nullptr, mode == Mode::EDIT))

			{
				Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getActiveObject());
				if (mesh)
				{
					mode = Mode::EDIT;
					viewport->setActiveCamera(CameraTypes::VIEWPORT);
					double time = glfwGetTime();

					VertexBVHImprovedSingleton->BuildBottomUp(mesh);
					EdgeBVHImprovedSingleton->BuildBottomUp(*mesh);
					FaceBVHImprovedSingleton->BuildBottomUp(*mesh);

					std::cout << "All 3 BVHs built in " << glfwGetTime() - time << " seconds";
				}

			}

			if (ImGui::MenuItem("Sculpt", nullptr, mode == Mode::SCULPT)) {
				viewport->setActiveCamera(CameraTypes::VIEWPORT);
				mode = Mode::SCULPT;
			}

			if (ImGui::MenuItem("Weight Paint", nullptr, mode == Mode::WEIGHT_PAINT)) {
				viewport->setActiveCamera(CameraTypes::VIEWPORT);
				mode = Mode::WEIGHT_PAINT;
			}

			if (ImGui::MenuItem("Texture Paint", nullptr, mode == Mode::TEXTURE_PAINT)) {
				viewport->setActiveCamera(CameraTypes::VIEWPORT);
				mode = Mode::TEXTURE_PAINT;
			}

			if (ImGui::MenuItem("UV Edit", nullptr, mode == Mode::UV_EDIT))
			{
				mode = Mode::UV_EDIT;
				viewport->setActiveCamera(CameraTypes::UV);
			}

			if (ImGui::MenuItem("Shader Editor", nullptr, mode == Mode::SHADER_EDIT))
			{
				mode = Mode::SHADER_EDIT;
				viewport->setActiveCamera(CameraTypes::SHADER);
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("Render"))
		{
			if (ImGui::MenuItem("Wireframe", nullptr, renderMode == RenderMode::WIREFRAME)) renderMode = RenderMode::WIREFRAME;
			if (ImGui::MenuItem("Solid", nullptr, renderMode == RenderMode::SOLID)) renderMode = RenderMode::SOLID;
			if (ImGui::MenuItem("Material Preview", nullptr, renderMode == RenderMode::MATERIAL_PREVIEW)) renderMode = RenderMode::MATERIAL_PREVIEW;
			if (ImGui::MenuItem("Render", nullptr, renderMode == RenderMode::RENDER)) renderMode = RenderMode::RENDER;

			ImGui::EndMenu();
		}


		ImGui::BeginDisabled(viewport->getMode() != Mode::EDIT);

		if (ImGui::BeginMenu("Selection"))
		{
			if (ImGui::MenuItem("Vertex", nullptr, selectMode == SelectMode::VERTEX)) selectMode = SelectMode::VERTEX;
			if (ImGui::MenuItem("Edge", nullptr, selectMode == SelectMode::EDGE)) selectMode = SelectMode::EDGE;
			if (ImGui::MenuItem("Face", nullptr, selectMode == SelectMode::FACE)) selectMode = SelectMode::FACE;

			ImGui::EndMenu();
		}

		ImGui::EndDisabled();


		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && hoverTime > 1.4)
		{
			glfwSetTime(0);
			hoverTime = 0;

			showViewportModes = false;
			ImGui::CloseCurrentPopup();
		}


		ImGui::EndPopup();
	}

}
bool MyGUI::isViewportAdjusted()
{
	return viewportAdjust;
}

void MyGUI::stopViewportAdjustment()
{
	viewportAdjust = false;
}


void MyGUI::drawBVH(Camera* camera) {
	static Shader& bvh = shaderSingleton->getShader("BVH");
	bvh.setVector4f(false, "color", FragColor::BVH);


	objectBVHImprovedSingleton->Draw(*camera, bvh, BVHSubd);



	//basic.setVector4f(false, "color", FragColor::Seams);
}

void MyGUI::BVHRayInteraction()
{
	/*static Shader& bvh = shaderSingleton->getShader("BVH");
	bvh.setVector4f(false, "color", FragColor::BVH);


	std::vector<int> garbage{};
	static Camera& camera = *cameraSingleton->getCamera(0);

	if (app->mode == Mode::OBJECT)
		objectBVHImprovedSingleton->DrawRayInteraction(objectBVHImprovedSingleton->getRoot(), camera.createRay(glfwWindow), camera, bvh);
	else if (app->mode == Mode::EDIT && app->selectMode == SelectMode::VERTEX)
	{

		VertexBVHImprovedSingleton->DrawRayInteraction(VertexBVHImprovedSingleton->getRoot(), camera.createRay(glfwWindow), camera, bvh);

	}*/


}


void MyGUI::gizmos()
{
	if (!app->selectedObjects.size())return;

	Viewport* viewport = getWindow()->getViewportAtCursor();
	if (!viewport)return;
	Camera* camera = viewport->getActiveCamera();

	ImGuizmo::SetOrthographic(false);

	ImGuizmo::AllowAxisFlip(false);

	glm::mat4 viewMatrix = camera->getViewMatrix();
	glm::mat4 projMatrix = camera->getProjectionMatrix();

	transform = objectSingleton->getActiveObject()->getModelMatrix();

	ImGuizmo::SetRect(0, 0, io->DisplaySize.x, io->DisplaySize.y);
	ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix),
		operation, ImGuizmo::MODE::WORLD,
		glm::value_ptr(transform), NULL);

	if (ImGuizmo::IsUsingAny()) {
		if (operation == ImGuizmo::OPERATION::TRANSLATE)
		{
			position[0] = transform[3][0];
			position[1] = transform[3][1];
			position[2] = transform[3][2];

			for (auto x : app->selectedObjects)
				objectSingleton->getObject(x)->translate(glm::vec3(transform[3]) - glm::vec3(previousTransform[3]));
		}

		if (operation == ImGuizmo::OPERATION::ROTATE)
		{
			glm::quat currentRotation = glm::quat_cast(transform);
			glm::quat previousRotation = glm::quat_cast(previousTransform);

			glm::quat deltaRotation = currentRotation * glm::inverse(previousRotation);
			for (auto i : app->selectedObjects)
				objectSingleton->getObject(i)->rotate(deltaRotation);

			glm::vec3 euler = glm::degrees(glm::eulerAngles(currentRotation));
			rotation[0] = euler.x;
			rotation[1] = euler.y;
			rotation[2] = euler.z;







		}

		if (operation == ImGuizmo::OPERATION::SCALE)
		{
			scale[0] = transform[0][0];
			scale[1] = transform[1][1];
			scale[2] = transform[2][2];
			if (scale[0] == 0)scale[0] = 0.00001;
			if (scale[1] == 0)scale[1] = 0.00001;
			if (scale[2] == 0)scale[2] = 0.00001;

			auto delta = transform / previousTransform;
			for (auto x : app->selectedObjects)
				objectSingleton->getObject(x)->scale(
					transform[0][0] / previousTransform[0][0],
					transform[1][1] / previousTransform[1][1],
					transform[2][2] / previousTransform[2][2]);
		}

		objectBVHImprovedSingleton->Refit();

	}
	previousTransform = transform;
}

void MyGUI::vertexTransform()
{

	Mesh* activeObject = dynamic_cast<Mesh*>(objectSingleton->getActiveObject());
	std::vector<DVertex*>& vertices = activeObject->getVertices();
	int numberOfVertices = activeObject->getNumberOfVertices();

	std::vector<int>& selectedVertices = static_cast<Mesh*>(activeObject)->getSelectedVertices();
	if (selectedVertices.size() == 0)return;

	position[0] = vertices[selectedVertices.back()]->position.x;
	position[1] = vertices[selectedVertices.back()]->position.y;
	position[2] = vertices[selectedVertices.back()]->position.z;

	ImGui::InputFloat3("DVertex position", position);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (int i = 0; i < selectedVertices.size(); i++)
		{
			vertices[selectedVertices[i]]->setPosition(position[0], position[1], position[2]);
			activeObject->updateVertexBuffer(selectedVertices[i]);
		}
		VertexBVHImprovedSingleton->Refit(activeObject);
	}
}

void MyGUI::transformations()
{
	if (!app->selectedObjects.size()) return;

	Object* activeObject = objectSingleton->getActiveObject();

	ImGui::InputFloat3("Position", position);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{

		activeObject->setPosition(position[0], position[1], position[2]);


		objectBVHImprovedSingleton->Refit();
	}

	ImGui::InputFloat3("Rotation", rotation);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{

		activeObject->setRotation(rotation[0], rotation[1], rotation[2]);

		objectBVHImprovedSingleton->Refit();


	}

	ImGui::InputFloat3("Scale", scale);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		activeObject->setScale(scale[0], scale[1], scale[2]);


		objectBVHImprovedSingleton->Refit();

	}
}

void MyGUI::selectObject()
{
	if (!app->selectedObjects.size())return;

	int selectedObjectIndex = app->selectedObjects.back();

	const glm::vec3& positionVec = objectSingleton->getObject(selectedObjectIndex)->getPosition();
	const glm::vec3& rotationVec = objectSingleton->getObject(selectedObjectIndex)->getRotationVec();
	const glm::vec3& scaleVec = objectSingleton->getObject(selectedObjectIndex)->getScale();

	position[0] = positionVec.x;
	position[1] = positionVec.y;
	position[2] = positionVec.z;

	scale[0] = scaleVec.x;
	scale[1] = scaleVec.y;
	scale[2] = scaleVec.z;

	rotation[0] = rotationVec.x;
	rotation[1] = rotationVec.y;
	rotation[2] = rotationVec.z;


}

void MyGUI::setGizmoOperation(ImGuizmo::OPERATION op) { operation = op; }

void MyGUI::initializeGrid3D(int width)
{
	int z = 0;

	for (int i = 0; i <= width; i++)
	{
		gridVertices3D.push_back(glm::vec2(-width * 0.5, width * 0.5 - i));
		gridVertices3D.push_back(glm::vec2(width * 0.5, width * 0.5 - i));

		gridIndices3D.push_back(z++);
		gridIndices3D.push_back(z++);
	}
	for (int i = 1; i < width; i++)
	{
		gridVertices3D.push_back(glm::vec2(-width * 0.5 + i, width * 0.5));
		gridVertices3D.push_back(glm::vec2(-width * 0.5 + i, -width * 0.5));
		gridIndices3D.push_back(z++);
		gridIndices3D.push_back(z++);
	}

	gridIndices3D.push_back(0);
	gridIndices3D.push_back(width * 2);

	gridIndices3D.push_back(1);
	gridIndices3D.push_back(width * 2 + 1);

	grid3DVAO.bind();
	VBO VBO(gridVertices3D);
	grid3DEBO.bufferData(gridIndices3D);

	grid3DVAO.linkAttribute(VBO, 0, 2, GL_FLOAT, sizeof(glm::vec2), (void*)0);

	grid3DVAO.unbind();
	VBO.unbind();
	grid3DEBO.unbind();


}
void MyGUI::initializeGrid2D(int width)
{

	// Horizontal lines 
	for (int i = 0; i <= width; i++)
	{
		float y = float(i) / width;
		gridVertices2D.push_back(glm::vec2(0.0f, y));
		gridVertices2D.push_back(glm::vec2(1.0f, y));
		gridIndices2D.push_back(i * 2);
		gridIndices2D.push_back(i * 2 + 1);
	}

	// Vertical lines 
	for (int i = 1; i < width; i++)
	{
		float x = float(i) / width;
		int baseIndex = (width + 1) * 2 + (i - 1) * 2;

		gridVertices2D.push_back(glm::vec2(x, 0.0f));
		gridVertices2D.push_back(glm::vec2(x, 1.0f));
		gridIndices2D.push_back(baseIndex);
		gridIndices2D.push_back(baseIndex + 1);
	}

	gridIndices2D.push_back(0);
	gridIndices2D.push_back(2 * width);

	gridIndices2D.push_back(1);
	gridIndices2D.push_back(2 * width + 1);

	grid2DVAO.bind();
	VBO VBO(gridVertices2D);
	grid2DEBO.bufferData(gridIndices2D);

	grid2DVAO.linkAttribute(VBO, 0, 2, GL_FLOAT, sizeof(glm::vec2), (void*)0);

	grid2DVAO.unbind();
	VBO.unbind();
	grid2DEBO.unbind();


}
void MyGUI::drawGrid3D(Viewport* viewport)
{
	static auto& shader = shaderSingleton->getShader("Grid");
	shader.activate();

	grid3DVAO.bind();
	grid3DEBO.bind();

	shader.setBool(true, "DDD", true);



	viewport->getActiveCamera()->cameraUniform(true, shader, "cameraMatrix");


	glDrawElements(GL_LINES, gridIndices3D.size(), GL_UNSIGNED_INT, 0);
}
void MyGUI::drawGrid2D(Viewport* viewport)
{
	static auto& shader = shaderSingleton->getShader("Grid");
	shader.activate();

	grid2DVAO.bind();
	grid2DEBO.bind();

	shader.setBool(true, "DDD", false);


	viewport->getActiveCamera()->cameraUniform(true, shader, "cameraMatrix");


	glDrawElements(GL_LINES, gridIndices2D.size(), GL_UNSIGNED_INT, 0);
}

void MyGUI::modes(Viewport* viewport)
{
	static const char* modes[] = { "Object mode","Edit mode","Sculpt mode","Weight paint","Texture paint ","UV Editor","Shader Editor" };

	Mode& mode = viewport->getMode();

	if (ImGui::Button(modes[int(mode)]))
		ImGui::OpenPopup("Modes");

	if (ImGui::BeginPopup("Modes"))
	{
		ImGui::SeparatorText("Mode");
		for (int i = 0; i < IM_ARRAYSIZE(modes); i++)
			if (i == 4)
				ImGui::Separator();
			else
				if (ImGui::Selectable(modes[i]))
				{
					Mode prev = mode;

					mode = Mode(i);

					if (mode == Mode::OBJECT && prev == Mode::EDIT)
					{
						objectBVHImprovedSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
						std::cout << "\n\n OBJECT BVH BUILT ...";

					}
					else if (mode == Mode::EDIT && prev == Mode::OBJECT)
					{
						Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getActiveObject());
						double time = glfwGetTime();

						VertexBVHImprovedSingleton->BuildBottomUp(mesh);
						EdgeBVHImprovedSingleton->BuildBottomUp(*mesh);
						FaceBVHImprovedSingleton->BuildBottomUp(*mesh);

						std::cout << "All 3 BVHs built in " << glfwGetTime() - time << " seconds";
					}

					if (mode == Mode::UV_EDIT)
						viewport->setActiveCamera(CameraTypes::UV);
					else if (mode == Mode::SHADER_EDIT)
						viewport->setActiveCamera(CameraTypes::SHADER);
					else
						viewport->setActiveCamera(CameraTypes::VIEWPORT);
				}
		ImGui::EndPopup();
	}


}

void MyGUI::dMesh()
{

	ImGui::Begin("DMesh");

	Mesh* mesh = dynamic_cast<Mesh*> (objectSingleton->getObject(app->selectedObjects.back()));

	if (!mesh)return;

	const std::vector<DVertex*>& vertices = mesh->getVertices();
	std::vector<int>& selectedVertices = mesh->getSelectedVertices();
	std::vector<DEdge*>& selectedEdges = mesh->getSelectedEdges();
	std::vector<DFace*>& selectedFaces = mesh->getSelectedFaces();

	static int clicked = 0;


	if (ImGui::Button("Vertex.edge"))
		clicked++;
	if (clicked)
	{
		clicked = 0;


		selectedVertices = { selectedVertices.back() };

		selectedEdges.clear();
		if (!vertices[selectedVertices.back()]->e)// if it doesnt exist
		{
			std::cerr << "\n\nvertex.edge does not exist\n";
			ImGui::End();
			return;
		}
		selectedEdges.push_back(vertices[selectedVertices.back()]->e);

		vertexEdge = selectedEdges[0];

		std::pair<int, int> edgeIndices = mesh->getEdgeIndices(selectedEdges.back());
		selectedVertices.push_back((edgeIndices.first == selectedVertices[0]) ? edgeIndices.second : edgeIndices.first);

	}
	ImGui::SameLine();
	if (ImGui::Button("Edge.v1"))
		clicked++;
	if (clicked)
	{
		clicked = 0;
		if (selectedEdges.empty())
		{
			std::cerr << "\n\n No edge selected\n";
			ImGui::End();
			return;
		}
		selectedVertices.clear();
		selectedVertices.push_back(mesh->getVertexIndex(selectedEdges[0]->v1));

		std::cout << "\n\tV1 = " << selectedVertices.back();
	}
	ImGui::SameLine();
	if (ImGui::Button("Edge.v2"))
		clicked++;
	if (clicked)
	{
		clicked = 0;
		if (selectedEdges.empty())
		{
			std::cerr << "\n\n No edge selected\n";
			ImGui::End();
			return;
		}
		selectedVertices.clear();
		selectedVertices.push_back(mesh->getVertexIndex(selectedEdges[0]->v2));

		std::cout << "\n\tV2 = " << selectedVertices.back();
	}
	if (ImGui::Button("Edge.DiskLink.d1.next"))
		clicked++;
	if (clicked)
	{
		clicked = 0;

		DVertex* v1 = selectedEdges.back()->v1;

		if (!selectedEdges[0]->d1.next)// if it doesnt exist
		{
			std::cerr << "\n\n diskLink.d1.next does not exist\n";
			ImGui::End();
			return;
		}

		selectedEdges[0] = selectedEdges[0]->d1.next;

		std::pair<int, int> edgeIndices = mesh->getEdgeIndices(selectedEdges.back());
		selectedVertices.clear();

		std::cout << "\nv1\t" << v1 << "\ne1\t" << &vertices[edgeIndices.first] << "\ne2\t" << &vertices[edgeIndices.second] << "\nv1 index: " << mesh->getVertexIndex(v1);


		if (v1 == vertices[edgeIndices.first])
		{
			selectedVertices.push_back(edgeIndices.second);
			selectedVertices.push_back(edgeIndices.first);

		}
		else if (v1 == vertices[edgeIndices.second])
		{
			selectedVertices.push_back(edgeIndices.first);
			selectedVertices.push_back(edgeIndices.second);

		}
		else std::cout << "\nKonju123  v1 = " << edgeIndices.first << "\tv2 = " << edgeIndices.second;

	}
	ImGui::SameLine();
	if (ImGui::Button("Edge.DiskLink.d1.prev"))
		clicked++;
	if (clicked)
	{
		clicked = 0;

		if (!selectedEdges[0]->d1.prev)// if it doesnt exist
		{
			std::cerr << "\n\n diskLink.d1.prev does not exist\n";
			ImGui::End();
			return;
		}

		selectedEdges[0] = selectedEdges[0]->d1.prev;

		std::pair<int, int> edgeIndices = mesh->getEdgeIndices(selectedEdges.back());
		selectedVertices.clear();
		selectedVertices.push_back(edgeIndices.first);
		selectedVertices.push_back(edgeIndices.second);

	}
	if (ImGui::Button("Edge.DiskLink.d2.next"))
		clicked++;
	if (clicked)
	{
		clicked = 0;

		selectedEdges[0] = selectedEdges[0]->d2.next;

		std::pair<int, int> edgeIndices = mesh->getEdgeIndices(selectedEdges.back());
		selectedVertices.clear();
		selectedVertices.push_back(edgeIndices.first);
		selectedVertices.push_back(edgeIndices.second);

	}
	ImGui::SameLine();
	if (ImGui::Button("Edge.DiskLink.d2.prev"))
		clicked++;
	if (clicked)
	{
		clicked = 0;

		selectedEdges[0] = selectedEdges[0]->d2.prev;

		std::pair<int, int> edgeIndices = mesh->getEdgeIndices(selectedEdges.back());
		selectedVertices.clear();
		selectedVertices.push_back(edgeIndices.first);
		selectedVertices.push_back(edgeIndices.second);

	}
	if (ImGui::Button("Edge.loop"))
		clicked++;
	if (clicked)
	{
		clicked = 0;
		if (!selectedEdges[0]->loop)// if it doesnt exist
		{
			std::cerr << "\n\n Edge.loop does not exist\n";
			ImGui::End();
			return;
		}
		selectedLoop = selectedEdges[0]->loop;
		selectedEdges[0] = selectedLoop->edge;

		std::pair<int, int> edgeIndices = mesh->getEdgeIndices(selectedEdges.back());
		selectedVertices.clear();
		selectedVertices.push_back(edgeIndices.first);
		selectedVertices.push_back(edgeIndices.second);

	}
	ImGui::SameLine();
	if (ImGui::Button("Loop.edge"))
		clicked++;
	if (clicked)
	{
		clicked = 0;
		if (!selectedLoop->edge)// if it doesnt exist
		{
			std::cerr << "\n\n loop.edge does not exist\n";
			ImGui::End();
			return;
		}
		selectedEdges[0] = selectedLoop->edge;

		std::pair<int, int> edgeIndices = mesh->getEdgeIndices(selectedEdges.back());
		selectedVertices.clear();
		//selectedVertices.push_back(edgeIndices.first);
		//selectedVertices.push_back(edgeIndices.second);

	}
	ImGui::SameLine();
	if (ImGui::Button("Loop.tip"))
		clicked++;
	if (clicked)
	{
		clicked = 0;
		if (!selectedLoop)// if it doesnt exist
		{
			std::cerr << "\n\n loop.edge not selected\n";
			ImGui::End();
			return;
		}

		selectedVertices.clear();
		selectedVertices.push_back(mesh->getVertexIndex(selectedLoop->tip));
		//selectedVertices.push_back(edgeIndices.first);
		//selectedVertices.push_back(edgeIndices.second);

	}
	if (ImGui::Button("Loop.next"))
		clicked++;
	if (clicked)
	{
		clicked = 0;
		if (!selectedLoop->next)// if it doesnt exist
		{
			std::cerr << "\n\n loop.next does not exist\n";
			ImGui::End();
			return;
		}
		selectedLoop = selectedLoop->next;
		selectedEdges[0] = selectedLoop->edge;

		std::pair<int, int> edgeIndices = mesh->getEdgeIndices(selectedEdges.back());
		selectedVertices.clear();
		selectedVertices.push_back(edgeIndices.first);
		selectedVertices.push_back(edgeIndices.second);

	}
	ImGui::SameLine();
	if (ImGui::Button("Loop.prev"))
		clicked++;
	if (clicked)
	{
		clicked = 0;
		if (!selectedLoop->prev)// if it doesnt exist
		{
			std::cerr << "\n\n loop.prev does not exist\n";
			ImGui::End();
			return;
		}
		selectedLoop = selectedLoop->prev;
		selectedEdges[0] = selectedLoop->edge;

		std::pair<int, int> edgeIndices = mesh->getEdgeIndices(selectedEdges.back());
		selectedVertices.clear();
		selectedVertices.push_back(edgeIndices.first);
		selectedVertices.push_back(edgeIndices.second);

	}
	if (ImGui::Button("Loop.radialNext"))
		clicked++;
	if (clicked)
	{
		clicked = 0;
		if (selectedLoop->radialNext == selectedLoop)// if it doesnt exist
		{
			std::cerr << "\n\n loop.radialNext ---> only edge loop\n";
			ImGui::End();
			return;
		}
		selectedLoop = selectedLoop->radialNext;
		selectedEdges[0] = selectedLoop->edge;

		std::pair<int, int> edgeIndices = mesh->getEdgeIndices(selectedEdges.back());
		selectedVertices.clear();
		selectedVertices.push_back(edgeIndices.first);
		selectedVertices.push_back(edgeIndices.second);

	}
	ImGui::SameLine();
	if (ImGui::Button("Loop.radialPrev"))
		clicked++;
	if (clicked)
	{
		clicked = 0;
		if (selectedLoop->radialPrev == selectedLoop)// if it doesnt exist
		{
			std::cerr << "\n\n loop.radialPrev ---> only edge loop\n";
			ImGui::End();
			return;
		}
		selectedLoop = selectedLoop->radialPrev;
		selectedEdges[0] = selectedLoop->edge;

		std::pair<int, int> edgeIndices = mesh->getEdgeIndices(selectedEdges.back());
		selectedVertices.clear();
		selectedVertices.push_back(edgeIndices.first);
		selectedVertices.push_back(edgeIndices.second);

	}

	if (ImGui::Button("Vertex.adjecentVertices"))
		clicked++;
	if (clicked)
	{
		clicked = 0;

		selectedVertices = { selectedVertices.back() };
		std::unordered_set<DVertex*>set = vertices[selectedVertices[0]]->getAdjecentVertices();

		for (DVertex* v : set)
			selectedVertices.push_back(mesh->getVertexIndex(v));

		for (auto x : selectedVertices)
			std::cout << " " << x;
		std::cout << "\n";

	}
	ImGui::SameLine();
	if (ImGui::Button("Vertex.adjecentEdges"))
		clicked++;
	if (clicked)
	{
		clicked = 0;

		selectedEdges.clear();
		std::unordered_set<DEdge*>set = vertices[selectedVertices[0]]->getAdjecentEdges();

		for (DEdge* e : set)
		{
			selectedEdges.push_back(e);
			auto temp = mesh->getEdgeIndices(e);
			std::cout << "\n " << temp.first << " " << temp.second;
		}

	}

	ImGui::SameLine();
	if (ImGui::Button("Vertex.adjecentFaces"))
		clicked++;
	if (clicked)
	{
		clicked = 0;

		selectedFaces.clear();
		std::unordered_set<DFace*>set = vertices[selectedVertices[0]]->getAdjecentFaces();

		for (DFace* f : set)
		{
			selectedFaces.push_back(f);
			auto temp = mesh->getFaceIndices(f);

			std::cout << "\n";
			for (auto x : temp)
				std::cout << " " << x;
			std::cout << "\n";
		}


		std::cout << "\n\n\t Selected faces:\t" << selectedFaces.size();

	}

	ImGui::End();

}



void MyGUI::shaderNodeEditor()
{
	// ovo negdje drugo prebaciti
	ImNodesStyle& style = ImNodes::GetStyle();
	style.Colors[ImNodesCol_TitleBarSelected] = IM_COL32(200, 200, 0, 255);

	ImGui::Begin("Shader Node Editor ");

	ImNodes::BeginNodeEditor();

	app->activeMaterial->drawNodes();

	auto& links = app->activeMaterial->getLinks();
	for (int i = 0; i < links.size(); ++i)
	{
		const std::pair<int, int> p = links[i];
		// in this case, we just use the array index of the link
		// as the unique identifier
		ImNodes::Link(i, p.first, p.second);
	}

	ImNodes::MiniMap(0.25f, ImNodesMiniMapLocation_BottomRight);

	ImNodes::EndNodeEditor();



	/////////////////////////////////////////////////////////////////////
		// link creation logic \\
	/////////////////////////////////////////////////////////////////////
		// types of viable links are noted in ShadingNodes.h \\
	////////////////////////////////////////////////////////////////////
	int outputAttribute, inputAttribute;
	if (ImNodes::IsLinkCreated(&outputAttribute, &inputAttribute))
		if (ImNodes::GetAttributePinShape(outputAttribute) == ImNodes::GetAttributePinShape(inputAttribute) ||
			ImNodes::GetAttributePinShape(inputAttribute) == ImNodesPinShape_Triangle)
			links.push_back(std::make_pair(outputAttribute, inputAttribute));





	// selection logic \\

	int node_id;
	const int num_selected_nodes = ImNodes::NumSelectedNodes();
	auto& selectedNodes = app->activeMaterial->getSelectedNodes();
	if (num_selected_nodes > 0)
	{
		selectedNodes.resize(num_selected_nodes);
		ImNodes::GetSelectedNodes(selectedNodes.data());
	}
	else selectedNodes.clear();

	int num_selected_links = ImNodes::NumSelectedLinks();
	auto& selectedLinks = app->activeMaterial->getSelectedLinks();
	if (num_selected_links > 0)
	{
		selectedLinks.resize(num_selected_links);
		ImNodes::GetSelectedLinks(selectedLinks.data());
	}
	else selectedLinks.clear();

	//std::cout << "\n Selected links: "<<selectedLinks.size();

	ImGui::End();
}

void MyGUI::addShadingNodes()
{

	hoverTime = glfwGetTime();

	ImGui::OpenPopup("Add popup");

	if (ImGui::BeginPopup("Add popup"))
	{
		ImGui::SeparatorText("Add");
		ImGui::Separator();
		ImGui::InputText("WIP", searchText, IM_ARRAYSIZE(searchText));
		ImGui::Separator();

		// Color Nodes
		if (ImGui::BeginMenu("Color"))
		{
			if (ImGui::MenuItem("Color"))
			{
				app->activeMaterial->createNode<ColorNode>();
				std::cout << "Color node added\n";
			}
			if (ImGui::MenuItem("ColorMix"))
			{
				app->activeMaterial->createNode<ColorMixNode>();
				std::cout << "ColorMix node added\n";
			}

			ImGui::EndMenu();
		}

		// Math Nodes
		if (ImGui::BeginMenu("Math"))
		{
			if (ImGui::MenuItem("Math"))
			{
				app->activeMaterial->createNode<MathNode>();
				std::cout << "Math node added\n";
			}
			ImGui::EndMenu();
		}

		// Texture Nodes
		if (ImGui::BeginMenu("Texture"))
		{
			if (ImGui::MenuItem("Texture"))
			{
				app->activeMaterial->createNode<TextureNode>();
				std::cout << "Texture node added\n";
			}
			ImGui::EndMenu();
		}

		// Input Nodes
		if (ImGui::BeginMenu("Input"))
		{
			if (ImGui::MenuItem("Value"))
			{
				app->activeMaterial->createNode<ValueNode>();
				std::cout << "Value node added\n";
			}
			ImGui::EndMenu();
		}

		// Output Nodes
		if (ImGui::BeginMenu("Output"))
		{
			if (ImGui::MenuItem("Color Output"))
			{
				app->activeMaterial->createNode<ColorOutputNode>();
				std::cout << "Color Output node added\n";
			}
			if (ImGui::MenuItem("Normal Output"))
			{
				app->activeMaterial->createNode<NormalOutputNode>();
				std::cout << "Normal Output node added\n";
			}
			if (ImGui::MenuItem("Roughness Output"))
			{
				app->activeMaterial->createNode<RoughnessOutputNode>();
				std::cout << "Roughness Output node added\n";
			}
			if (ImGui::MenuItem("Metallic Output"))
			{
				app->activeMaterial->createNode<MetallicOutputNode>();
				std::cout << "Metallic Output node added\n";
			}
			if (ImGui::MenuItem("Ambient Occlusion Output"))
			{
				app->activeMaterial->createNode<AmbientOcclusionOutputNode>();
				std::cout << "Ambient Occlusion Output node added\n";
			}

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	// auto-close logic
	if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
		&& !ImGui::IsAnyItemHovered()
		&& hoverTime > 1.4)
	{
		glfwSetTime(0);
		hoverTime = 0;

		getWindow()->getViewportAtCursor()->getKeys()[GLFW_KEY_A] = 0;

		showAddMenuFlag = false;
		ImGui::CloseCurrentPopup();
	}



}








void MyGUI::pbrRender(const char* filename, int width, int height)

{

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // your background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < objectSingleton->getNumberOfObjects(); i++)
	{
		Object* object = objectSingleton->getObject(i);
		if (!dynamic_cast<Mesh*>(object))continue;
		Mesh* mesh = dynamic_cast<Mesh*>(object);


		mesh->renderDraw(*getWindow()->getViewportAtCursor()->getActiveCamera());
	}

	glFinish();


	glPixelStorei(GL_PACK_ALIGNMENT, 1);


	std::vector<unsigned char> pixels(width * height * 3);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

	// Flip vertically (OpenGL origin is bottom-left, images expect top-left)
	std::vector<unsigned char> flipped(width * height * 3);
	for (int y = 0; y < height; ++y)
	{
		memcpy(&flipped[y * width * 3],
			&pixels[(height - 1 - y) * width * 3],
			width * 3);
	}

	stbi_write_png(filename, width, height, 3, flipped.data(), width * 3);


	FileSystem::openFile(filename);

}

int flatten(RaytracingBVHNode* node, std::vector<flatRTNode>& out, std::vector<Triangle>& trinagles)
{
	//if (!node) return -1;

	//int index = out.size();
	//out.emplace_back(); // placeholder

	//int leftIndex = flatten(node->left, out, trinagles);
	//int rightIndex = flatten(node->right, out, trinagles);

	//flatRTNode f;
	//f.aabbMin = node->box.min;
	//f.aabbMax = node->box.max;
	//f.left = leftIndex;
	//f.right = rightIndex;

	//if (node->left == nullptr && node->right == nullptr) {
	//	// leaf
	//	f.triIndex = trinagles.size();
	//	trinagles.push_back(node->tri);
	//	/*std::cout << node->tri.v0.x << " " << node->tri.v0.y << " " << node->tri.v0.z << "\n";
	//	std::cout << node->tri.v1.x << " " << node->tri.v1.y << " " << node->tri.v1.z << "\n";
	//	std::cout << node->tri.v2.x << " " << node->tri.v2.y << " " << node->tri.v2.z << "\n\n";*/
	//}
	//else {
	//	f.triIndex = -1;
	//}

	//out[index] = f;
	return 0;
}

void sendData(Shader& shader)
{





}

void MyGUI::raytraceRender(const char* filename, int width, int height)
{


	auto t0 = std::chrono::high_resolution_clock::now();



	Shader& computeShader = shaderSingleton->getShader("ComputeShader");
	Shader& computeOutput = shaderSingleton->getShader("ComputeOutput");

	// ovo mene jebe druze sudija

	//GLuint texture;
	//glGenTextures(1, &texture);
	//glBindTexture(GL_TEXTURE_2D, texture);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	//glBindImageTexture(0, RaytracingBVHSingleton->getOutputTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	RaytracingBVHSingleton->activate();

	computeShader.activate();



	static Texture tex("planks.png", 1);
	tex.bind();

	tex.textureUniform(computeShader, "tex", 1);


	/*std::vector<std::string> skybox
	{
		"px1.png",
		"nx1.png",
		"py1.png",
		"ny1.png",
		"pz1.png",
		"nz1.png"
	};*/

	//static unsigned int textureID;
	//glGenTextures(1, &textureID);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	//std::string projectDir = getProjectDirr();



	//int w, h, nrChannels;
	//for (unsigned int i = 0; i < 6; i++)
	//{
	//	std::string imagePath = "C:\\Users\\adnan\\Desktop\\Dardaneli\\assets\\" + skybox[i];
	//	stbi_set_flip_vertically_on_load(false);
	//	unsigned char* data = stbi_load(imagePath.c_str(), &w, &h, &nrChannels, 0);
	//	if (data)
	//	{
	//		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
	//			0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data
	//		);
	//		stbi_image_free(data);
	//	}
	//	else
	//	{
	//		std::cout << "Cubemap tex failed to load at path: " << skybox[i] << std::endl;
	//		stbi_image_free(data);
	//	}
	//}
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	/*glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);*/
	glUniform1i(glGetUniformLocation(computeShader.getID(), "skybox"), 2);



	//sendData(computeShader);


	//RaytracingBVHSingleton->Build();





	//std::cout << "\nGPU BVH nodes: " << gpuNodes.size() << "\n";
	//std::cout << "Triangles size: " << triangleData.size() << "\n";
	//std::cout << "TriangleMaterialData size: " << triangleMaterialData.size() << "\n\n"; 

	//for (auto x : triangleMaterialData)
	//{
	//	std::cout << " Normal v1	" << x.n1x << " " << x.n1y << " " << x.n1z << "\n";
	//	std::cout << " Normal v2	" << x.n2x << " " << x.n2y << " " << x.n2z << "\n";
	//	std::cout << " Normal v3	" << x.n3x << " " << x.n3y << " " << x.n3z << "\n\n";
	//}


	// ove buffere jos prebaciti da se ne kreiraju svaki put iznova.. i ne mora se ja msm svaki put slati data
	//GLuint bvhBuffer, triBuffer, matBuffer;

	//glGenBuffers(1, &bvhBuffer);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhBuffer);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, gpuNodes.size() * sizeof(flatRTNode), gpuNodes.data(), GL_STATIC_DRAW);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bvhBuffer);

	//glGenBuffers(1, &triBuffer);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, triBuffer);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, triangleData.size() * sizeof(Triangle), triangleData.data(), GL_STATIC_DRAW);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, triBuffer);


	//glGenBuffers(1, &matBuffer);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, matBuffer);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, triangleMaterialData.size() * sizeof(TriangleMaterial), triangleMaterialData.data(), GL_STATIC_DRAW);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, matBuffer);



	// cameraSingleton sam izbacio... zato je comment outano

	//computeShader.setVector3f(true, "camPos", cameraSingleton->getCamera(0)->getPosition());
	//computeShader.setVector3f(true, "camDir", cameraSingleton->getCamera(0)->getOrientation());
	//computeShader.setVector3f(true, "camUp", cameraSingleton->getCamera(0)->getUp());
	//computeShader.setVector3f(true, "camRight", glm::normalize(glm::cross(cameraSingleton->getCamera(0)->getOrientation(), cameraSingleton->getCamera(0)->getUp())));
	//computeShader.setFloat(true, "fov", glm::radians(cameraSingleton->getCamera(0)->getFOV()));
	//computeShader.setVector2i(true, "resolution", glm::vec2(cameraSingleton->getCamera(0)->getWidth(), cameraSingleton->getCamera(0)->getHeight()));



	glDispatchCompute(width, height, 1);


	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);


	computeOutput.activate();

	//GLuint quadVAO;

	//glCreateVertexArrays(1, &quadVAO);
	//glBindVertexArray(quadVAO);

	//glBindTextureUnit(0, RaytracingBVHSingleton->getOutputTexture());
	RaytracingBVHSingleton->draw();

	//glDrawArrays(GL_TRIANGLES, 0, 3);



	//glDeleteTextures(1, &texture);
	//glDeleteVertexArrays(1, &quadVAO);
	//glDeleteTextures(1, &textureID);

	//RaytracingBVHSingleton->destroy();

	//glDeleteBuffers(1, &bvhBuffer);
	//glDeleteBuffers(1, &triBuffer);
	//glDeleteBuffers(1, &matBuffer);

	glFinish();

	auto t1 = std::chrono::high_resolution_clock::now();

	//std::cout << "\n Raytrace render time: " << std::chrono::duration<double, std::milli>(t1 - t0).count() << " ms\n";
}

void MyGUI::importObject()
{

	std::string dlgId = "ChooseObj"; // unique per node


	IGFD::FileDialogConfig cfg;
	cfg.path = getDownloadsPath();
	ImGuiFileDialog::Instance()->OpenDialog(
		dlgId.c_str(),
		"Select OBJ File",
		"OBJ Files (*.obj){.obj}",
		cfg
	);


	// File dialog update
	if (ImGuiFileDialog::Instance()->Display(dlgId.c_str()))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
			new Mesh(ImGuiFileDialog::Instance()->GetFilePathName().c_str());

		ImGuiFileDialog::Instance()->Close();

		showImportDialog = false;
	}
}








