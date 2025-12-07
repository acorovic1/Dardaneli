#include "MyGUI.h"

#include <iomanip>
#include <chrono>
#include <unordered_set>

#include "glad/glad.h"
#include "stb/stb_image_write.h"

#include "Window.h"
#include "Mesh/DFace.h"
#include "Mesh/DLoop.h"
#include "MaterialManager.h"
#include "RaytracingBVH.h"

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

void openFile(const char* filename) {
#if defined(_WIN32)
	ShellExecuteA(NULL, "open", filename, NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
	std::string cmd = "open ";
	cmd += filename;
	system(cmd.c_str());
#elif defined(__linux__)
	std::string cmd = "xdg-open ";
	cmd += filename;
	system(cmd.c_str());
#endif
}





MyGUI::MyGUI(GLFWwindow* glfwWindow) :io(nullptr), gizmoIo(nullptr), glfwWindow(glfwWindow) { MyGUI::init(); }

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



void MyGUI::drawUI()
{
	ImGui::Begin("Dardaneli - ImGUI");

	Mode pastMode = app->mode;
	modes();
	ImGui::SameLine();
	Mode currentMode = app->mode;



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



	int current = static_cast<int>(app->renderMode);
	ImGui::SameLine();
	if (ImGui::RadioButton("Wireframe", &current, 0)) app->renderMode = RenderMode::WIREFRAME; ImGui::SameLine();
	if (ImGui::RadioButton("Solid", &current, 1)) app->renderMode = RenderMode::SOLID; ImGui::SameLine();
	if (ImGui::RadioButton("Material Preview", &current, 2)) app->renderMode = RenderMode::MATERIAL_PREVIEW; ImGui::SameLine();
	if (ImGui::RadioButton("Render", &current, 3)) app->renderMode = RenderMode::RENDER; ImGui::SameLine();


	ImGui::End();


	ImGui::Begin("Tools");

	if (currentMode == Mode::OBJECT)drawObjectModeUI(pastMode != currentMode);
	else if (currentMode == Mode::EDIT)drawEditModeUI(pastMode != currentMode);
	else if (currentMode == Mode::UV_EDIT)drawUVModeUI(pastMode != currentMode);
	else if (currentMode == Mode::SHADER_EDIT)drawShaderEditorUI(pastMode != currentMode);

	ImGui::End();

	if (app->mode == Mode::EDIT)
		dMesh();






}

void MyGUI::drawObjectModeUI(bool change)
{

	ImGui::Checkbox("BVHTree", &BVHTree);
	ImGui::SameLine();
	ImGui::Checkbox("FaceCulling", &faceCulling);
	ImGui::InputInt("BVHTreeSubdivision", &BVHSubd);
	ImGui::Checkbox("Gizmo", &gizmo);

	if (gizmo)
		gizmos();

	if (showAddMenuFlag)
		addMenu();

	if (BVHTree)
		drawBVH();

	if (app->objectIndices.size())
	{
		int& lastIndex = app->objectIndices.back();
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

void MyGUI::drawEditModeUI(bool change)
{
	Mesh* mesh = dynamic_cast<Mesh*>(app->getActiveObject());
	if (change)
	{
		double time = glfwGetTime();

		VertexBVHSingleton->BuildBottomUp(*mesh);
		EdgeBVHSingleton->BuildBottomUp(*mesh);
		FaceBVHSingleton->BuildBottomUp(*mesh);

		std::cout << "All 3 BVHs built in " << glfwGetTime() - time << " seconds";

	}

	if (BVHTree)
	{
		static Shader& basic = shaderSingleton->getShader("Basic");
		basic.setInteger(false, "colorMode", static_cast<int>(FragColor::BVH));
		if (app->selectMode == SelectMode::VERTEX)
			VertexBVHSingleton->DrawLeaves(VertexBVHSingleton->getRoot(), *cameraSingleton->getCamera(0), basic);
		else if (app->selectMode == SelectMode::EDGE)
			EdgeBVHSingleton->DrawLeaves(EdgeBVHSingleton->getRoot(), *cameraSingleton->getCamera(0), basic);
		else if (app->selectMode == SelectMode::FACE)
			FaceBVHSingleton->DrawLeaves(FaceBVHSingleton->getRoot(), *cameraSingleton->getCamera(0), basic);

	}

	if (showDeleteMenuFlag)
		deleteMenu();

	if (showExtrudeMenuFlag)
		extrudeMenu();


	ImGui::Checkbox("BVHTree", &BVHTree);
	ImGui::SameLine();
	ImGui::InputInt("BVHTreeSubdivision", &eBVHSubd);
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

	int e = (int)app->selectMode;

	ImGui::RadioButton("DVertex select", &e, 0); ImGui::SameLine();
	ImGui::RadioButton("Edge select", &e, 1); ImGui::SameLine();
	ImGui::RadioButton("DFace select", &e, 2);

	if (e == 0)app->selectMode = SelectMode::VERTEX;
	else if (e == 1)app->selectMode = SelectMode::EDGE;
	else if (e == 2)app->selectMode = SelectMode::FACE;


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



}

void MyGUI::drawUVModeUI(bool change)
{
	ImGui::Checkbox("BVHTree", &BVHTree);

	Mesh* mesh = static_cast<Mesh*>(objectSingleton->getObject(app->objectIndices.back()));
	UVVertexBVHSingleton->BuildBottomUp(*mesh); // prebaci ovo na unwrap funkciju
	static Shader basic = shaderSingleton->getShader("Basic");

	if (BVHTree)
	{
		basic.setInteger(false, "colorMode", static_cast<int>(FragColor::BVH));
		UVVertexBVHSingleton->DrawLeaves(UVVertexBVHSingleton->getRoot(), *cameraSingleton->getCamera("UV"), basic);
	}

}

void MyGUI::drawShaderEditorUI(bool change)
{
	if (showAddMenuFlag) addShadingNodes();


	Mesh* mesh = dynamic_cast<Mesh*>(app->getActiveObject());


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
void MyGUI::showInsetMenu() { showInsetMenuFlag = true; }

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

			getWindow()->getKeys()[GLFW_KEY_Q] = 0;
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

		Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getObject(app->objectIndices.back()));
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

			getWindow()->getKeys()[GLFW_KEY_X] = 0;
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

		Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getObject(app->objectIndices.back()));
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

		selected_option = -1;

		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && hoverTime > 1.4)
		{
			glfwSetTime(0);
			hoverTime = 0;

			getWindow()->getKeys()[GLFW_KEY_E] = 0;
			showExtrudeMenuFlag = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}


}


