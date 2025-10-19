#include "MyGUI.h"
#include <iomanip>
#include "glad/glad.h"
#include "Window.h"
#include "DFace.h"
#include "DLoop.h"
#include <unordered_set>
#include "stb/stb_image_write.h"
#include "ShadingNodes/MaterialManager.h"


#include "ShadingNodes/TextureNode.h"
#include "ShadingNodes/MathNode.h"
#include "ShadingNodes/ValueNode.h"
#include "ShadingNodes/ColorOutputNode.h"
#include "ShadingNodes/NormalOutputNode.h"
#include "ShadingNodes/RoughnessOutputNode.h"
#include "ShadingNodes/MetallicOutputNode.h"
#include "ShadingNodes/AmbientOcclusionOutputNode.h"
#include "ShadingNodes/ColorNode.h"
#include "ShadingNodes/ColorMixNode.h"


#include "Lights/Light.h"
#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"
#include "Lights/SpotLight.h"

#include <windows.h>

void MyGUI::SaveFinalRender(const char* filename, int width, int height)
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


		mesh->renderDraw(*window->getCamera());
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


	ShellExecuteA(NULL, "open", filename, NULL, NULL, SW_SHOWNORMAL);

}



MyGUI::MyGUI(Window* window) :io(nullptr), gizmoIo(nullptr), window(window) {}

void MyGUI::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	gizmoIo = &ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");

	ImNodes::CreateContext();

	InitializeGrid2D();
	InitializeGrid3D();
}
void MyGUI::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}
void MyGUI::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void MyGUI::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImNodes::DestroyContext();
	ImGui::DestroyContext();
}

ImGuiIO* MyGUI::getIO() { return io; }
Mode MyGUI::getMode()
{
	return app->mode;
}
SelectMode MyGUI::getSelectMode()
{
	return app->selectMode;
}
std::vector<int>& MyGUI::getObjectIndex() { return app->objectIndices; }

