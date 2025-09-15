#include "Renderer.h"

#include "ShaderManager.h"
#include "MyGUI.h"
#include "DFace.h"

void Renderer::viewportEditor()
{

	glClearColor(0.23f, 0.33f, 0.33f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glStencilMask(0x00);

	if (gui.getFaceCulling())
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	gui.Grid3D();



	static Camera* camera = window.getCamera();

	camera->Update();




	auto& selectedObjects = gui.getObjectIndex();
	for (int i = 0; i < objectSingleton->getNumberOfObjects(); i++)
	{
		Object* object = objectSingleton->getObject(i);
		if (std::any_of(selectedObjects.begin(), selectedObjects.end(), [i](int a) {return i == a; }))
		{
			if (gui.getMode() == Mode::OBJECT)
			{
				// OUTLINE STENCIL BUFFER TECHNIQUE


				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);
				glPointSize(5.0f);

				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				object->Draw(shaderSingleton->getShader("Basic"), *camera, GL_LINES);

				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);
				glDisable(GL_DEPTH_TEST);
				if (i == selectedObjects[selectedObjects.size() - 1])
					object->Draw(shaderSingleton->getShader("ActiveSelect"), *camera, GL_LINES);//OUTLINE
				else
					object->Draw(shaderSingleton->getShader("Select"), *camera);//OUTLINE

				glStencilMask(0xFF);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glEnable(GL_DEPTH_TEST);
			}
			else if (gui.getMode() == Mode::EDIT)
			{
				object->Draw(shaderSingleton->getShader("Basic"), *camera, GL_TRIANGLES);

				object->Draw(shaderSingleton->getShader("EditMode"), *camera, GL_LINES);

				if (gui.getSelectMode() == SelectMode::VERTEX)
				{
					glPointSize(5.0f);
					object->Draw(shaderSingleton->getShader("EditMode"), *camera, GL_POINTS); // black color

					auto& selectedVertices = static_cast<Mesh*>(object)->getSelectedVertices();

					object->bindVAO();
					auto x = object->getModelReference();

					Shader& selectShader = shaderSingleton->getShader("SelectEdit"); // orange color
					selectShader.Activate();
					selectShader.setMat4(true, "model", x);
					camera->CameraUniform(selectShader, "cameraMatrix");

					for (int j = 0; j < selectedVertices.size(); j++)
					{
						if (j == selectedVertices.size() - 1)
						{
							Shader& activeShader = shaderSingleton->getShader("ActiveEdit"); // red color
							activeShader.Activate();
							activeShader.setMat4(true, "model", x);
							camera->CameraUniform(activeShader, "cameraMatrix");
						}
						glDrawArrays(GL_POINTS, selectedVertices[j], 1);
					}

					glPointSize(1.0f);
				}
				else if (gui.getSelectMode() == SelectMode::EDGE)
				{
					Mesh* mesh = dynamic_cast<Mesh*>(object);
					if (!mesh)return;
					std::vector<DEdge*>& selectedEdges = mesh->getSelectedEdges();
					std::vector<GLuint> edgeVerts = std::vector<GLuint>();
					std::vector<GLuint> seamVerts = std::vector<GLuint>();
					for (auto& x : selectedEdges)
					{
						std::pair<int, int>edgeVertices = mesh->getEdgeIndices(x);
						edgeVerts.push_back(edgeVertices.first);
						edgeVerts.push_back(edgeVertices.second);


					}
					object->bindVAO();
					auto x = object->getModelReference();

					Shader selectShader = shaderSingleton->getShader("SelectEdit"); // red color
					selectShader.Activate();
					selectShader.setMat4(true, "model", x);
					camera->CameraUniform(selectShader, "cameraMatrix");

					EBO ebo(edgeVerts);
					ebo.Bind();
					if (edgeVerts.size())
					{
						glDrawElements(GL_LINES, edgeVerts.size() - 2, GL_UNSIGNED_INT, 0); // draw red

						Shader activeShader = shaderSingleton->getShader("ActiveEdit"); // orange color
						activeShader.Activate();
						activeShader.setMat4(true, "model", x);
						camera->CameraUniform(activeShader, "cameraMatrix");
						glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)((edgeVerts.size() - 2) * sizeof(GLuint))); // draw orange

					}

					for (DEdge* edge : mesh->getAllEdges())
						if (edge->isSeam)
						{
							std::pair<int, int>edgeVertices = mesh->getEdgeIndices(edge);
							seamVerts.push_back(edgeVertices.first);
							seamVerts.push_back(edgeVertices.second);
						}

					// Draw seam edges (blue/green/whatever shader)
					if (!seamVerts.empty())
					{
						EBO seamEBO(seamVerts);
						seamEBO.Bind();

						Shader seamShader = shaderSingleton->getShader("SelectEdit"); // you define in shader list
						seamShader.Activate();
						seamShader.setMat4(true, "model", x);
						camera->CameraUniform(seamShader, "cameraMatrix");

						//std::cout << "\n\n\t seamVerts size = "<<seamVerts.size();

						glDrawElements(GL_LINES, seamVerts.size(), GL_UNSIGNED_INT, 0);
					}
				}
				else // SelectMode::FACE
				{
					Mesh* mesh = dynamic_cast<Mesh*>(object);
					if (!mesh)return;
					auto drawFaces = mesh->formTrianglesForDrawing();
					/*for (int i = 0;i < drawFaces.size();i++)
					{
						std::cout << drawFaces[i]<<" ";
						if (i % 3 == 2 && i)std::cout << "\n";
					}*/
					std::vector<int> selectedVerts;
					std::vector<DFace*>& selectedFaces = mesh->getSelectedFaces();


					for (auto& x : selectedFaces)
					{
						std::vector<int> temp = mesh->getFaceIndices(x);
						selectedVerts.push_back(x->getVertices().size());
						selectedVerts.insert(selectedVerts.end(), temp.begin(), temp.end());
					}

					/*for (auto x : selectedVerts)
						std::cout << x << " XXX";*/



					if (drawFaces.size())
					{
						object->bindVAO();
						auto x = object->getModelReference();


						Shader selectShader = shaderSingleton->getShader("SelectEdit"); // red color
						selectShader.Activate();
						selectShader.setMat4(true, "model", x);
						camera->CameraUniform(selectShader, "cameraMatrix");

						EBO ebo(drawFaces);
						ebo.Bind();

						int lastFace = selectedVerts[0];
						for (int i = lastFace + 1; i < selectedVerts.size(); i += selectedVerts[i] + 1)
						{
							lastFace = selectedVerts[i];
						}

						lastFace = (lastFace - 2) * 3; // actual number of indices used to make that face


						//std::cout << "\nvec.size " <<drawFaces.size() << " --- lastFace = " << lastFace << "\n";

						glDisable(GL_CULL_FACE);

						glDrawElements(GL_TRIANGLES, drawFaces.size() - lastFace, GL_UNSIGNED_INT, 0); // draw red

						Shader activeShader = shaderSingleton->getShader("ActiveEdit"); // orange color
						activeShader.Activate();
						activeShader.setMat4(true, "model", x);
						camera->CameraUniform(activeShader, "cameraMatrix");
						glDrawElements(GL_TRIANGLES, lastFace, GL_UNSIGNED_INT, (void*)((drawFaces.size() - lastFace) * sizeof(GLuint))); // draw orange

						glEnable(GL_CULL_FACE);
					}
				}
			}
		}

		if (app->renderMode != RenderMode::MATERIAL_PREVIEW)
		{

			camera->CameraUniform(shaderSingleton->getShader("Basic"), "cameraMatrix");
			object->Draw(shaderSingleton->getShader("Basic"), *camera);
		}
		else
		{
			Mesh* mesh = dynamic_cast<Mesh*>(object);
			
			
			//camera->CameraUniform(*shaderr.get(), "cameraMatrix");
			mesh->renderDraw(*camera);
		}
	}
}

