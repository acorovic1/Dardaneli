#include "Renderer.h"

#include "ShaderManager.h"
#include "MyGUI.h"

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
					auto& selectedEdges = static_cast<Mesh*>(object)->getSelectedEdges();
					std::vector<GLuint> edges = std::vector<GLuint>();
					edges.insert(edges.end(), selectedEdges.begin(), selectedEdges.end());
					object->bindVAO();
					auto x = object->getModelReference();

					Shader selectShader = shaderSingleton->getShader("SelectEdit"); // red color
					selectShader.Activate();
					selectShader.setMat4(true, "model", x);
					camera.CameraUniform(selectShader, "cameraMatrix");

					EBO ebo(edges);
					ebo.Bind();
					if (edges.size())
					{
						glDrawElements(GL_LINES, edges.size() - 2, GL_UNSIGNED_INT, 0); // draw red

						Shader activeShader = shaderSingleton->getShader("ActiveEdit"); // orange color
						activeShader.Activate();
						activeShader.setMat4(true, "model", x);
						camera.CameraUniform(activeShader, "cameraMatrix");
						glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)((edges.size() - 2) * sizeof(GLuint))); // draw orange
						
					}
				}
				else // SelectMode::FACE
				{
					auto selectedFaces = static_cast<Mesh*>(object)->formTrianglesForDrawing();

					auto temp = static_cast<Mesh*>(object)->getSelectedFaces();
					

					object->bindVAO();
					auto x = object->getModelReference();


					Shader selectShader = shaderSingleton->getShader("SelectEdit"); // red color
					selectShader.Activate();
					selectShader.setMat4(true, "model", x);
					camera.CameraUniform(selectShader, "cameraMatrix");

					EBO ebo(selectedFaces);
					ebo.Bind();
					if (selectedFaces.size())
					{
						int lastFace = temp[0];
						for (int i = lastFace + 1; i < temp.size(); i += temp[i] + 1)
						{
							lastFace = temp[i];
						}

						lastFace = (lastFace - 2) * 3; // actual number of indices used to make that face

						auto temp = static_cast<Mesh*>(object)->getSelectedFaces().back();



						glDrawElements(GL_TRIANGLES, selectedFaces.size()- lastFace, GL_UNSIGNED_INT, 0); // draw red

						Shader activeShader = shaderSingleton->getShader("ActiveEdit"); // orange color
						activeShader.Activate();
						activeShader.setMat4(true, "model", x);
						camera.CameraUniform(activeShader, "cameraMatrix");
						glDrawElements(GL_TRIANGLES, lastFace, GL_UNSIGNED_INT, (void*)((selectedFaces.size() - lastFace) * sizeof(GLuint))); // draw orange

					}
				}
			}
		}

		object->Draw(shaderSingleton->getShader("Basic"), camera);
	}
}