void MyGUI::DrawUI()
{
	ImGui::Begin("Dardaneli - ImGUI");

	if (ImGui::Button("Save Final Render"))
	{
		int fbWidth, fbHeight;
    glfwGetFramebufferSize(window->getWindow(), &fbWidth, &fbHeight); // not window size!
    SaveFinalRender("final_render.png", fbWidth, fbHeight);
	}



	Modes();

	static bool edit = true;

	if (app->mode == Mode::OBJECT)
	{
		edit = true;
		ImGui::Checkbox("BVHTree", &BVHTree);
		ImGui::SameLine();
		ImGui::Checkbox("FaceCulling", &faceCulling);
		ImGui::InputInt("BVHTreeSubdivision", &BVHSubd);
		ImGui::Checkbox("Gizmo", &gizmo);

		if (gizmo)
			Gizmos();

		if (showAddMenu)
			Add();



		if (BVHTree)
			DrawBVH();

		if (app->objectIndices.size())
			if (ImGui::InputInt("Index", &app->objectIndices[app->objectIndices.size() - 1]))
				SelectObject();

		Transformations();
	}
	else if (app->mode == Mode::EDIT)
	{
		Mesh* mesh = nullptr;
		if (edit)
		{
			double time = glfwGetTime();
			mesh = static_cast<Mesh*>(objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1]));
			VertexBVHSingleton->BuildBottomUp(*objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1]));
			EdgeBVHSingleton->BuildBottomUp(*mesh);
			FaceBVHSingleton->BuildBottomUp(*mesh);
			std::cout << "All 3 BVHs built in "<<glfwGetTime()-time<<" seconds";
			edit = false;
		}


		ImGui::Checkbox("BVHTree", &BVHTree);
		ImGui::SameLine();
		ImGui::InputInt("BVHTreeSubdivision", &eBVHSubd);
		ImGui::Checkbox("FaceCulling", &faceCulling);
		ImGui::Button("Mark seam");
		if (ImGui::IsItemClicked())
		{
			ImGui::SetTooltip("Mark seam for selected edges");
			std::cout << "\n\nSeams marked";
			for (DEdge* edge : static_cast<Mesh*>(objectSingleton->getObject(app->objectIndices.back()))->getSelectedEdges())
				edge->isSeam = true;

			static_cast<Mesh*>(objectSingleton->getObject(app->objectIndices.back()))->lscmUVUnwrap();
		}
		ImGui::Button("Clear seam");
		if (ImGui::IsItemClicked())
		{
			ImGui::SetTooltip("Clear seam for selected edges");

			for (DEdge* edge : static_cast<Mesh*>(objectSingleton->getObject(app->objectIndices.back()))->getSelectedEdges())
				edge->isSeam = false;

			static_cast<Mesh*>(objectSingleton->getObject(app->objectIndices.back()))->mergeUVs();
		}

		if (BVHTree)
		{
			//VertexBVHSingleton->Draw(*cameraSingleton->getCamera(0),shaderSingleton->getShader("AABB"),eBVHSubd);
			if (app->selectMode == SelectMode::VERTEX)
			{
				VertexBVHSingleton->DrawLeaves(VertexBVHSingleton->getRoot(), *cameraSingleton->getCamera(0), shaderSingleton->getShader("AABB"));

			}
			if (app->selectMode == SelectMode::EDGE)
			{
				EdgeBVHSingleton->DrawLeaves(EdgeBVHSingleton->getRoot(), *cameraSingleton->getCamera(0), shaderSingleton->getShader("AABB"));

			}
			if (app->selectMode == SelectMode::FACE)
			{
				FaceBVHSingleton->DrawLeaves(FaceBVHSingleton->getRoot(), *cameraSingleton->getCamera(0), shaderSingleton->getShader("AABB"));

			}
			//if(app->selectMode==SelectMode::FACE)EdgeBVHSingleton->DrawLeaves(VertexBVHSingleton->getRoot(), *cameraSingleton->getCamera(0), shaderSingleton->getShader("AABB"));

		}


		auto& vertexIndicesTemp = static_cast<Mesh*>(objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1]))->getSelectedVertices();
		if (vertexIndicesTemp.size())
			(ImGui::InputInt("Index", &vertexIndicesTemp[vertexIndicesTemp.size() - 1]));

		static int e = 0;
		e = (int)app->selectMode;
		ImGui::RadioButton("DVertex select", &e, 0); ImGui::SameLine();
		ImGui::RadioButton("Edge select", &e, 1); ImGui::SameLine();
		ImGui::RadioButton("DFace select", &e, 2);

		if (e == 0)app->selectMode = SelectMode::VERTEX;
		else if (e == 1)app->selectMode = SelectMode::EDGE;
		else if (e == 2)app->selectMode = SelectMode::FACE;

		VertexTransform();


		if (showDeleteMenu)
			Delete();

		if (showExtrudeMenu)
			Extrude();

		if (showInsetMenu)
			Inset();
	}
	else if (app->mode == Mode::UV_EDITOR)
	{
		ImGui::Checkbox("BVHTree", &BVHTree);

		Mesh* mesh = static_cast<Mesh*>(objectSingleton->getObject(app->objectIndices.back()));
		UVVertexBVHSingleton->BuildBottomUp(*mesh); // prebaci ovo na unwrap funkciju

		if (BVHTree)
		{
			UVVertexBVHSingleton->DrawLeaves(UVVertexBVHSingleton->getRoot(), *cameraSingleton->getCamera("UV"), shaderSingleton->getShader("AABB"));
		}
	}
	else if (app->mode == Mode::SHADER_EDITOR)
	{
		Mesh* mesh = static_cast<Mesh*>(objectSingleton->getObject(app->objectIndices.back()));
		if (showAddMenu)
		{

			addShadingNodes();
		}

		ImGui::Begin("Materials");

		static int current = 0;


		std::vector<Material*> materials;

		// prave se (nepotrebne?) kopije pokazivaca na materijale
		if (current == 0)
		{
			for (auto mat : *mesh->getAllMaterials())
				materials.push_back(mat.first);
		}
		else
			materials = materialSingleton->getAllMaterials();

		std::sort(materials.begin(), materials.end(),
			[](Material* a, Material* b) {
				return a->getName() < b->getName();
			});



		ImGui::RadioButton("Object Materials", &current, 0);
		ImGui::SameLine();
		if (ImGui::Button("Assign Material")) {

			mesh->assignMaterial(app->activeMaterial);

			std::cout << "\nMaterial assigned" << "\t  number of materials: "<<mesh->getAllMaterials()->size();

		}
		ImGui::SameLine();
		if (ImGui::Button("Remove Material")) {

			mesh->removeMaterial(app->activeMaterial);

		}
		ImGui::RadioButton("All Materials", &current, 1);
		ImGui::SameLine();
		if (ImGui::Button("Add new material")) {
			std::string matName = "Material " + std::to_string(materials.size());
			mesh->addMaterial(new Material(matName));

		}

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

		ImGui::End();
	}
	ImGui::End();

	if (app->mode == Mode::EDIT)
		DMesh();


	if (ImGui::Begin("Render Mode"))
	{
		int current = static_cast<int>(app->renderMode);

		if (ImGui::RadioButton("Wireframe", &current, 0)) app->renderMode = RenderMode::WIREFRAME;
		if (ImGui::RadioButton("Solid", &current, 1)) app->renderMode = RenderMode::SOLID;
		if (ImGui::RadioButton("Material Preview", &current, 2)) app->renderMode = RenderMode::MATERIAL_PREVIEW;
		if (ImGui::RadioButton("Render", &current, 3)) app->renderMode = RenderMode::RENDER;

	}
	ImGui::End();


}