void MyGUI::drawBVH() {
	static Shader& basic = shaderSingleton->getShader("Basic");
	basic.setInteger(false, "colorMode", static_cast<int>(FragColor::UV));
	objectBVHSingleton->Draw(*cameraSingleton->getCamera(0), basic, BVHSubd);
	basic.setInteger(false, "colorMode", static_cast<int>(FragColor::Seams));
}


void MyGUI::gizmos()
{
	Camera* camera = getWindow()->getCamera();

	ImGuizmo::SetOrthographic(false);

	ImGuizmo::AllowAxisFlip(false);

	glm::mat4 viewMatrix = camera->getViewMatrix();
	glm::mat4 projMatrix = camera->getProjectionMatrix();

	static glm::mat4 transform = glm::mat4(1.0f);
	if (app->objectIndices.size())
		if (app->objectIndices[app->objectIndices.size() - 1] != -1 && app->objectIndices[app->objectIndices.size() - 1] < objectSingleton->getNumberOfObjects())
			transform = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1])->getModel();

	static glm::mat4 previousTransform = glm::mat4(1.0f);

	ImGuizmo::SetRect(0, 0, io->DisplaySize.x, io->DisplaySize.y);
	ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix),
		operation, ImGuizmo::MODE::WORLD,
		glm::value_ptr(transform), NULL);

	if (ImGuizmo::IsUsingAny()) {
		if (operation == ImGuizmo::OPERATION::TRANSLATE)
			if (previousTransform != transform) {
				positionPrev[0] = position[0];
				positionPrev[1] = position[1];
				positionPrev[2] = position[2];

				position[0] = transform[3][0];
				position[1] = transform[3][1];
				position[2] = transform[3][2];
				//std::cout << "\n Translate ";
				auto delta = glm::vec3(transform[3]) - glm::vec3(previousTransform[3]);

				for (auto x : app->objectIndices)
					objectSingleton->getObject(x)->translate(delta);
			}

		if (operation == ImGuizmo::OPERATION::ROTATE)
		{

			bool transformsDifferent = false;
			const float matrixmyEpsilon = 1e-5f;
			for (int i = 0; i < 4 && !transformsDifferent; ++i)
				for (int j = 0; j < 4; ++j)
					if (fabs(previousTransform[i][j] - transform[i][j]) > matrixmyEpsilon)
					{
						transformsDifferent = true;
						break;
					}

			if (transformsDifferent)
			{
				glm::quat currentQuat = glm::quat_cast(transform);
				glm::quat prevQuat = glm::quat_cast(previousTransform);

				glm::quat deltaQuat = currentQuat * glm::inverse(prevQuat);
				deltaQuat = glm::normalize(deltaQuat);

				glm::vec3 euler = glm::eulerAngles(currentQuat);
				rotation[0] = euler.x * radian;
				rotation[1] = euler.y * radian;
				rotation[2] = euler.z * radian;

				if (fabs(rotation[0]) < myEpsilon) rotation[0] = 0.0f;
				if (fabs(rotation[1]) < myEpsilon) rotation[1] = 0.0f;
				if (fabs(rotation[2]) < myEpsilon) rotation[2] = 0.0f;

				std::cout << rotation[0] << " " << rotation[1] << " " << rotation[2] << " ";


				float angle = glm::angle(deltaQuat);
				if (angle > myEpsilon)
				{
					glm::vec3 axis = glm::axis(deltaQuat);
					for (auto x : app->objectIndices)
					{
						auto object = objectSingleton->getObject(x);
						object->rotate(glm::degrees(angle), axis);
					}
				}


				previousTransform = transform;
			}
		}

		if (operation == ImGuizmo::OPERATION::SCALE)
			if (previousTransform != transform) {

				scalePrev[0] = scale[0];
				scalePrev[1] = scale[1];
				scalePrev[2] = scale[2];

				scale[0] = transform[0][0];
				scale[1] = transform[1][1];
				scale[2] = transform[2][2];
				if (scale[0] == 0)scale[0] = 0.00001;
				if (scale[1] == 0)scale[1] = 0.00001;
				if (scale[2] == 0)scale[2] = 0.00001;

				auto delta = transform / previousTransform;
				for (auto x : app->objectIndices)
					objectSingleton->getObject(x)->scale(delta[0][0], delta[1][1], delta[2][2]);
			}
		objectBVHSingleton->Refit();
		//VertexBVHSingleton->Refit();
	}
	previousTransform = transform;
}