void Renderer::uvEditor()
{


	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	static Camera* camera = window.getCamera();
	camera->Update();

	gui.Grid2D();

	// ovo treba uljepsat, izbacit VAOve i VBOve te ih staviti u mesh(vjerovatno)
	Mesh* mesh = dynamic_cast<Mesh*>(app->getActiveObject());
	std::vector<std::shared_ptr<UVVertex>>& uvCoords = mesh->getUVCoords();
	std::vector<glm::vec2> uvCoordsVec;
	for (auto x : uvCoords)
	{
		uvCoordsVec.push_back(x->uv);
	}


	Shader selectShader = shaderSingleton->getShader("UV"); // blue color
	selectShader.Activate();
	camera->CameraUniform(selectShader, "cameraMatrix");

	VAO uvVAO;
	VBO uvVBO(uvCoordsVec);

	uvVAO.Bind();
	uvVBO.Bind();

	uvVAO.LinkAttribute(uvVBO, 0, 2, GL_FLOAT, sizeof(glm::vec2), (void*)0); // UV coordinates

	glPointSize(6.0f);
	glDrawArrays(GL_POINTS, 0, (GLsizei)uvCoordsVec.size());

	auto& edgeIndices = mesh->getUVEdgeIndices();

	EBO edgeEBO(edgeIndices);

	edgeEBO.Bind();
	glDrawElements(GL_LINES, edgeIndices.size(), GL_UNSIGNED_INT, 0);


}

void Renderer::shaderEditor()
{

	glClearColor(0.23f, 0.33f, 0.33f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//viewportEditor();

	gui.ShowShaderEditor();

}



Renderer::Renderer(Window& window, MyGUI& gui) :window(window), gui(gui) {}

void Renderer::Init() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);

	//std::cout << "NUMBER OF SHADERS " << shaderSingleton->getNumberOfShaders();
}

void Renderer::Render()
{

	Mode mode = gui.getMode();
	if (mode == Mode::OBJECT || mode == Mode::EDIT || mode == Mode::SCULPT || mode == Mode::TEXTURE_PAINT || mode == Mode::WEIGHT_PAINT)
		viewportEditor();
	else if (mode == Mode::UV_EDITOR)
		uvEditor();
	else if (mode == Mode::SHADER_EDITOR)
		shaderEditor();
}






