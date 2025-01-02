#include "MyGUI.h"
#include <iomanip>
#include "glad/glad.h"

MyGUI::MyGUI():io(nullptr),gizmoIo(nullptr){}

void MyGUI::Init(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	gizmoIo = &ImGui::GetIO(); 
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
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
	return mode;
}
SelectMode MyGUI::getSelectMode()
{
	return selectMode;
}
int& MyGUI::getObjectIndex() { return objectIndex; }
int& MyGUI::getVertexIndex() { return vertexIndex; }
;

void MyGUI::DrawUI()
{	

	ImGui::Begin("Dardaneli - ImGUI");

	Modes();

	static bool edit = true;

	if(mode == Mode::OBJECT)
	{
		edit = true;
		ImGui::Checkbox("BVHTree", &BVHTree);
		ImGui::InputInt("BVHTreeSubdivision", &BVHSubd);

		if (gizmo)
			Gizmos();

		if (showAddMenu)
			Add();

		if (BVHTree)
			DrawBVH();

		if (ImGui::InputInt("Index", &objectIndex))
			SelectObject();

		Transformations();
	}
	else if (mode == Mode::EDIT)
	{
		ImGui::Checkbox("BVHTree", &BVHTree);
		ImGui::InputInt("BVHTreeSubdivision", &eBVHSubd);

		if (edit)
		{
			editModeBVHSingleton->BuildBottomUp(*objectSingleton->getObject(objectIndex) );
			std::cout << "built";
			edit = false;
		}
		if(BVHTree)
			//editModeBVHSingleton->Draw(*cameraSingleton->getCamera(0),shaderSingleton->getShader("AABB"),eBVHSubd);
		editModeBVHSingleton->DrawLeaves(editModeBVHSingleton->getRoot(),*cameraSingleton->getCamera(0), shaderSingleton->getShader("AABB"));
		(ImGui::InputInt("Index", &vertexIndex));
		if (glfwGetTime() > 10)
		{
			//objectSingleton->getObject(objectIndex)->getVertices()->at(vertexIndex).Translate(glm::vec3(-1.0f, 0.0f, 0.0f));
			//objectSingleton->getObject(objectIndex)->UpdateData(vertexIndex);
			//glfwSetTime(0);
			//std::cout << "	moved " << objectSingleton->getObject(objectIndex)->getName() << " " << objectSingleton->getObject(0)->getVertices()->at(vertexIndex).position.x;
			
		}
		static int e = 0;

		ImGui::RadioButton("Vertex select", &e, 0); ImGui::SameLine();
		ImGui::RadioButton("Edge select", &e, 1); ImGui::SameLine();
		ImGui::RadioButton("Face select", &e, 2);

		

		if (e == 0)selectMode = SelectMode::VERTEX;
		else if (e == 1)selectMode = SelectMode::EDGE;
		else if (e == 2)selectMode = SelectMode::FACE;
		VertexTransform();
	}




	ImGui::End();
}


void MyGUI::DrawBVH(){objectBVHSingleton->Draw(*cameraSingleton->getCamera(0), shaderSingleton->getShader("AABB"), BVHSubd);}

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

		if(ImGui::BeginMenu("Empty")) 
		{
			ImGui::MenuItem("Empty test");
			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Image")) {
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

		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && hoverTime> 0.4)
		{
			glfwSetTime(0);
			hoverTime = 0;
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
	
	static glm::mat4 transform = glm::mat4(1.0f);
	if (objectIndex != -1 && objectIndex<objectSingleton->getNumberOfObjects())
		transform = objectSingleton->getObject(objectIndex)->getModelReference();
	static glm::mat4 previousTransform = glm::mat4(1.0f);
	
	
	ImGuizmo::SetRect(0, 0, io->DisplaySize.x, io->DisplaySize.y);
	ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix),
		operation, ImGuizmo::MODE::WORLD,
		glm::value_ptr(transform),NULL);
	
	if (ImGuizmo::IsUsingAny()) {

	

		if (operation == ImGuizmo::OPERATION::TRANSLATE)
			if (previousTransform != transform) {

				translatePrev[0] = translate[0];
				translatePrev[1] = translate[1];
				translatePrev[2] = translate[2];

				translate[0] = transform[3][0];
				translate[1] = transform[3][1];
				translate[2] = transform[3][2];
				std::cout << "\n Translate ";
				auto delta = glm::vec3(transform[3]) - glm::vec3(previousTransform[3]);

				objectSingleton->getObject(objectIndex)->Translate(delta);
			}

		if (operation == ImGuizmo::OPERATION::ROTATE)
			if (previousTransform != transform) {

				std::cout << "\n Rotate ";
				
				rotatePrev[0] = rotate[0];
				rotatePrev[1] = rotate[1];
				rotatePrev[2] = rotate[2];

				if (transform[2][1] != 1 && transform[2][1] != -1)
				{
					rotate[0] = -glm::asin(transform[2][1]);
				}

				rotate[0] = std::atan2(-transform[2][1], transform[2][2]) * radian;
				rotate[1] = std::atan2(transform[2][0], std::sqrt(transform[0][0] * transform[0][0] + transform[1][0] * transform[1][0])) * radian;
				rotate[2] = std::atan2(-transform[1][0], transform[0][0]) * radian;

				std::cout << rotate[0] << " " << rotate[1] << " " << rotate[2] << " ";

				if (fabs(rotate[0])< epsilon)
					rotate[0] = 0;
				
				if (fabs(rotate[1]) < epsilon)
					rotate[1] = 0;
				
				if (fabs(rotate[2]) < epsilon)
					rotate[2] = 0;
				
				auto object = objectSingleton->getObject(objectIndex);
				
				if (fabs(rotate[0]) > epsilon)
					object->Rotate(rotate[0] - rotatePrev[0], glm::vec3(1.0f, 0.0f, 0.0f));
				if (fabs(rotate[1]) > epsilon)
					object->Rotate(rotate[1] - rotatePrev[1], glm::vec3(0.0f, 1.0f, 0.0f));
				if (fabs(rotate[2]) > epsilon)
					object->Rotate(rotate[2] - rotatePrev[2], glm::vec3(0.0f, 0.0f, 1.0f));
			}

		if (operation == ImGuizmo::OPERATION::SCALE)
			if (previousTransform != transform) {
				
				std::cout << "\n Scale ";
				
				scalePrev[0] = scale[0];
				scalePrev[1] = scale[1];
				scalePrev[2] = scale[2];

				scale[0] = transform[0][0];
				scale[1] = transform[1][1];
				scale[2] = transform[2][2];
				if (scale[0] == 0)scale[0] == 1;
				if (scale[1] == 0)scale[1] == 1;
				if (scale[2] == 0)scale[2] == 1;

				auto delta = transform / previousTransform;
				
				objectSingleton->getObject(objectIndex)->Scale(delta[0][0], delta[1][1], delta[2][2]);
			}
		objectBVHSingleton->Refit();
		//editModeBVHSingleton->Refit();
	
	}
	previousTransform = transform;
	


	
	
}