void MyGUI::DrawBVH() { objectBVHSingleton->Draw(*cameraSingleton->getCamera(0), shaderSingleton->getShader("AABB"), BVHSubd); }

void MyGUI::Add() {
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

			window->getKeys()[GLFW_KEY_Q] = 0;
			//std::cout << "HEHEHAHA ";
			showAddMenu = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
void MyGUI::Delete()
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

			window->getKeys()[GLFW_KEY_X] = 0;
			//std::cout << "HEHEHAHA ";
			showDeleteMenu = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
void MyGUI::AddMenu() { showAddMenu = true; }

void MyGUI::DeleteMenu()
{
	showDeleteMenu = true;


}

void MyGUI::ExtrudeMenu()
{
	showExtrudeMenu = true;
}

void MyGUI::Extrude()
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

			window->getKeys()[GLFW_KEY_E] = 0;
			//std::cout << "HEHEHAHA ";
			showExtrudeMenu = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}


}

void MyGUI::InsetMenu()
{
	showInsetMenu = true;
}

void MyGUI::Inset()
{
	hoverTime = glfwGetTime();
	static int selected_option = -1;
	const char* options[] = { "Group", "Individual" };

	ImGui::OpenPopup("Inset popup");

	if (ImGui::BeginPopup("Inset popup"))
	{
		ImGui::SeparatorText("Inset");
		ImGui::Separator();

		Mesh* mesh = dynamic_cast<Mesh*>(objectSingleton->getObject(app->objectIndices.back()));
		if (!mesh)return;

		for (int i = 0; i < 2; ++i) {
			if (i == 6) {
				//ImGui::Separator();
			}

			if (ImGui::Selectable(options[i])) {
				selected_option = i;
			}
		}


		if (selected_option == 0)
		{
			mesh->inset(mesh->getSelectedFaces());
			window->getKeys()[GLFW_KEY_S] = 1;

		}
		else if (selected_option == 1)
		{
			mesh->insetIndividual(mesh->getSelectedFaces());
			window->getKeys()[GLFW_KEY_S] = 1;

		}




		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && hoverTime > 1.4)
		{
			glfwSetTime(0);
			hoverTime = 0;
			showInsetMenu = false;
			ImGui::CloseCurrentPopup();
		}
		selected_option = -1;
		ImGui::EndPopup();
	}

}

