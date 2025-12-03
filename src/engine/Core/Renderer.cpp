#include "Renderer.h"

#include "ShaderManager.h"
#include "MyGUI.h"
#include "Mesh/DFace.h"

void Renderer::viewportEditor()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glClearColor(0.23f, 0.33f, 0.33f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	gui.drawGrid3D();

	RenderMode renderMode = app->getRenderMode();

	if (!gui.getFaceCulling() || renderMode == RenderMode::WIREFRAME)
		glDisable(GL_CULL_FACE);
	else
		glEnable(GL_CULL_FACE);


	static Camera* camera = window.getCamera();
	camera->update();



	static Shader& basicShader = shaderSingleton->getShader("Basic");
	auto& selectedObjects = gui.getObjectIndex();
	for (int i = 0; i < objectSingleton->getNumberOfObjects(); i++)
	{
		Object* object = objectSingleton->getObject(i);
		Mesh* mesh = dynamic_cast<Mesh*>(object);
		basicShader.activate();
		// setting model and cameraUniform uniforms may be reduntat bcs they are also set in object.draw method
		basicShader.setMat4(true, "model", object->getModel());
		camera->cameraUniform(true, basicShader, "cameraMatrix");
		if (std::any_of(selectedObjects.begin(), selectedObjects.end(), [i](int a) {return i == a; }))
		{
			if (gui.getMode() == Mode::OBJECT)
			{
				// OUTLINE STENCIL BUFFER TECHNIQUE

				// why doesnt this work properly

				/*glEnable(GL_STENCIL_TEST);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);

				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

				basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::Default));
				object->draw(basicShader, *camera, GL_TRIANGLES);

				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);
				glDisable(GL_DEPTH_TEST);*/
				if (i == selectedObjects.back())
				{
					basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::ActiveSelection));
					object->draw(basicShader, *camera, GL_LINES, true); //OUTLINE
				}
				else
				{
					basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::NonActiveSelection));
					object->draw(basicShader, *camera, GL_LINES, true); //OUTLINE
				}

				/*glStencilMask(0xFF);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glEnable(GL_DEPTH_TEST);*/



			}
			else if (gui.getMode() == Mode::EDIT)
			{
				glLineWidth(2.0f);
				basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::Black));
				mesh->draw(basicShader, *camera, GL_LINES);

				if (gui.getSelectMode() == SelectMode::VERTEX)
				{
					glPointSize(5.0f);
					basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::Black));
					mesh->draw(basicShader, *camera, GL_POINTS);


					mesh->bindVAO();

					basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::NonActiveSelection));

					auto& selectedVertices = mesh->getSelectedVertices();
					for (int j = 0; j < selectedVertices.size(); j++)
					{
						if (j == selectedVertices.size() - 1)
							basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::ActiveSelection));

						glDrawArrays(GL_POINTS, selectedVertices[j], 1);
					}

					glPointSize(1.0f);
				}
				else if (gui.getSelectMode() == SelectMode::EDGE)
				{

					std::vector<DEdge*>& selectedEdges = mesh->getSelectedEdges();
					std::vector<GLuint> edgeVerts = std::vector<GLuint>();
					std::vector<GLuint> seamVerts = std::vector<GLuint>();
					for (auto& x : selectedEdges)
					{
						std::pair<int, int>edgeVertices = mesh->getEdgeIndices(x);
						edgeVerts.push_back(edgeVertices.first);
						edgeVerts.push_back(edgeVertices.second);


					}
					mesh->bindVAO();

					basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::NonActiveSelection));


					EBO ebo(edgeVerts);
					ebo.bind();
					if (edgeVerts.size())
					{
						glDrawElements(GL_LINES, edgeVerts.size() - 2, GL_UNSIGNED_INT, 0); // draw red

						basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::ActiveSelection));
						glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)((edgeVerts.size() - 2) * sizeof(GLuint))); // draw orange

					}
					/////////////////////////////////////////////////////// inneficient for high poly meshes.. inneficient anyways tbh
					for (DEdge* edge : mesh->getAllEdges())
						if (edge->isSeam)
						{
							std::pair<int, int>edgeVertices = mesh->getEdgeIndices(edge);
							seamVerts.push_back(edgeVertices.first);
							seamVerts.push_back(edgeVertices.second);
						}

					if (!seamVerts.empty())
					{
						EBO seamEBO(seamVerts);
						seamEBO.bind();

						basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::Seams));

						glDrawElements(GL_LINES, seamVerts.size(), GL_UNSIGNED_INT, 0);
					}
				}
				else // SelectMode::FACE
				{

					auto drawFaces = mesh->formTrianglesForDrawing();

					std::vector<int> selectedVerts;
					std::vector<DFace*>& selectedFaces = mesh->getSelectedFaces();

					for (auto& x : selectedFaces)
					{
						std::vector<int> temp = mesh->getFaceIndices(x);
						selectedVerts.push_back(x->getVertices().size());
						selectedVerts.insert(selectedVerts.end(), temp.begin(), temp.end());
					}


					if (drawFaces.size())
					{
						mesh->bindVAO();

						EBO ebo(drawFaces);
						ebo.bind();

						int lastFace = selectedVerts[0];
						for (int i = lastFace + 1; i < selectedVerts.size(); i += selectedVerts[i] + 1)
							lastFace = selectedVerts[i];

						lastFace = (lastFace - 2) * 3; // actual number of indices used to make that face


						glDisable(GL_CULL_FACE);

						basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::NonActiveSelection));
						glDrawElements(GL_TRIANGLES, drawFaces.size() - lastFace, GL_UNSIGNED_INT, 0);


						basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::ActiveSelection));
						glDrawElements(GL_TRIANGLES, lastFace, GL_UNSIGNED_INT, (void*)((drawFaces.size() - lastFace) * sizeof(GLuint))); // draw orange

						glEnable(GL_CULL_FACE);
					}
				}
			}
		};
		

		glLineWidth(1.0f);
		basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::Default));
		if (renderMode == RenderMode::WIREFRAME)
		{
			if (mesh)
			{
				basicShader.setInteger(true, "colorMode", static_cast<int>(FragColor::Black));
				mesh->draw(basicShader, *camera, GL_LINES);
			}
			else
				object->draw(basicShader, *camera, GL_TRIANGLES);
		}
		else if (renderMode == RenderMode::SOLID)
		{

			object->draw(basicShader, *camera, GL_TRIANGLES);

		}
		else if (renderMode == RenderMode::MATERIAL_PREVIEW)
		{
			if (mesh)
				mesh->materialDraw(*camera);
			else
				object->draw(basicShader, *camera, GL_TRIANGLES);
		}
		else if (renderMode == RenderMode::RENDER)
		{
			if (mesh)
				mesh->renderDraw(*camera);
			else
				object->draw(basicShader, *camera, GL_TRIANGLES);
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
	camera->update();

	gui.drawGrid2D();

	// ovo treba uljepsat, izbacit VAOve i VBOve te ih staviti u mesh(vjerovatno)
	Mesh* mesh = dynamic_cast<Mesh*>(app->getActiveObject());
	std::vector<std::shared_ptr<UVVertex>>& uvCoords = mesh->getUVCoords();
	std::vector<glm::vec2> uvCoordsVec;
	for (auto x : uvCoords)
	{
		uvCoordsVec.push_back(x->uv);
	}


	static Shader uvShader = shaderSingleton->getShader("UV"); // blue color
	uvShader.activate();
	camera->cameraUniform(true, uvShader, "cameraMatrix");

	VAO uvVAO;
	VBO uvVBO(uvCoordsVec);

	uvVAO.bind();
	uvVBO.bind();

	uvVAO.linkAttribute(uvVBO, 0, 2, GL_FLOAT, sizeof(glm::vec2), (void*)0); // UV coordinates

	glPointSize(6.0f);
	glDrawArrays(GL_POINTS, 0, (GLsizei)uvCoordsVec.size());

	auto& edgeIndices = mesh->getUVEdgeIndices();

	EBO edgeEBO(edgeIndices);

	edgeEBO.bind();
	glDrawElements(GL_LINES, edgeIndices.size(), GL_UNSIGNED_INT, 0);


}

void Renderer::shaderEditor()
{

	glClearColor(0.23f, 0.33f, 0.33f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);



	gui.shaderNodeEditor();

}



Renderer::Renderer(Window& window, MyGUI& gui) :window(window), gui(gui) {}

void Renderer::init() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);

	//std::cout << "NUMBER OF SHADERS " << shaderSingleton->getNumberOfShaders();
}

void Renderer::render()
{

	Mode mode = gui.getMode();
	if (mode == Mode::OBJECT || mode == Mode::EDIT || mode == Mode::SCULPT || mode == Mode::TEXTURE_PAINT || mode == Mode::WEIGHT_PAINT)
		viewportEditor();
	else if (mode == Mode::UV_EDIT)
		uvEditor();
	else if (mode == Mode::SHADER_EDIT)
		shaderEditor();
}






