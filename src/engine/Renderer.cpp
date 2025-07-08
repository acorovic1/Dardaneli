#include "Renderer.h"

#include "ShaderManager.h"
#include "MyGUI.h"
#include "DFace.h"

Renderer::Renderer() {}

void Renderer::Init() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);

	//std::cout << "NUMBER OF SHADERS " << shaderSingleton->getNumberOfShaders();
}

void Renderer::Render(Window& window, MyGUI& gui)
{
	glClearColor(0.23f, 0.33f, 0.33f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	auto camera = window.getCamera();
	auto& selectedObjects = gui.getObjectIndex();
	camera.Update();

	glStencilMask(0x00);

	if (gui.getFaceCulling())
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	for (int i = 0; i < objectSingleton->getNumberOfObjects(); i++)
	{
		Object* object = objectSingleton->getObject(i);
		if (std::any_of(selectedObjects.begin(), selectedObjects.end(), [i](int a) {return i == a; }))
		{
			if (gui.getMode() == Mode::OBJECT)
			{
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);
				glPointSize(5.0f);

				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				object->Draw(shaderSingleton->getShader("Basic"), camera, GL_LINES);

				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);
				glDisable(GL_DEPTH_TEST);
				if (i == selectedObjects[selectedObjects.size() - 1])
					object->Draw(shaderSingleton->getShader("ActiveSelect"), camera, GL_LINES);//OUTLINE
				else
					object->Draw(shaderSingleton->getShader("Select"), camera);//OUTLINE

				glStencilMask(0xFF);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glEnable(GL_DEPTH_TEST);
			}
			else if (gui.getMode() == Mode::EDIT)
			{
				object->Draw(shaderSingleton->getShader("Basic"), camera, GL_TRIANGLES);

				object->Draw(shaderSingleton->getShader("EditMode"), camera, GL_LINES);

				if (gui.getSelectMode() == SelectMode::VERTEX)
				{
					glPointSize(5.0f);
					object->Draw(shaderSingleton->getShader("EditMode"), camera, GL_POINTS); // black color

					auto& selectedVertices = static_cast<Mesh*>(object)->getSelectedVertices();

					object->bindVAO();
					auto x = object->getModelReference();

					Shader selectShader = shaderSingleton->getShader("SelectEdit"); // orange color
					selectShader.Activate();
					selectShader.setMat4(true, "model", x);
					camera.CameraUniform(selectShader, "cameraMatrix");

					for (int j = 0; j < selectedVertices.size(); j++)
					{
						if (j == selectedVertices.size() - 1)
						{
							Shader activeShader = shaderSingleton->getShader("ActiveEdit"); // red color
							activeShader.Activate();
							activeShader.setMat4(true, "model", x);
							camera.CameraUniform(activeShader, "cameraMatrix");
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
					camera.CameraUniform(selectShader, "cameraMatrix");

					EBO ebo(edgeVerts);
					ebo.Bind();
					if (edgeVerts.size())
					{
						glDrawElements(GL_LINES, edgeVerts.size() - 2, GL_UNSIGNED_INT, 0); // draw red

						Shader activeShader = shaderSingleton->getShader("ActiveEdit"); // orange color
						activeShader.Activate();
						activeShader.setMat4(true, "model", x);
						camera.CameraUniform(activeShader, "cameraMatrix");
						glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)((edgeVerts.size() - 2) * sizeof(GLuint))); // draw orange

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
						camera.CameraUniform(selectShader, "cameraMatrix");

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
						camera.CameraUniform(activeShader, "cameraMatrix");
						glDrawElements(GL_TRIANGLES, lastFace, GL_UNSIGNED_INT, (void*)((drawFaces.size() - lastFace) * sizeof(GLuint))); // draw orange

						glEnable(GL_CULL_FACE);
					}
				}
			}
		}

		object->Draw(shaderSingleton->getShader("Basic"), camera);
	}
}