void MyGUI::Gizmos()
{
	Camera* camera = window->getCamera();

	ImGuizmo::SetOrthographic(false);

	ImGuizmo::AllowAxisFlip(false);

	glm::mat4 viewMatrix = camera->getViewMatrix();
	glm::mat4 projMatrix = camera->getProjectionMatrix();
	//OVDE
	static glm::mat4 transform = glm::mat4(1.0f);
	if (app->objectIndices.size())
		if (app->objectIndices[app->objectIndices.size() - 1] != -1 && app->objectIndices[app->objectIndices.size() - 1] < objectSingleton->getNumberOfObjects())
			transform = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1])->getModelReference();
	static glm::mat4 previousTransform = glm::mat4(1.0f);

	ImGuizmo::SetRect(0, 0, io->DisplaySize.x, io->DisplaySize.y);
	ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix),
		operation, ImGuizmo::MODE::WORLD,
		glm::value_ptr(transform), NULL);

	if (ImGuizmo::IsUsingAny()) {
		if (operation == ImGuizmo::OPERATION::TRANSLATE)
			if (previousTransform != transform) {
				app->translatePrev[0] = app->translate[0];
				app->translatePrev[1] = app->translate[1];
				app->translatePrev[2] = app->translate[2];

				app->translate[0] = transform[3][0];
				app->translate[1] = transform[3][1];
				app->translate[2] = transform[3][2];
				//std::cout << "\n Translate ";
				auto delta = glm::vec3(transform[3]) - glm::vec3(previousTransform[3]);

				for (auto x : app->objectIndices)
					objectSingleton->getObject(x)->Translate(delta);
			}

		if (operation == ImGuizmo::OPERATION::ROTATE)
		{
			// compare matrices with an myEpsilon, not direct !=
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
				// Get quaternions from transforms
				glm::quat currentQuat = glm::quat_cast(transform);
				glm::quat prevQuat = glm::quat_cast(previousTransform);

				// Delta rotation: how to go from previous to current
				glm::quat deltaQuat = currentQuat * glm::inverse(prevQuat);
				deltaQuat = glm::normalize(deltaQuat); // stability

				// Update Euler angles for UI (if you need to display them)
				glm::vec3 euler = glm::eulerAngles(currentQuat); // radians, order: XYZ
				app->rotate[0] = euler.x * radian;
				app->rotate[1] = euler.y * radian;
				app->rotate[2] = euler.z * radian;

				if (fabs(app->rotate[0]) < myEpsilon) app->rotate[0] = 0.0f;
				if (fabs(app->rotate[1]) < myEpsilon) app->rotate[1] = 0.0f;
				if (fabs(app->rotate[2]) < myEpsilon) app->rotate[2] = 0.0f;

				std::cout << app->rotate[0] << " " << app->rotate[1] << " " << app->rotate[2] << " ";

				// Apply the delta rotation to each selected object as axis-angle
				float angle = glm::angle(deltaQuat); // radians
				if (angle > myEpsilon)
				{
					glm::vec3 axis = glm::axis(deltaQuat);
					for (auto x : app->objectIndices)
					{
						auto object = objectSingleton->getObject(x);
						object->Rotate(glm::degrees(angle), axis); // assumes Rotate(angle, axis) expects radians
					}
				}

				// Store for next frame
				previousTransform = transform;
			}
		}

		if (operation == ImGuizmo::OPERATION::SCALE)
			if (previousTransform != transform) {
				//std::cout << "\n Scale ";

				app->scalePrev[0] = app->scale[0];
				app->scalePrev[1] = app->scale[1];
				app->scalePrev[2] = app->scale[2];

				app->scale[0] = transform[0][0];
				app->scale[1] = transform[1][1];
				app->scale[2] = transform[2][2];
				if (app->scale[0] == 0)app->scale[0] = 1;
				if (app->scale[1] == 0)app->scale[1] = 1;
				if (app->scale[2] == 0)app->scale[2] = 1;

				auto delta = transform / previousTransform;
				for (auto x : app->objectIndices)
					objectSingleton->getObject(x)->Scale(delta[0][0], delta[1][1], delta[2][2]);
			}
		objectBVHSingleton->Refit();
		//VertexBVHSingleton->Refit();
	}
	previousTransform = transform;
}

