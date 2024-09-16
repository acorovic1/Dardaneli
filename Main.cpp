#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

int width=800, height=600;


int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Dardaneli", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, width, height);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");


	// Cube vertex data (positions, normals, colors, and UVs)
	std::vector<Vertex>cubeVertices{
		// Position         // Normal           // Color           // UV
		// Front face
	Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) }, // V0
		Vertex{ glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) }, // V1
		Vertex{ glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) }, // V2
		Vertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f) }, // V3

		// Back face
		Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) }, // V4
		Vertex{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) }, // V5
		Vertex{ glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) }, // V6
		Vertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f) }, // V7
	};

	// Cube indices for indexed drawing
	std::vector<GLuint> cubeIndices = {
		// Front face
		0, 1, 2,
		2, 3, 0,
		// Back face
		4, 5, 6,
		6, 7, 4,
		// Left face
		0, 3, 7,
		7, 4, 0,
		// Right face
		1, 2, 6,
		6, 5, 1,
		// Top face
		3, 2, 6,
		6, 7, 3,
		// Bottom face
		0, 1, 5,
		5, 4, 0
	};

	// Plane vertex data (positions, normals, colors, and UVs)
	std::vector<Vertex> planeVertices = {
		// Position         // Normal           // Color           // UV
		Vertex{glm::vec3(-0.5f, 0.0f,  0.5f),glm::vec3(0.0f, 1.0f,  0.0f),glm::vec3(1.0f, 0.0f, 0.0f),glm::vec2(0.0f, 0.0f),},  // V0
		Vertex{glm::vec3(0.5f, 0.0f,  0.5f), glm::vec3(0.0f, 1.0f,  0.0f),glm::vec3(0.0f, 1.0f, 0.0f),glm::vec2(1.0f, 0.0f)},  // V1
		Vertex{glm::vec3(0.5f, 0.0f, -0.5f), glm::vec3(0.0f, 1.0f,  0.0f),glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},  // V2
		Vertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(0.0f, 1.0f,  0.0f),glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)}  // V3
	};

	// Plane indices for indexed drawing
	std::vector<GLuint>planeIndices = {
		0, 1, 2,
		2, 3, 0
	};


	std::vector<Texture> textures
	{
		//Texture("planks.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		//Texture("planksSpec.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

	Mesh plane(planeVertices, planeIndices, textures);
	Mesh cube1(cubeVertices, cubeIndices, textures);
	Mesh cube(cubeVertices, cubeIndices, textures);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);


	model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));


	

	Shader basicShader("basic.vert", "basic.frag");
	Shader lightShader("light.vert", "light.frag");
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	basicShader.Activate();
	
	
	glEnable(GL_DEPTH_TEST);
	
	//testiranje indexa
	std::cout << "Cube index = " << cube.getIndex()<<"\n";
	std::cout << "plane index = "<<plane.getIndex() << "\n";
	
	float translation[3] = { 0.0f,0.0f,0.0f };
	int objectIndex=0;
	

	while (!glfwWindowShouldClose(window)) {
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (!io.WantCaptureMouse) {

		camera.Inputs(window);
		}

		camera.Update(45.0f, 0.1f, 100.0f);

		//camera.CameraUniform(basicShader, "cameraMatrix");


		//lightShader.setMat4(false, "model", model);
		//cube.Draw(basicShader, camera);
		cube.Draw(lightShader, camera);
		plane.Draw(lightShader, camera);

		ImGui::Begin("Dardaneli - ImGUI");
		ImGui::Text("Hello");
		
		if (ImGui::InputInt("Index", &objectIndex))
		{
			std::cout << "Index change to - " << objectIndex << "\n";
			translation[0] = 0;
			translation[1] = 0;
			translation[2] = 0;
		}

		if (ImGui::InputFloat3("position", translation)) 
		{
			listOfObjects->getObject(objectIndex)->Translate(translation[0],translation[1],translation[2]);

		}




		
		
		
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		glfwSwapBuffers(window);
		glfwPollEvents();
	};

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	basicShader.Delete();
	lightShader.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();


	return 0;
}