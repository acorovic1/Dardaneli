#include "MyGUI.h"
#include <iomanip>
#include "glad/glad.h"
#include "Window.h"
#include "Face.h"
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
		ImGui::InputInt("BVHTreeSubdivision", &eBVHSubd);

		if (edit)
		{
			editModeBVHSingleton->BuildBottomUp(*objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1]));
			std::cout << "built";
			edit = false;
		}

		if (BVHTree)
			//editModeBVHSingleton->Draw(*cameraSingleton->getCamera(0),shaderSingleton->getShader("AABB"),eBVHSubd);
			editModeBVHSingleton->DrawLeaves(editModeBVHSingleton->getRoot(), *cameraSingleton->getCamera(0), shaderSingleton->getShader("AABB"));
		auto& vertexIndicesTemp = static_cast<Mesh*>(objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1]))->getSelectedVertices();
		if (vertexIndicesTemp.size())
			(ImGui::InputInt("Index", &vertexIndicesTemp[vertexIndicesTemp.size() - 1]));

		static int e = 0;

		ImGui::RadioButton("Vertex select", &e, 0); ImGui::SameLine();
		ImGui::RadioButton("Edge select", &e, 1); ImGui::SameLine();
		ImGui::RadioButton("Face select", &e, 2);

		if (e == 0)app->selectMode = SelectMode::VERTEX;
		else if (e == 1)app->selectMode = SelectMode::EDGE;
		else if (e == 2)app->selectMode = SelectMode::FACE;

		VertexTransform();
	}

	ImGui::End();

	ImGui::Begin("Half-edge");

	// selected vertices
	auto& vertexIndicesTemp = static_cast<Mesh*>(objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1]))->getSelectedVertices();

	if (vertexIndicesTemp.size())
	{



		static int clicked = 0;


		if (ImGui::Button("Vertex.edge"))
			clicked++;
		if (clicked)
		{
			clicked = 0;

			vertexIndicesTemp = std::vector<int>{ vertexIndicesTemp[vertexIndicesTemp.size() - 1] };

			auto vertices = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1])->getVerticesCopy();

			if (vertices[vertexIndicesTemp[0]].edge == nullptr)
			{
				std::cout << "\nSelected edge is nullptr..vertex.edge\n";
				ImGui::End();
				return;
			}
			selectedEdge = vertices[vertexIndicesTemp[0]].edge;

			auto it = std::find(vertices.begin(), vertices.end(), *selectedEdge->tip);

			vertexIndicesTemp.push_back(it - vertices.begin());
			int x = it - vertices.begin();
			vertexIndicesTemp = std::vector<int>{ vertexIndicesTemp[0],x };

			std::cout << "\n\nStart vertex = " << vertexIndicesTemp[0] << "   End vertex = " << vertexIndicesTemp[1];

		}

		ImGui::SameLine();
		if (ImGui::Button("vert.getFaces"))
			clicked++;
		if (clicked)
		{
			clicked = 0;
			auto vertices = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1])->getVerticesCopy();
			std::unordered_set<Face*> faces;

			for (auto x : vertexIndicesTemp)
			{
				auto temp = vertices[x].getAdjecentFaces();
				faces.insert(temp.begin(), temp.end());
			}
			vertexIndicesTemp.clear();
			std::unordered_set<int> indices;
			std::cout << "\n\n\n";
			for (auto x : faces)
				for (auto y : x->getVertices())
				{
					auto it = std::find(vertices.begin(), vertices.end(), *y);
					std::cout << it - vertices.begin() << " ";

					indices.insert(it - vertices.begin());
				}

			vertexIndicesTemp = std::vector<int>(indices.begin(), indices.end());
			std::cout << "\n Selected vertices are ";
			for (auto x : vertexIndicesTemp)
				std::cout << x << " ";
			std::cout << "\n face vector size " << faces.size();
		}


		if (ImGui::Button("Edge.next"))
			clicked++;
		if (clicked)
		{
			clicked = 0;

			if (selectedEdge->next == nullptr)
			{
				std::cout << "\nSelected edge is nullptr..edge.next\n";
				ImGui::End();
				return;
			}
			auto temp = selectedEdge->tip;
			selectedEdge = selectedEdge->next;

			vertexIndicesTemp = std::vector<int>(2);

			auto vertices = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1])->getVerticesCopy();

			auto it = std::find(vertices.begin(), vertices.end(), *selectedEdge->tip);

			vertexIndicesTemp[1] = it - vertices.begin();


			it = std::find(vertices.begin(), vertices.end(), *temp);
			vertexIndicesTemp[0] = it - vertices.begin();

			std::cout << "\n\nStart vertex = " << vertexIndicesTemp[0] << "   End vertex = " << vertexIndicesTemp[1];
		}


		ImGui::SameLine();
		if (ImGui::Button("vert.getEdges"))
			clicked++;
		if (clicked)
		{
			clicked = 0;
			auto vertices = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1])->getVerticesCopy();
			std::unordered_set<Edge*> edges;

			for (auto x : vertexIndicesTemp)
			{
				auto temp = vertices[x].getAdjecentEdges();
				edges.insert(temp.begin(), temp.end());
			}
			vertexIndicesTemp.clear();
			std::unordered_set<int> indices;
			std::cout << "\n\n\n";
			for (auto x : edges)

			{
				auto it = std::find(vertices.begin(), vertices.end(), *x->tip);
				indices.insert(it - vertices.begin());

				it = std::find(vertices.begin(), vertices.end(), *x->pair->tip);

				indices.insert(it - vertices.begin());
			}

			vertexIndicesTemp = std::vector<int>(indices.begin(), indices.end());
			std::cout << "\n Selected vertices are ";
			for (auto x : vertexIndicesTemp)
				std::cout << x << " ";
			std::cout << "\n edge vector size " << edges.size();
		}



		if (ImGui::Button("Edge.pair"))
			clicked++;
		if (clicked)
		{
			clicked = 0;


			if (selectedEdge->pair == nullptr)
			{
				std::cout << "\nSelected edge is nullptr..edge.pair\n";
				ImGui::End();
				return;
			}
			auto temp = selectedEdge->tip;
			selectedEdge = selectedEdge->pair;

			vertexIndicesTemp = std::vector<int>(2);

			auto vertices = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1])->getVerticesCopy();

			auto it = std::find(vertices.begin(), vertices.end(), *selectedEdge->tip);

			vertexIndicesTemp[1] = it - vertices.begin();


			it = std::find(vertices.begin(), vertices.end(), *temp);
			vertexIndicesTemp[0] = it - vertices.begin();

			std::cout << "\n\nStart vertex = " << vertexIndicesTemp[0] << "   End vertex = " << vertexIndicesTemp[1];


		}

		ImGui::SameLine();
		if (ImGui::Button("vert.getVertices"))
			clicked++;
		if (clicked)
		{
			clicked = 0;
			auto vertices = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1])->getVerticesCopy();
			std::unordered_set<Vertex*> verts;

			for (auto x : vertexIndicesTemp)
			{
				auto temp = vertices[x].getAdjecentVertices();
				verts.insert(temp.begin(), temp.end());
			}
			std::unordered_set<int> indices(vertexIndicesTemp.begin(), vertexIndicesTemp.end());
			vertexIndicesTemp.clear();
			std::cout << "\n\n\n";
			for (auto x : verts)

			{
				auto it = std::find(vertices.begin(), vertices.end(), *x);
				indices.insert(it - vertices.begin());

			}

			vertexIndicesTemp.insert(vertexIndicesTemp.end(),indices.begin(), indices.end());
			std::cout << "\n Selected vertices are ";
			for (auto x : vertexIndicesTemp)
				std::cout << x << " ";
			std::cout << "\n vertex vector size " << verts.size();
		}



		if (ImGui::Button("Edge.tip"))
			clicked++;
		if (clicked)
		{
			clicked = 0;
			if (selectedEdge->tip == nullptr)
			{
				std::cout << "\nSelected tip is nullptr..edge.tip\n";
				ImGui::End();
				return;
			}
			vertexIndicesTemp = std::vector<int>(1);

			auto vertices = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1])->getVerticesCopy();

			auto it = std::find(vertices.begin(), vertices.end(), *selectedEdge->tip);

			vertexIndicesTemp[0] = it - vertices.begin();

			std::cout << "\n\nStart vertex = " << vertexIndicesTemp[0];
		}


		if (ImGui::Button("Edge.face"))
			clicked++;
		if (clicked)
		{
			clicked = 0;
			if (selectedEdge->face == nullptr)
			{
				std::cout << "\nSelected face is nullptr..edge.tip\n";
				ImGui::End();
				return;
			}
			vertexIndicesTemp = std::vector<int>(0);

			auto vertices = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1])->getVerticesCopy();

			selectedFace = selectedEdge->face;
			Edge* e = selectedEdge;
			do {

				auto it = std::find(vertices.begin(), vertices.end(), *e->tip);
				vertexIndicesTemp.push_back(it - vertices.begin());

				e = e->next;


			} while (e != selectedEdge);

			std::cout << "Selected vertices for a face = " << vertexIndicesTemp.size();
		}



		if (ImGui::Button("Face.edge"))
			clicked++;
		if (clicked)
		{
			clicked = 0;
			if (selectedFace->edge == nullptr)
			{
				std::cout << "\nSelected edge is nullptr..face.edge\n";
				ImGui::End();
				return;
			}
			selectedEdge = selectedFace->edge;

			vertexIndicesTemp = std::vector<int>(2);

			auto vertices = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1])->getVerticesCopy();

			auto it = std::find(vertices.begin(), vertices.end(), *selectedEdge->tip);

			vertexIndicesTemp[1] = it - vertices.begin();

			auto temp = selectedEdge->next;

			while (temp->next != selectedEdge)
			{
				temp = temp->next;
			}

			it = std::find(vertices.begin(), vertices.end(), *temp->tip);
			vertexIndicesTemp[0] = it - vertices.begin();

			std::cout << "\n\nStart vertex = " << vertexIndicesTemp[0] << "   End vertex = " << vertexIndicesTemp[1];

		}
		
	
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
			std::cout << "HEHEHAHA ";
			showAddMenu = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