void MyGUI::VertexTransform()
{
	static float offset[3];

	Object* activeObject = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1]);
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
		//std::cout << " DVertex moved ";

		offset[0] = app->vertexPosition[0] - app->vertexPrevPosition[0];
		offset[1] = app->vertexPosition[1] - app->vertexPrevPosition[1];
		offset[2] = app->vertexPosition[2] - app->vertexPrevPosition[2];

		for (int i = 0; i < selectedVertices.size(); i++)
		{
			vertices[selectedVertices[i]]->Translate(offset);
			activeObject->UpdateVertexBuffer(selectedVertices[i]);
		}

		app->vertexPrevPosition[0] = app->vertexPosition[0];
		app->vertexPrevPosition[1] = app->vertexPosition[1];
		app->vertexPrevPosition[2] = app->vertexPosition[2];
		//
		VertexBVHSingleton->Refit(*activeObject);
	}
}

void MyGUI::Transformations()
{
	if (!app->objectIndices.size())return;
	Object* activeObject = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1]);

	ImGui::InputFloat3("Location", app->translate);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		std::cout << " Object moved ";

		activeObject->Translate(app->translate[0] - app->translatePrev[0], app->translate[1] - app->translatePrev[1], app->translate[2] - app->translatePrev[2]);

		app->translatePrev[0] = app->translate[0];
		app->translatePrev[1] = app->translate[1];
		app->translatePrev[2] = app->translate[2];

		objectBVHSingleton->Refit();
	}

	ImGui::InputFloat3("Rotation", app->rotate);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		std::cout << " Object rotated ";

		if (app->rotatePrev[0] != app->rotate[0])
			activeObject->Rotate(app->rotate[0] - app->rotatePrev[0], glm::vec3(1.0f, 0.0f, 0.0f));
		else if (app->rotatePrev[1] != app->rotate[1])
			activeObject->Rotate(app->rotate[1] - app->rotatePrev[1], glm::vec3(0.0f, 1.0f, 0.0f));
		else if (app->rotatePrev[2] != app->rotate[2])
			activeObject->Rotate(app->rotate[2] - app->rotatePrev[2], glm::vec3(0.0f, 0.0f, 1.0f));

		objectBVHSingleton->Refit();

		app->rotatePrev[0] = app->rotate[0];
		app->rotatePrev[1] = app->rotate[1];
		app->rotatePrev[2] = app->rotate[2];
	}

	ImGui::InputFloat3("Scale", app->scale);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		std::cout << " Object scaled ";

		if (app->scalePrev[0] != app->scale[0])
			activeObject->Scale(app->scale[0] / app->scalePrev[0], 1.0f, 1.0f);
		else if (app->scalePrev[1] != app->scale[1])
			activeObject->Scale(1.0f, app->scale[1] / app->scalePrev[1], 1.0f);
		else if (app->scalePrev[2] != app->scale[2])
			activeObject->Scale(1.0f, 1.0f, app->scale[2] / app->scalePrev[2]);

		objectBVHSingleton->Refit();

		app->scalePrev[0] = app->scale[0];
		app->scalePrev[1] = app->scale[1];
		app->scalePrev[2] = app->scale[2];
	}
}