void MyGUI::vertexTransform()
{
	static float offset[3];

	Object* activeObject = objectSingleton->getObject(app->objectIndices.back());
	std::vector<DVertex*>& vertices = activeObject->getVertices();
	int numberOfVertices = activeObject->getNumberOfVertices();

	std::vector<int>& selectedVertices = static_cast<Mesh*>(activeObject)->getSelectedVertices();
	if (selectedVertices.size() == 0)return;

	app->vertexPrevPosition[0] = app->vertexPosition[0] = vertices[selectedVertices.back()]->position.x;
	app->vertexPrevPosition[1] = app->vertexPosition[1] = vertices[selectedVertices.back()]->position.y;
	app->vertexPrevPosition[2] = app->vertexPosition[2] = vertices[selectedVertices.back()]->position.z;

	ImGui::InputFloat3("DVertex position", app->vertexPosition);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{


		offset[0] = app->vertexPosition[0] - app->vertexPrevPosition[0];
		offset[1] = app->vertexPosition[1] - app->vertexPrevPosition[1];
		offset[2] = app->vertexPosition[2] - app->vertexPrevPosition[2];

		for (int i = 0; i < selectedVertices.size(); i++)
		{
			vertices[selectedVertices[i]]->translate(offset);
			activeObject->updateVertexBuffer(selectedVertices[i]);
		}

		app->vertexPrevPosition[0] = app->vertexPosition[0];
		app->vertexPrevPosition[1] = app->vertexPosition[1];
		app->vertexPrevPosition[2] = app->vertexPosition[2];

		VertexBVHSingleton->Refit(*activeObject);
	}
}

