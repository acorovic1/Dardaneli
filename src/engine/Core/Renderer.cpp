#include "Renderer.h"

#include "ShaderManager.h"
#include "MyGUI.h"
#include "Mesh/DFace.h"

void Renderer::viewportEditor()
{

	glClearColor(0.23f, 0.33f, 0.33f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilMask(0x00);

	gui.drawGrid3D();

	RenderMode renderMode = app->getRenderMode();

	if (!gui.getFaceCulling() || renderMode==RenderMode::WIREFRAME)
		glDisable(GL_CULL_FACE);
	else
		glEnable(GL_CULL_FACE);


	static Camera* camera = window.getCamera();
	camera->update();



	static Shader& basicShader = shaderSingleton->getShader("Basic");
	static Shader& selectShader = shaderSingleton->getShader("SelectEdit"); // orange color
	static Shader& activeShader = shaderSingleton->getShader("ActiveEdit"); // red color
	auto& selectedObjects = gui.getObjectIndex();
	for (int i = 0; i < objectSingleton->getNumberOfObjects(); i++)
	{
		Object* object = objectSingleton->getObject(i);
		Mesh* mesh = dynamic_cast<Mesh*>(object);
		if (std::any_of(selectedObjects.begin(), selectedObjects.end(), [i](int a) {return i == a; }))
		{
			if (gui.getMode() == Mode::OBJECT)
			{
				// OUTLINE STENCIL BUFFER TECHNIQUE


				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);
				glPointSize(5.0f);


				object->draw(shaderSingleton->getShader("Basic"), *camera, GL_LINES);

				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);
				glDisable(GL_DEPTH_TEST);
				if (i == selectedObjects[selectedObjects.size() - 1])
					object->draw(shaderSingleton->getShader("ActiveSelect"), *camera, GL_LINES);//OUTLINE
				else
					object->draw(shaderSingleton->getShader("Select"), *camera, GL_LINES);//OUTLINE

				glStencilMask(0xFF);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glEnable(GL_DEPTH_TEST);
			}
			else if (gui.getMode() == Mode::EDIT)
			{

				mesh->draw(shaderSingleton->getShader("EditMode"), *camera, GL_LINES);

				if (gui.getSelectMode() == SelectMode::VERTEX)
				{
					glPointSize(5.0f);
					mesh->draw(shaderSingleton->getShader("EditMode"), *camera, GL_POINTS); // black color

					auto& selectedVertices = mesh->getSelectedVertices();

					mesh->bindVAO();
					const glm::mat4& model = mesh->getModel();


					selectShader.activate();
					selectShader.setMat4(true, "model", model);
					camera->cameraUniform(true, selectShader, "cameraMatrix");

					for (int j = 0; j < selectedVertices.size(); j++)
					{
						if (j == selectedVertices.size() - 1)
						{
							activeShader.activate();
							activeShader.setMat4(true, "model", model);
							camera->cameraUniform(true, activeShader, "cameraMatrix");
						}
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
					const glm::mat4& model = mesh->getModel();


					selectShader.activate();
					selectShader.setMat4(true, "model", model);
					camera->cameraUniform(true, selectShader, "cameraMatrix");

					EBO ebo(edgeVerts);
					ebo.bind();
					if (edgeVerts.size())
					{
						glDrawElements(GL_LINES, edgeVerts.size() - 2, GL_UNSIGNED_INT, 0); // draw red

						activeShader.activate();
						activeShader.setMat4(true, "model", model);
						camera->cameraUniform(true, activeShader, "cameraMatrix");
						glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)((edgeVerts.size() - 2) * sizeof(GLuint))); // draw orange

					}

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

						static Shader& seamShader = shaderSingleton->getShader("SelectEdit");
						// pauk
						seamShader.activate();
						seamShader.setMat4(true, "model", model);
						camera->cameraUniform(true, seamShader, "cameraMatrix");

						//std::cout << "\n\n\t seamVerts size = "<<seamVerts.size();

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
						const glm::mat4& x = mesh->getModel();
						
						selectShader.activate();
						selectShader.setMat4(true, "model", x);
						camera->cameraUniform(true, selectShader, "cameraMatrix");

						EBO ebo(drawFaces);
						ebo.bind();

						int lastFace = selectedVerts[0];
						for (int i = lastFace + 1; i < selectedVerts.size(); i += selectedVerts[i] + 1)
						{
							lastFace = selectedVerts[i];
						}

						lastFace = (lastFace - 2) * 3; // actual number of indices used to make that face


						glDisable(GL_CULL_FACE);

						glDrawElements(GL_TRIANGLES, drawFaces.size() - lastFace, GL_UNSIGNED_INT, 0); 


						activeShader.activate();
						activeShader.setMat4(true, "model", x);
						camera->cameraUniform(true, activeShader, "cameraMatrix");
						glDrawElements(GL_TRIANGLES, lastFace, GL_UNSIGNED_INT, (void*)((drawFaces.size() - lastFace) * sizeof(GLuint))); // draw orange

						glEnable(GL_CULL_FACE);
					}
				}
			}
		};



		if (renderMode == RenderMode::WIREFRAME)
		{
			if (mesh)
				mesh->draw(basicShader, *camera, GL_LINES);
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


	Shader selectShader = shaderSingleton->getShader("UV"); // blue color
	selectShader.activate();
	camera->cameraUniform(true, selectShader, "cameraMatrix");

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