void MyGUI::SelectObject()
{
	if (!app->objectIndices.size())return;
	if (app->objectIndices[app->objectIndices.size() - 1] < objectSingleton->getNumberOfObjects() && app->objectIndices[app->objectIndices.size() - 1] >= 0)
	{
		gizmo = false;
		//std::cout << "\nSELECTED ---> " << objectSingleton->getObject(objectIndex[objectIndex.size()-1])->getName();

		app->model = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1])->getModelReference();

		app->translate[0] = app->translatePrev[0] = app->model[3][0];
		app->translate[1] = app->translatePrev[1] = app->model[3][1];
		app->translate[2] = app->translatePrev[2] = app->model[3][2];

		app->rotate[0] = std::atan2(-app->model[2][1], app->model[2][2]) * radian;
		app->rotate[1] = std::atan2(app->model[2][0], std::sqrt(app->model[0][0] * app->model[0][0] + app->model[1][0] * app->model[1][0])) * radian;
		app->rotate[2] = std::atan2(-app->model[1][0], app->model[0][0]) * radian;

		if (app->rotate[0] < myEpsilon)
			app->rotate[0] = app->rotatePrev[0] = 0;
		else app->rotatePrev[0] = app->rotate[0];
		if (app->rotate[1] < myEpsilon)
			app->rotate[1] = app->rotatePrev[1] = 0;
		else app->rotatePrev[1] = app->rotate[1];
		if (app->rotate[2] < myEpsilon)
			app->rotate[2] = app->rotatePrev[2] = 0;
		else app->rotatePrev[2] = app->rotate[2];

		app->scale[0] = app->scalePrev[0] = sqrt(app->model[0][0] * app->model[0][0] + app->model[0][1] * app->model[0][1] + app->model[0][2] * app->model[0][2]);
		app->scale[1] = app->scalePrev[1] = sqrt(app->model[1][0] * app->model[1][0] + app->model[1][1] * app->model[1][1] + app->model[1][2] * app->model[1][2]);
		app->scale[2] = app->scalePrev[2] = sqrt(app->model[2][0] * app->model[2][0] + app->model[2][1] * app->model[2][1] + app->model[2][2] * app->model[2][2]);
	}
	else {
		std::cout << "\nSELECTED ---> nothing";
		gizmo = false;

		app->translate[0] = app->translatePrev[0] = -1000000;
		app->translate[1] = app->translatePrev[1] = -1000000;
		app->translate[2] = app->translatePrev[2] = -1000000;

		app->rotate[0] = app->rotatePrev[0] = -1000000;
		app->rotate[1] = app->rotatePrev[1] = -1000000;
		app->rotate[2] = app->rotatePrev[2] = -1000000;

		app->scale[0] = app->scalePrev[0] = -1000000;
		app->scale[1] = app->scalePrev[1] = -1000000;
		app->scale[2] = app->scalePrev[2] = -1000000;
	}
}

void MyGUI::setGizmoOperation(ImGuizmo::OPERATION op)
{
	operation = op;
}

void MyGUI::InitializeGrid3D(int width)
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

	grid3DVAO.Bind();
	VBO VBO(gridVertices3D);
	grid3DEBO.bufferData(gridIndices3D);

	grid3DVAO.LinkAttribute(VBO, 0, 2, GL_FLOAT, sizeof(glm::vec2), (void*)0);

	grid3DVAO.Unbind();
	VBO.Unbind();
	grid3DEBO.Unbind();


}
void MyGUI::InitializeGrid2D(int width)
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

	grid2DVAO.Bind();
	VBO VBO(gridVertices2D);
	grid2DEBO.bufferData(gridIndices2D);

	grid2DVAO.LinkAttribute(VBO, 0, 2, GL_FLOAT, sizeof(glm::vec2), (void*)0);

	grid2DVAO.Unbind();
	VBO.Unbind();
	grid2DEBO.Unbind();


}
void MyGUI::Grid3D()
{
	static auto& shader = shaderSingleton->getShader("Grid");
	shader.Activate();

	grid3DVAO.Bind();
	grid3DEBO.Bind();

	shader.setBool(true, "DDD", true);


	window->getCamera()->CameraUniform(shader, "cameraMatrix");

	glDrawElements(GL_LINES, gridIndices3D.size(), GL_UNSIGNED_INT, 0);
}

void MyGUI::Grid2D()
{
	static auto& shader = shaderSingleton->getShader("Grid");
	shader.Activate();

	grid2DVAO.Bind();
	grid2DEBO.Bind();

	shader.setBool(true, "DDD", false);

	window->getCamera()->CameraUniform(shader, "cameraMatrix");

	glDrawElements(GL_LINES, gridIndices2D.size(), GL_UNSIGNED_INT, 0);
}