void MyGUI::VertexTransform()
{
	 
	static float offset[3];
	
	if (vertexIndex < 0 && vertexIndex >= objectSingleton->getObject(objectIndex)->getVerticesReference().size())return;
	vertexPrevPosition[0] = vertexPosition[0] = objectSingleton->getObject(objectIndex)->getVerticesReference().at(vertexIndex).getPosition().x;
	vertexPrevPosition[1] = vertexPosition[1] = objectSingleton->getObject(objectIndex)->getVerticesReference().at(vertexIndex).getPosition().y;
	vertexPrevPosition[2] = vertexPosition[2] = objectSingleton->getObject(objectIndex)->getVerticesReference().at(vertexIndex).getPosition().z;

	ImGui::InputFloat3("Vertex position", vertexPosition);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{

		std::cout << " Vertex moved ";
		
		offset[0] = vertexPosition[0] - vertexPrevPosition[0];
		offset[1] = vertexPosition[1] - vertexPrevPosition[1];
		offset[2] = vertexPosition[2] - vertexPrevPosition[2];

		objectSingleton->getObject(objectIndex)->getVerticesReference().at(vertexIndex).Translate(offset);
		objectSingleton->getObject(objectIndex)->UpdateData(vertexIndex);

		vertexPrevPosition[0] = vertexPosition[0];
		vertexPrevPosition[1] = vertexPosition[1];
		vertexPrevPosition[2] = vertexPosition[2];
		//
		editModeBVHSingleton->Refit(*objectSingleton->getObject(objectIndex));

	}
}

void MyGUI::Transformations()
{
	


	ImGui::InputFloat3("Location", translate);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
	
		std::cout << " Object moved ";

		objectSingleton->getObject(objectIndex)->Translate(translate[0]-translatePrev[0], translate[1]-translatePrev[1], translate[2]-translatePrev[2]);

		translatePrev[0] = translate[0];
		translatePrev[1] = translate[1];
		translatePrev[2] = translate[2];

		objectBVHSingleton->Refit();

	}

	ImGui::InputFloat3("Rotation", rotate);
	if(ImGui::IsItemDeactivatedAfterEdit())
	{
		std::cout << " Object rotated ";

		if(rotatePrev[0]!=rotate[0])
			objectSingleton->getObject(objectIndex)->Rotate(rotate[0] - rotatePrev[0], glm::vec3(1.0f, 0.0f, 0.0f));
		else if (rotatePrev[1] != rotate[1])
			objectSingleton->getObject(objectIndex)->Rotate(rotate[1] - rotatePrev[1], glm::vec3(0.0f, 1.0f, 0.0f));
		else if (rotatePrev[2] != rotate[2])
			objectSingleton->getObject(objectIndex)->Rotate(rotate[2] - rotatePrev[2], glm::vec3(0.0f, 0.0f, 1.0f));

		objectBVHSingleton->Refit();

		rotatePrev[0] = rotate[0];
		rotatePrev[1] = rotate[1];
		rotatePrev[2] = rotate[2];


	}

	ImGui::InputFloat3("Scale", scale);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		std::cout << " Object scaled ";

		if (scalePrev[0] != scale[0])
			objectSingleton->getObject(objectIndex)->Scale(scale[0]/scalePrev[0], 1.0f, 1.0f);
		else if (scalePrev[1] != scale[1])
			objectSingleton->getObject(objectIndex)->Scale(1.0f, scale[1]/scalePrev[1], 1.0f);
		else if (scalePrev[2] != scale[2])
			objectSingleton->getObject(objectIndex)->Scale(1.0f, 1.0f, scale[2]/scalePrev[2]);

		objectBVHSingleton->Refit();

		scalePrev[0] = scale[0];
		scalePrev[1] = scale[1];
		scalePrev[2] = scale[2];


	}
}