void MyGUI::transformations()
{
	if (!app->objectIndices.size()) return;

	Object* activeObject = objectSingleton->getObject(app->objectIndices.back());

	ImGui::InputFloat3("Position", position);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		std::cout << " Object moved ";

		activeObject->translate(position[0] - positionPrev[0], position[1] - positionPrev[1], position[2] - positionPrev[2]);

		positionPrev[0] = position[0];
		positionPrev[1] = position[1];
		positionPrev[2] = position[2];

		objectBVHSingleton->Refit();
	}

	ImGui::InputFloat3("Rotation", rotation);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		std::cout << " Object rotationd ";


		if (rotationPrev[0] != rotation[0])
			activeObject->rotate(rotation[0] - rotationPrev[0], glm::vec3(1.0f, 0.0f, 0.0f));
		else if (rotationPrev[1] != rotation[1])
			activeObject->rotate(rotation[1] - rotationPrev[1], glm::vec3(0.0f, 1.0f, 0.0f));
		else if (rotationPrev[2] != rotation[2])
			activeObject->rotate(rotation[2] - rotationPrev[2], glm::vec3(0.0f, 0.0f, 1.0f));

		objectBVHSingleton->Refit();

		rotationPrev[0] = rotation[0];
		rotationPrev[1] = rotation[1];
		rotationPrev[2] = rotation[2];


		//glm::mat4& model = objectSingleton->getObject(app->objectIndices.back())->getModel();

		//std::cout << std::endl;
		//std::cout << std::fixed << std::setprecision(4);

		//for (int row = 0; row < 4; row++)
		//{
		//	std::cout << "[ ";
		//	for (int col = 0; col < 4; col++)
		//	{
		//		std::cout << std::setw(9) << model[col][row] << " ";
		//	}
		//	std::cout << "]\n";
		//}
	}

	ImGui::InputFloat3("Scale", scale);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		std::cout << " Object scaled ";

		if (scalePrev[0] != scale[0])
			activeObject->scale(scale[0] / scalePrev[0], 1.0f, 1.0f);
		else if (scalePrev[1] != scale[1])
			activeObject->scale(1.0f, scale[1] / scalePrev[1], 1.0f);
		else if (scalePrev[2] != scale[2])
			activeObject->scale(1.0f, 1.0f, scale[2] / scalePrev[2]);

		objectBVHSingleton->Refit();

		scalePrev[0] = scale[0];
		scalePrev[1] = scale[1];
		scalePrev[2] = scale[2];
	}
}