void MyGUI::Modes()
{
	static const char* modes[] = { "Object mode","Edit mode","Sculpt mode","Weight paint","Texture paint ","UV Editor","Shader Editor" };

	if (ImGui::Button("Mode - "))
		ImGui::OpenPopup("Modes");

	ImGui::SameLine();
	ImGui::TextUnformatted(modes[int(app->mode)]);

	if (ImGui::BeginPopup("Modes"))
	{
		ImGui::SeparatorText("Mode");
		for (int i = 0; i < IM_ARRAYSIZE(modes); i++)
			if (i == 4) {
				ImGui::Separator();
			}
			else
				if (ImGui::Selectable(modes[i]))

				{
					app->mode = Mode(i);

					if (app->mode == Mode::UV_EDITOR)
						window->setCamera(cameraSingleton->getCamera("UV"));
					else if (app->mode == Mode::SHADER_EDITOR)
						window->setCamera(cameraSingleton->getCamera("Shader"));
					else
						window->setCamera(cameraSingleton->getCamera("Viewport"));
				}


		ImGui::EndPopup();
	}


}

void MyGUI::DMesh()
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

void MyGUI::ShowShaderEditor()
{
	// ovo negdje drugo prebaciti
	ImNodesStyle& style = ImNodes::GetStyle();
	style.Colors[ImNodesCol_TitleBarSelected] = IM_COL32(200, 200, 0, 255);

	ImGui::Begin("Shader Editor");

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
	{
		if (ImNodes::GetAttributePinShape(outputAttribute) == ImNodes::GetAttributePinShape(inputAttribute) ||
			ImNodes::GetAttributePinShape(inputAttribute) == ImNodesPinShape_Triangle)
		{

			links.push_back(std::make_pair(outputAttribute, inputAttribute));

			std::cout << "\n Link created from attr " << outputAttribute << " to attr " << inputAttribute;
		}
	}

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

		if (ImGui::BeginMenu("Node"))
		{
			ImGui::SeparatorText("Primitives");

			if (ImGui::MenuItem("Tex"))
			{
				app->activeMaterial->createNode<TextureNode>();
				std::cout << "Texture node added \n";
			}
			if (ImGui::MenuItem("Math"))
			{
				app->activeMaterial->createNode<MathNode>();
				std::cout << "Math node added \n";
			}
			if (ImGui::MenuItem("Value"))
			{
				app->activeMaterial->createNode<ValueNode>();
				std::cout << "Value node added \n";
			}
			if (ImGui::MenuItem("Color Output"))
			{
				app->activeMaterial->createNode<ColorOutputNode>();
				std::cout << "Color Output node added \n";
			}
			if (ImGui::MenuItem("Normal Output"))
			{
				app->activeMaterial->createNode<NormalOutputNode>();
				std::cout << "Normal Output node added \n";
			}
			if (ImGui::MenuItem("Roughness Output"))
			{
				app->activeMaterial->createNode<RoughnessOutputNode>();
				std::cout << "Roughness Output node added \n";
			}
			if (ImGui::MenuItem("Metallic Output"))
			{
				app->activeMaterial->createNode<MetallicOutputNode>();
				std::cout << "Metallic Output node added \n";
			}
			if (ImGui::MenuItem("Ambient Occlusion Output"))
			{
				app->activeMaterial->createNode<AmbientOcclusionOutputNode>();
				std::cout << "AmbientOcclusion Output node added \n";
			}
			if (ImGui::MenuItem("Color"))
			{
				app->activeMaterial->createNode<ColorNode>();
				std::cout << "Color node added \n";
			}
			if (ImGui::MenuItem("ColorMix"))
			{
				app->activeMaterial->createNode<ColorMixNode>();
				std::cout << "ColorMix node added \n";
			}

			ImGui::EndMenu();
		}


		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && hoverTime > 1.4)
		{
			glfwSetTime(0);
			hoverTime = 0;

			window->getKeys()[GLFW_KEY_A] = 0;

			showAddMenu = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}



}