void MyGUI::SelectObject()
{
	
	
		if (objectIndex < objectSingleton->getNumberOfObjects() && objectIndex >= 0)
		{
			gizmo = true;
			std::cout << "\nSELECTED ---> " << objectSingleton->getObject(objectIndex)->getName();

			model = objectSingleton->getObject(objectIndex)->getModelReference();


			translate[0] = translatePrev[0] = model[3][0];
			translate[1] = translatePrev[1] = model[3][1];
			translate[2] = translatePrev[2] = model[3][2];


			rotate[0] = std::atan2(-model[2][1], model[2][2]) * radian;
			rotate[1] = std::atan2(model[2][0], std::sqrt(model[0][0] * model[0][0] + model[1][0] * model[1][0])) * radian;
			rotate[2] = std::atan2(-model[1][0], model[0][0]) * radian;

			if (rotate[0] < epsilon)
				rotate[0] = rotatePrev[0] = 0;
			else rotatePrev[0] = rotate[0];
			if (rotate[1] < epsilon)
				rotate[1] = rotatePrev[1] = 0;
			else rotatePrev[1] = rotate[1];
			if (rotate[2] < epsilon)
				rotate[2] = rotatePrev[2] = 0;
			else rotatePrev[2] = rotate[2];




			scale[0] = scalePrev[0] = sqrt(model[0][0] * model[0][0] + model[0][1] * model[0][1] + model[0][2] * model[0][2]  );
			scale[1] = scalePrev[1] = sqrt(model[1][0] * model[1][0] + model[1][1] * model[1][1] + model[1][2] * model[1][2]  );
			scale[2] = scalePrev[2] = sqrt(model[2][0] * model[2][0] + model[2][1] * model[2][1] + model[2][2] * model[2][2]  );
		}
		else {
			std::cout << "\nSELECTED ---> nothing";
			gizmo = false;

			translate[0] = translatePrev[0] = -1000000;
			translate[1] = translatePrev[1] = -1000000;
			translate[2] = translatePrev[2] = -1000000;

			rotate[0] = rotatePrev[0] = -1000000;
			rotate[1] = rotatePrev[1] = -1000000;
			rotate[2] = rotatePrev[2] = -1000000;

			scale[0] = scalePrev[0] = -1000000;
			scale[1] = scalePrev[1] = -1000000;
			scale[2] = scalePrev[2] = -1000000;

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
		vertices.push_back(glm::vec2(-width * 0.5, width * 0.5 - i));
		vertices.push_back(glm::vec2(width * 0.5, width * 0.5 - i));
	
		indices.push_back(z++);
		indices.push_back(z++);
	
	}
	for (int i = 1; i < width; i++)
	{
		vertices.push_back(glm::vec2(-width * 0.5 + i, width * 0.5));
		vertices.push_back(glm::vec2(-width * 0.5 + i, -width * 0.5));
		indices.push_back(z++);
		indices.push_back(z++);
	}
	
	indices.push_back(0);
	indices.push_back(width * 2);
	
	indices.push_back(1);
	indices.push_back(width * 2 + 1);
	
	VAO.Bind();
	VBO VBO(vertices);
	EBO EBO(indices);
	
	VAO.LinkAttribute(VBO, 0, 2, GL_FLOAT, sizeof(glm::vec2), (void*)0);
	
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();

	

}

void MyGUI::Grid()
{
	
	
	static auto &shader =shaderSingleton->getShader("Grid");
	shader.Activate();
	VAO.Bind();
	
	cameraSingleton->getCamera(0)->CameraUniform(shader, "cameraMatrix");
	
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);


}

void MyGUI::Modes()
{

	static const char* modes[] = { "Object mode","Edit mode","Sculpt mode","Weight paint mode","Texture paint mode" };

	if (ImGui::Button("Mode - "))
		ImGui::OpenPopup("Modes");

	ImGui::SameLine();
	ImGui::TextUnformatted(modes[int(mode)]);

	if (ImGui::BeginPopup("Modes"))
	{
		ImGui::SeparatorText("Mode");
		for (int i = 0; i < IM_ARRAYSIZE(modes); i++)
			if (ImGui::Selectable(modes[i]))
				mode = Mode(i);

		ImGui::EndPopup();
	}
}