void MyGUI::selectObject()
{
	if (!app->objectIndices.size())return;

	int selectedObjectIndex = app->objectIndices.back();

	if (selectedObjectIndex < objectSingleton->getNumberOfObjects() && selectedObjectIndex >= 0)
	{
		glm::mat4& model = objectSingleton->getObject(selectedObjectIndex)->getModel();
		const glm::vec3& positionVec = objectSingleton->getObject(selectedObjectIndex)->getPosition();
		const glm::vec3& rotationVec = objectSingleton->getObject(selectedObjectIndex)->getRotationVec();
		const glm::vec3& scaleVec = objectSingleton->getObject(selectedObjectIndex)->getScale();

		//std::cout << std::endl;
		//std::cout << std::fixed << std::setprecision(4);

		//for (int row = 0; row < 4; row++)
		//{
		//	std::cout << "[ ";
		//	for (int col = 0; col < 4; col++)
		//	{
		//		std::cout << std::setw(9) << model[col][row] << " ";
		//	}
		//	std::cout << "]\n";
		//}

		//std::cout << std::endl;
		//gizmo = false;
		//std::cout << "\nSELECTED ---> " << objectSingleton->getObject(objectIndex[objectIndex.size()-1])->getName();


		// pauk

		position[0] = positionPrev[0] = positionVec.x;
		position[1] = positionPrev[1] = positionVec.y;
		position[2] = positionPrev[2] = positionVec.z;

		scale[0] = scalePrev[0] = scaleVec.x;
		scale[1] = scalePrev[1] = scaleVec.y;
		scale[2] = scalePrev[2] = scaleVec.z;

		//std::cout << "\nRotation quat " << objectSingleton->getObject(selectedObjectIndex)->getRotation().x << " "
		//	<< objectSingleton->getObject(selectedObjectIndex)->getRotation().y << " "
		//	<< objectSingleton->getObject(selectedObjectIndex)->getRotation().z << " "
		//	<< objectSingleton->getObject(selectedObjectIndex)->getRotation().w << " ";
		//std::cout << "\nRotation vec: " << rotationVec.x << " " << rotationVec.y << " " << rotationVec.z;

		rotation[0] = rotationVec.x;
		rotation[1] = rotationVec.y;
		rotation[2] = rotationVec.z;

		if (rotation[0] < myEpsilon)
			rotation[0] = rotationPrev[0] = 0;
		else rotationPrev[0] = rotation[0];
		if (rotation[1] < myEpsilon)
			rotation[1] = rotationPrev[1] = 0;
		else rotationPrev[1] = rotation[1];
		if (rotation[2] < myEpsilon)
			rotation[2] = rotationPrev[2] = 0;
		else rotationPrev[2] = rotation[2];


	}
	else {
		std::cout << "\nSELECTED ---> nothing";
		gizmo = false;

		position[0] = positionPrev[0] = -1000000;
		position[1] = positionPrev[1] = -1000000;
		position[2] = positionPrev[2] = -1000000;

		rotation[0] = rotationPrev[0] = -1000000;
		rotation[1] = rotationPrev[1] = -1000000;
		rotation[2] = rotationPrev[2] = -1000000;

		scale[0] = scalePrev[0] = -1000000;
		scale[1] = scalePrev[1] = -1000000;
		scale[2] = scalePrev[2] = -1000000;
	}
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
void MyGUI::drawGrid3D()
{
	static auto& shader = shaderSingleton->getShader("Grid");
	shader.activate();

	grid3DVAO.bind();
	grid3DEBO.bind();

	shader.setBool(true, "DDD", true);


	getWindow()->getCamera()->cameraUniform(true, shader, "cameraMatrix");

	glDrawElements(GL_LINES, gridIndices3D.size(), GL_UNSIGNED_INT, 0);
}
void MyGUI::drawGrid2D()
{
	static auto& shader = shaderSingleton->getShader("Grid");
	shader.activate();

	grid2DVAO.bind();
	grid2DEBO.bind();

	shader.setBool(true, "DDD", false);

	getWindow()->getCamera()->cameraUniform(true, shader, "cameraMatrix");

	glDrawElements(GL_LINES, gridIndices2D.size(), GL_UNSIGNED_INT, 0);
}

void MyGUI::modes()
{
	static const char* modes[] = { "Object mode","Edit mode","Sculpt mode","Weight paint","Texture paint ","UV Editor","Shader Editor" };

	if (ImGui::Button(modes[int(app->mode)]))
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
					app->mode = Mode(i);

					if (app->mode == Mode::UV_EDIT)
						getWindow()->setCamera(cameraSingleton->getCamera("UV"));
					else if (app->mode == Mode::SHADER_EDIT)
						getWindow()->setCamera(cameraSingleton->getCamera("Shader"));
					else
						getWindow()->setCamera(cameraSingleton->getCamera("Viewport"));
				}
		ImGui::EndPopup();
	}


}

void MyGUI::dMesh()
{

	ImGui::Begin("DMesh");

	Mesh* mesh = dynamic_cast<Mesh*> (objectSingleton->getObject(app->objectIndices.back()));

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

		getWindow()->getKeys()[GLFW_KEY_A] = 0;

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


		mesh->renderDraw(*getWindow()->getCamera());
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


	openFile(filename);

}



int flatten(RaytracingBVHNode* node, std::vector<flatRTNode>& out, std::vector<Triangle>&trinagles)
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