void MyGUI::AddMenu() { showAddMenu = true; }

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
				if (app->scale[0] == 0)app->scale[0] == 1;
				if (app->scale[1] == 0)app->scale[1] == 1;
				if (app->scale[2] == 0)app->scale[2] == 1;

				auto delta = transform / previousTransform;
				for (auto x : app->objectIndices)
					objectSingleton->getObject(x)->Scale(delta[0][0], delta[1][1], delta[2][2]);
			}
		objectBVHSingleton->Refit();
		//editModeBVHSingleton->Refit();
	}
	previousTransform = transform;
}

void MyGUI::VertexTransform()
{
	static float offset[3];

	Object* activeObject = objectSingleton->getObject(app->objectIndices[app->objectIndices.size() - 1]);
	std::vector<Vertex>& vertices = activeObject->getVerticesReference();
	int numberOfVertices = activeObject->getNumberOfVertices();

	std::vector<int>& selectedVertices = static_cast<Mesh*>(activeObject)->getSelectedVertices();
	if (selectedVertices.size() == 0)return;

	app->vertexPrevPosition[0] = app->vertexPosition[0] = vertices[selectedVertices[selectedVertices.size() - 1]].getPositionCopy().x;
	app->vertexPrevPosition[1] = app->vertexPosition[1] = vertices[selectedVertices[selectedVertices.size() - 1]].getPositionCopy().y;
	app->vertexPrevPosition[2] = app->vertexPosition[2] = vertices[selectedVertices[selectedVertices.size() - 1]].getPositionCopy().z;

	ImGui::InputFloat3("Vertex position", app->vertexPosition);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		//std::cout << " Vertex moved ";

		offset[0] = app->vertexPosition[0] - app->vertexPrevPosition[0];
		offset[1] = app->vertexPosition[1] - app->vertexPrevPosition[1];
		offset[2] = app->vertexPosition[2] - app->vertexPrevPosition[2];

		for (int i = 0; i < selectedVertices.size(); i++)
		{
			vertices[selectedVertices[i]].Translate(offset);
			activeObject->UpdateData(selectedVertices[i]);
		}

		app->vertexPrevPosition[0] = app->vertexPosition[0];
		app->vertexPrevPosition[1] = app->vertexPosition[1];
		app->vertexPrevPosition[2] = app->vertexPosition[2];
		//
		editModeBVHSingleton->Refit(*activeObject);
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
	EBO EBO(gridIndices);

	gridVAO.LinkAttribute(VBO, 0, 2, GL_FLOAT, sizeof(glm::vec2), (void*)0);

	gridVAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void MyGUI::Grid()
{
	static auto& shader = shaderSingleton->getShader("Grid");
	shader.Activate();
	gridVAO.Bind();

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