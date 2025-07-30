#include "MyGUI.h"
#include <iomanip>
#include "glad/glad.h"
#include "Window.h"
#include "DFace.h"
#include "DLoop.h"
#include <unordered_set>

MyGUI::MyGUI(Window* window) :io(nullptr), gizmoIo(nullptr), window(window) {}

void MyGUI::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	gizmoIo = &ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window->GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");

	InitializeGrid();
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
		ImGui::Checkbox("BVHTree", &BVHTree);
		ImGui::SameLine();
		ImGui::InputInt("BVHTreeSubdivision", &eBVHSubd);
		ImGui::Checkbox("FaceCulling", &faceCulling);
		if (edit)
		{
			Mesh* mesh = static_cast<Mesh*>(objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1]));
			VertexBVHSingleton->BuildBottomUp(*objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1]));
			EdgeBVHSingleton->BuildBottomUp(*mesh);
			FaceBVHSingleton->BuildBottomUp(*mesh);
			std::cout << "built";
			edit = false;
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

	ImGui::End();

	if (app->mode == Mode::EDIT)
		DMesh();

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
			ImGui::MenuItem("Light test");
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
			std::cout << "\n\naj karamba ";

		}
		else if (selected_option == 1)
		{
			mesh->insetIndividual(mesh->getSelectedFaces());
			window->getKeys()[GLFW_KEY_S] = 1;
			std::cout << "\n\naj karamba ";

		}




		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && hoverTime > 1.4)
		{
			glfwSetTime(0);
			hoverTime = 0;
			if (selected_option		!= -1)
			{
				
				std::cout << "\n\naj karamba ";
			}
			showInsetMenu = false;
			ImGui::CloseCurrentPopup();
		}
		selected_option = -1;
		ImGui::EndPopup();
	}

}

void MyGUI::Gizmos()
{
	Camera* camera = cameraSingleton->getCamera(0);

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
			if (previousTransform != transform) {
				//std::cout << "\n Rotate ";

				app->rotatePrev[0] = app->rotate[0];
				app->rotatePrev[1] = app->rotate[1];
				app->rotatePrev[2] = app->rotate[2];

				if (transform[2][1] != 1 && transform[2][1] != -1)
				{
					app->rotate[0] = -glm::asin(transform[2][1]);
				}

				app->rotate[0] = std::atan2(-transform[2][1], transform[2][2]) * radian;
				app->rotate[1] = std::atan2(transform[2][0], std::sqrt(transform[0][0] * transform[0][0] + transform[1][0] * transform[1][0])) * radian;
				app->rotate[2] = std::atan2(-transform[1][0], transform[0][0]) * radian;

				std::cout << app->rotate[0] << " " << app->rotate[1] << " " << app->rotate[2] << " ";

				if (fabs(app->rotate[0]) < epsilon)
					app->rotate[0] = 0;

				if (fabs(app->rotate[1]) < epsilon)
					app->rotate[1] = 0;

				if (fabs(app->rotate[2]) < epsilon)
					app->rotate[2] = 0;
				for (auto x : app->objectIndices)
				{
					auto object = objectSingleton->getObject(x);

					if (fabs(app->rotate[0]) > epsilon)
						object->Rotate(app->rotate[0] - app->rotatePrev[0], glm::vec3(1.0f, 0.0f, 0.0f));
					if (fabs(app->rotate[1]) > epsilon)
						object->Rotate(app->rotate[1] - app->rotatePrev[1], glm::vec3(0.0f, 1.0f, 0.0f));
					if (fabs(app->rotate[2]) > epsilon)
						object->Rotate(app->rotate[2] - app->rotatePrev[2], glm::vec3(0.0f, 0.0f, 1.0f));
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

		if (app->rotate[0] < epsilon)
			app->rotate[0] = app->rotatePrev[0] = 0;
		else app->rotatePrev[0] = app->rotate[0];
		if (app->rotate[1] < epsilon)
			app->rotate[1] = app->rotatePrev[1] = 0;
		else app->rotatePrev[1] = app->rotate[1];
		if (app->rotate[2] < epsilon)
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

void MyGUI::InitializeGrid(int width)
{
	int z = 0;
	for (int i = 0; i <= width; i++)
	{
		gridVertices.push_back(glm::vec2(-width * 0.5, width * 0.5 - i));
		gridVertices.push_back(glm::vec2(width * 0.5, width * 0.5 - i));

		gridIndices.push_back(z++);
		gridIndices.push_back(z++);
	}
	for (int i = 1; i < width; i++)
	{
		gridVertices.push_back(glm::vec2(-width * 0.5 + i, width * 0.5));
		gridVertices.push_back(glm::vec2(-width * 0.5 + i, -width * 0.5));
		gridIndices.push_back(z++);
		gridIndices.push_back(z++);
	}

	gridIndices.push_back(0);
	gridIndices.push_back(width * 2);

	gridIndices.push_back(1);
	gridIndices.push_back(width * 2 + 1);

	gridVAO.Bind();
	VBO VBO(gridVertices);
	gridEBO.bufferData(gridIndices);

	gridVAO.LinkAttribute(VBO, 0, 2, GL_FLOAT, sizeof(glm::vec2), (void*)0);

	gridVAO.Unbind();
	VBO.Unbind();
	gridEBO.Unbind();


}

void MyGUI::Grid()
{
	static auto& shader = shaderSingleton->getShader("Grid");
	shader.Activate();

	gridVAO.Bind();
	gridEBO.Bind();

	cameraSingleton->getCamera(0)->CameraUniform(shader, "cameraMatrix");

	glDrawElements(GL_LINES, gridIndices.size(), GL_UNSIGNED_INT, 0);
}

void MyGUI::Modes()
{
	static const char* modes[] = { "Object mode","Edit mode","Sculpt mode","Weight paint mode","Texture paint mode" };

	if (ImGui::Button("Mode - "))
		ImGui::OpenPopup("Modes");

	ImGui::SameLine();
	ImGui::TextUnformatted(modes[int(app->mode)]);

	if (ImGui::BeginPopup("Modes"))
	{
		ImGui::SeparatorText("Mode");
		for (int i = 0; i < IM_ARRAYSIZE(modes); i++)
			if (ImGui::Selectable(modes[i]))
				app->mode = Mode(i);

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