void sendData(Shader &shader)
{


	RaytracingBVHSingleton->Build();

	std::vector<flatRTNode>& gpuNodes=RaytracingBVHSingleton->getNodes();
	std::vector<Triangle> gpuTriangles = RaytracingBVHSingleton->getTriangles();	

	std::cout << "\nGPU BVH nodes: " << gpuNodes.size() << "\n";
	std::cout << "Triangles size: " << gpuTriangles.size() << "\n\n";

	/*for (auto& tri : gpuTriangles)
	{

		std::cout << "\n\n"<<tri.v0x << " " << tri.v0y << " " << tri.v0z << "\n";
		std::cout << tri.v1x << " " << tri.v1y << " " << tri.v1z << "\n";
		std::cout << tri.v2x << " " << tri.v2y << " " << tri.v2z << "\n";
	}
	*/
	for (auto& node : gpuNodes)
		std::cout << "node.triIndex = " << node.triIndex << "\t node.right = "<<node.right << "\n";
	

	GLuint bvhBuffer, triBuffer;
	//
	glGenBuffers(1, &bvhBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, gpuNodes.size() * sizeof(flatRTNode), gpuNodes.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bvhBuffer);

	glGenBuffers(1, &triBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, gpuTriangles.size() * sizeof(Triangle), gpuTriangles.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, triBuffer);




	shader.setVector3f(true, "camPos", cameraSingleton->getCamera(0)->getPosition());
	shader.setVector3f(true, "camDir", cameraSingleton->getCamera(0)->getOrientation());
	shader.setVector3f(true, "camUp", cameraSingleton->getCamera(0)->getUp());
	shader.setVector3f(true, "camRight", glm::normalize(glm::cross(cameraSingleton->getCamera(0)->getOrientation(), cameraSingleton->getCamera(0)->getUp())));
	shader.setFloat(true, "fov", glm::radians(cameraSingleton->getCamera(0)->getFOV()));
	shader.setVector2i(true, "resolution", glm::vec2(cameraSingleton->getCamera(0)->getWidth(), cameraSingleton->getCamera(0)->getHeight()));

}

void MyGUI::raytraceRender(const char* filename, int width, int height)
{
	auto t0 = std::chrono::high_resolution_clock::now();
	Shader computeShader("ComputeShader", "computeTest.comp");

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	computeShader.activate();



	sendData(computeShader);
	//
	auto s0 = std::chrono::high_resolution_clock::now();
	//
	glDispatchCompute(width, height, 1);
	//
	//
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	auto s1 = std::chrono::high_resolution_clock::now();
	double shader_ms = std::chrono::duration<double, std::milli>(s1 - s0).count();

	//for (auto x : gpuTriangles)
	//	std::cout << "Triangle .debug = " << x.hit << "\n";
	



	// Read back texture 
	std::vector<float> pixels(width * height * 4);

	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels.data());



	// Convert float -> unsigned char
	std::vector<unsigned char> u8(width * height * 3);

	for (int i = 0; i < width * height; i++)
	{
		u8[i * 3 + 0] = (unsigned char)(glm::clamp(pixels[i * 4 + 0], 0.0f, 1.0f) * 255);
		u8[i * 3 + 1] = (unsigned char)(glm::clamp(pixels[i * 4 + 1], 0.0f, 1.0f) * 255);
		u8[i * 3 + 2] = (unsigned char)(glm::clamp(pixels[i * 4 + 2], 0.0f, 1.0f) * 255);
	}

	// Flip vertically
	std::vector<unsigned char> flipped(width * height * 3);
	for (int y = 0; y < height; ++y)
	{
		memcpy(&flipped[y * width * 3],
			&u8[(height - 1 - y) * width * 3],
			width * 3);
	}


	stbi_write_png(filename, width, height, 3, flipped.data(), width * 3);

	auto t1 = std::chrono::high_resolution_clock::now();
	double total_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

	std::cout << "Shader time: " << shader_ms << " ms\n";
	std::cout << "Render time: " << total_ms << " ms\n\n";

	openFile(filename);

	glDeleteTextures(1, &texture);





}









