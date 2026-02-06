#define NOMINMAX


#include "Mesh/Mesh.h"
#include "Mesh/DFace.h"
#include "Mesh/DLoop.h"
#include "Mesh/DEdge.h"
#include "Mesh/DVertex.h"

#include "ObjectModeBVH.h"
#include "VertexBVH.h"
#include "EdgeBVH.h"
#include "FaceBVH.h"

#include "Application.h"
#include "CameraManager.h"
#include "MaterialManager.h"
#include "UnorderedPair.h"

#include "Lights/Light.h"
#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"
#include "Lights/SpotLight.h"

#include "Material.h"
#include "Loader.h"

#include "igl/lscm.h"
#include "igl/boundary_loop.h"

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>  // this includes the definition of hash for glm types

#include "unordered_map"
#include "queue"
#include <chrono>
#include <Improved/ObjectModeBVHImproved.h>

Mesh::Mesh(std::string&& name, std::vector <DVertex*> vertices,
	std::vector <GLuint>& indices, const std::vector<GLuint>& edgeIndices, const  std::vector <Texture>& textures) :Object(name) {
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::edgeIndices = edgeIndices;


	vao.bind();
	vbo.bufferData(Mesh::vertices);
	ebo.bufferData(Mesh::indices);
	edgeEBO.bufferData(Mesh::edgeIndices);

	vao.linkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(DVertex), (void*)0); //position
	vao.linkAttribute(vbo, 1, 3, GL_FLOAT, sizeof(DVertex), (void*)(3 * sizeof(float))); //normal

	vao.unbind();
	vbo.unbind();
	ebo.unbind();

	//objectBVHSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());

	objectBVHImprovedSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());

}

Mesh::Mesh(const char* file) :Object("objLoad")
{
	using Clock = std::chrono::high_resolution_clock;

	auto start = Clock::now();
	Loader::obj(file, vertices, indices, edgeIndices);
	auto end = Clock::now();

	std::chrono::duration<double, std::milli> ms = end - start;

	std::cout << "\nLoaded mesh from " << file << " with " << vertices.size() << " vertices and " << indices.size() / 3 << " faces in " << ms.count() << " ms\n";;

	vao.bind();
	vbo.bufferData(Mesh::vertices);
	ebo.bufferData(Mesh::indices);
	edgeEBO.bufferData(Mesh::edgeIndices);

	vao.linkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(DVertex), (void*)0); //position
	vao.linkAttribute(vbo, 1, 3, GL_FLOAT, sizeof(DVertex), (void*)(3 * sizeof(float))); //normal

	vao.unbind();
	vbo.unbind();
	ebo.unbind();

	start = Clock::now();

	objectBVHImprovedSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
	//objectBVHSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
	end = Clock::now();
	ms = end - start;
	std::cout << "Built BVH for " << file << " in " << ms.count() << " ms\n";;

}

Mesh::~Mesh() {}


void Mesh::translate(glm::vec3& translateVector)
{
	position += translateVector;
	fillModel();


}
void Mesh::translate(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;

	fillModel();
}

void Mesh::rotate(float degrees, const glm::vec3& axisVector)
{
	rotation = glm::rotate(rotation, glm::radians(degrees), axisVector);

	fillModel();
}

void Mesh::scale(glm::vec3& scaleVector)
{
	scaling *= scaleVector;
	fillModel();
}
void Mesh::scale(float x, float y, float z) {
	scaling.x *= x;
	scaling.y *= y;
	scaling.z *= z;

	fillModel();
}




void Mesh::bindEBO() { ebo.bind(); }
void Mesh::updateEBO() { ebo.bufferData(indices); }

void Mesh::updateEdgeEBO()
{
	edgeEBO.bufferData(edgeIndices);
}

std::unordered_set<DFace*> Mesh::getAllFaces()
{
	std::unordered_set<DFace*> returnSet;

	for (auto x : getAllEdges())
	{
		if (!x->loop)
			continue;

		//std::cout << "\n\ngetAllFaces";
		DLoop* l = x->loop;
		do {

			//std::cout << "\t\t\thii";

			returnSet.insert(l->face);

			if (l == l->radialNext)break;

			l = l->radialNext;


		} while (l != x->loop);
		//returnSet.insert(x->loop->face);



	}

	returnSet.erase(nullptr);


	return returnSet;
}



std::unordered_set<DEdge*> Mesh::getAllEdges()
{

	//std::cout << "\n\t\tMesh.getAllEdges\tVertices size " << vertices.size();
	std::unordered_set<DEdge*> returnSet;
	for (auto x : vertices)
	{
		//std::cout <<"\n\n\t"<< x->position.x << " " << x->position.y << " " << x->position.z << "\n";
		//std::cout << "\nVertex with index " << this->getVertexIndex(x);
		std::unordered_set<DEdge*> temp = x->getAdjecentEdges();
		//std::cout << "\t\tfinished ";
		returnSet.insert(temp.begin(), temp.end());
	}

	returnSet.erase(nullptr);

	return returnSet;
}

DFace* Mesh::getFace(std::unordered_set<int> indices)
{
	if (indices.size() < 3)return nullptr;

	DVertex* vert = vertices[*indices.begin()];

	for (DFace* face : vert->getAdjecentFaces())
	{

		bool found = true;
		for (auto v : face->getVertices())
			if (!indices.count(this->getVertexIndex(v)))
			{
				found = false;
				break;
			}

		if (found)return face;
	}

	std::cerr << "\n\n\n Mesh.getFace(indices) returns nullptr\n\n";
	return nullptr;
}

DEdge* Mesh::getEdge(int start, int end)
{
	DVertex* v1 = vertices[start];
	DVertex* v2 = vertices[end];

	DEdge* edge = v1->e;

	if (edge)
		do {
			if (edge->v1 == v1)
			{
				if (edge->v2 == v2)
					return edge;

				edge = edge->d1.next;
			}
			else // edge->v2 == v1
			{
				if (edge->v1 == v2)
					return edge;

				edge = edge->d2.next;
			}
		} while (edge != v1->e);

	//std::cerr << "\n\n\n Mesh.getEdge(start,end) returns nullptr\n " << start << " " << end << "\n\n";
	return nullptr;
}

DEdge* Mesh::getEdge(DVertex* start, DVertex* end)
{

	DEdge* edge = start->e;

	if (edge)
		do {
			if (edge->v1 == start)
			{
				if (edge->v2 == end)
					return edge;

				edge = edge->d1.next;
			}
			else // edge->v2 == v1
			{
				if (edge->v1 == end)
					return edge;

				edge = edge->d2.next;
			}
		} while (edge != start->e);


	return nullptr;
}





void Mesh::materialDraw(Camera& camera)
{

	for (auto& buff : renderBuffers)
	{
		Material* mat = buff.first;
		VAO& matVAO = std::get<0>(buff.second);
		VBO& matVBO = std::get<1>(buff.second);
		EBO& matEBO = std::get<2>(buff.second);
		//std::vector<GPUVertex>& matVerts = std::get<3>(buff.second);
		std::vector<GLuint >& matIndices = std::get<4>(buff.second);

		// may be a redundant call??
		auto shaderr = *mat->compileShader(RenderMode::MATERIAL_PREVIEW).get();

		shaderr.activate();
		camera.cameraUniform(true, shaderr, "cameraMatrix");
		shaderr.setVector3f(true, "camPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
		shaderr.setMat4(true, "model", model);
		if (mat->textures.size())
		{
			for (auto& tex : mat->textures)
			{
				tex.get()->textureUniform(shaderr, ("tex" + std::to_string(tex.get()->ID)).c_str(), tex.get()->unit);
				tex.get()->bind();

			}
		}


		matVAO.bind();
		matEBO.bind();

		glDrawElements(GL_TRIANGLES, matIndices.size(), GL_UNSIGNED_INT, 0);
		matEBO.unbind();
		matVAO.unbind();

	}


	if (renderBuffers.size() == 0)
		draw(shaderSingleton->getShader("Basic"), camera, GL_TRIANGLES);
}

void Mesh::renderDraw(Camera& camera)
{
	for (auto x : vertices)
		x->normal = glm::normalize(x->position);




	struct GPULight {
		int type;
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 color;
		float intensity;
		float innerCutoff;
		float outerCutoff;
	};

	GPULight gpuLights[32];
	int activeLights = 0;

	for (Object* l : objectSingleton->getAllObjects())
	{
		GPULight g{};
		if (auto* d = dynamic_cast<DirectionalLight*>(l)) {
			g.type = 0;
			g.direction = d->getDirection();
			g.color = d->getColor();
			g.intensity = d->getIntensity();
		}
		else if (auto* p = dynamic_cast<PointLight*>(l)) {
			g.type = 1;
			g.position = p->getPosition();
			g.color = p->getColor();
			g.intensity = p->getIntensity();
		}
		else if (auto* s = dynamic_cast<SpotLight*>(l)) {
			g.type = 2;
			g.position = s->getPosition();
			g.direction = s->getDirection();
			g.color = s->getColor();
			g.intensity = s->getIntensity();
			g.innerCutoff = glm::cos(glm::radians(s->getInnerCutoff()));
			g.outerCutoff = glm::cos(glm::radians(s->getOuterCutoff()));
		}

		if (activeLights < 32)
			gpuLights[activeLights++] = g;
	}


	for (auto& buff : renderBuffers)
	{
		Material* mat = buff.first;
		VAO& matVAO = std::get<0>(buff.second);
		VBO& matVBO = std::get<1>(buff.second);
		EBO& matEBO = std::get<2>(buff.second);
		//std::vector<GPUVertex>& matVerts = std::get<3>(buff.second);
		std::vector<GLuint >& matIndices = std::get<4>(buff.second);

		// may be a redundant call??
		auto shaderr = *mat->compileShader(RenderMode::RENDER).get();

		shaderr.activate();
		camera.cameraUniform(true, shaderr, "cameraMatrix");
		shaderr.setVector3f(true, "camPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
		shaderr.setMat4(true, "model", model);
		if (mat->textures.size())
		{
			for (auto& tex : mat->textures)
			{
				tex.get()->textureUniform(shaderr, ("tex" + std::to_string(tex.get()->ID)).c_str(), tex.get()->unit);
				tex.get()->bind();

			}
		}

		shaderr.setInteger(true, "numLights", activeLights);

		for (int i = 0; i < activeLights; ++i) {
			std::string prefix = "lights[" + std::to_string(i) + "].";

			shaderr.setInteger(true, (prefix + "type").c_str(), gpuLights[i].type);
			shaderr.setVector3f(true, (prefix + "position").c_str(), gpuLights[i].position.x, gpuLights[i].position.y, gpuLights[i].position.z);
			shaderr.setVector3f(true, (prefix + "direction").c_str(), gpuLights[i].direction.x, gpuLights[i].direction.y, gpuLights[i].direction.z);
			shaderr.setVector3f(true, (prefix + "color").c_str(), gpuLights[i].color.x, gpuLights[i].color.y, gpuLights[i].color.z);
			shaderr.setFloat(true, (prefix + "intensity").c_str(), gpuLights[i].intensity);
			shaderr.setFloat(true, (prefix + "innerCutoff").c_str(), gpuLights[i].innerCutoff);
			shaderr.setFloat(true, (prefix + "outerCutoff").c_str(), gpuLights[i].outerCutoff);
		}


		matVAO.bind();
		matEBO.bind();

		glDrawElements(GL_TRIANGLES, matIndices.size(), GL_UNSIGNED_INT, 0);
		matEBO.unbind();
		matVAO.unbind();

	}

	if (renderBuffers.size() == 0)
		draw(shaderSingleton->getShader("Basic"), camera, GL_TRIANGLES);



}

void Mesh::buildGPUVertices()
{

	for (auto& it : renderBuffers)
	{
		Material* mat = it.first;
		VAO& matVAO = std::get<0>(it.second);
		VBO& matVBO = std::get<1>(it.second);
		EBO& matEBO = std::get<2>(it.second);
		std::vector<GPUVertex>& matVerts = std::get<3>(it.second);
		std::vector<GLuint >& matIndices = std::get<4>(it.second);

		matVerts.clear();
		matIndices.clear();

		for (DFace* face : materials[mat])
		{
			std::vector<GLuint> tempHelper;
			for (DLoop* loop : face->getLoopsVector())
			{
				GPUVertex gpv;
				gpv.position = loop->tip->position;
				gpv.normal = loop->tip->normal;
				gpv.uv = loop->uvVertex->uv;



				//std::cout << "\n\t" << gpv.position.x << " " << gpv.position.y << " " << gpv.position.z << "  |  ";

				tempHelper.push_back(matVerts.size());
				matVerts.push_back(gpv);

			}
			// only quads for now
			matIndices.push_back(tempHelper[1]);
			matIndices.push_back(tempHelper[2]);
			matIndices.push_back(tempHelper[0]);

			matIndices.push_back(tempHelper[0]);
			matIndices.push_back(tempHelper[2]);
			matIndices.push_back(tempHelper[3]);

		}

		matVAO.bind();
		matVBO.bufferData(matVerts);
		matEBO.bufferData(matIndices);

	}



}

void Mesh::addMaterial(Material* mat)
{
	if (materials.size() == 0)
	{
		materials.insert({ mat,getAllFaces() });
		buildGPUVertices();
	}
	else
		materials.insert({ mat,{} });

}

void Mesh::assignMaterial(Material* mat)
{
	// if there were no materials, assign this material to all faces
	if (materials.size() == 0)
	{
		materials.insert({ mat,getAllFaces() });

	}
	else
	{



		for (auto face : selectedFaces)
		{
			for (auto& it : materials)
			{
				it.second.erase(face);
			}
			materials[mat].insert(face);
		}

	}
	renderBuffers.emplace(
		mat,
		std::make_tuple(VAO{}, VBO{}, EBO{}, std::vector<GPUVertex>{}, std::vector<GLuint>{})
	);

	VAO& matVAO = std::get<0>(renderBuffers[mat]);
	VBO& matVBO = std::get<1>(renderBuffers[mat]);

	matVAO.bind();
	matVAO.linkAttribute(matVBO, 0, 3, GL_FLOAT, sizeof(GPUVertex), (void*)0); //position
	matVAO.linkAttribute(matVBO, 1, 3, GL_FLOAT, sizeof(GPUVertex), (void*)(3 * sizeof(float))); //normal
	matVAO.linkAttribute(matVBO, 2, 2, GL_FLOAT, sizeof(GPUVertex), (void*)(6 * sizeof(float))); //uvs

	matVAO.unbind();

	buildGPUVertices();
}

void Mesh::removeMaterial(Material* mat)
{
	materials.erase(mat);
	renderBuffers.erase(mat);

}

void Mesh::draw(Shader& shader, Camera& camera, GLenum mode, bool outline) {


	shader.activate();
	vao.bind();
	camera.cameraUniform(true, shader, "cameraMatrix");
	shader.setVector3f(true, "camPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	if (outline)
		shader.setMat4(true, "model", glm::scale(model, glm::vec3(1.03f)));
	else
		shader.setMat4(true, "model", model);

	if (mode == GL_TRIANGLES)
	{
		ebo.bind();
		glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, 0);

	}
	else if (mode == GL_LINES)
	{
		edgeEBO.bind();
		glDrawElements(mode, edgeIndices.size(), GL_UNSIGNED_INT, 0);
	}
	else if (mode == GL_POINTS)
	{
		vbo.bind();
		glDrawArrays(mode, 0, vertices.size());
	}

	vao.unbind();
}


void Mesh::edgeScale(DEdge* edge, float delta, bool update)
{


	DVertex* v1 = edge->v1;
	DVertex* v2 = edge->v2;

	glm::vec3 dir = glm::normalize(v1->position - v2->position);

	v1->position += delta * dir;
	v2->position -= delta * dir;

	//std::cout << "\ni am here "<<v1->position.x;
	std::cout << "\n\n\tMesh.edgeScale " << v1->position.x << " " << v1->position.y << " " << v1->position.z;
	std::cout << "\n\n\tMesh.edgeScale " << v2->position.x << " " << v2->position.y << " " << v2->position.z;
	std::cout << "\n\n\tMesh.edgeScale delta " << delta;
	std::cout << "\n\n\tMesh.edgeScale dir " << dir.x << " " << dir.y << " " << dir.z;
	std::cout << "\n\n\n ";

	if (!update)return;


	vbo.bufferData(vertices);

	EdgeBVHSingleton->Refit(*this);


}

void Mesh::inset(std::vector<DFace*> faces)
{
	return; //

	// mnogo zajebana operacija!!
	// 
	// 
	// find outer rim

	// delete everything except vertices from the inside of the rim

	// extrude outer rim inwards, just like in the individual inset

	// get directions of new edges and find all the vertices that lie in that direction (use epsilon )
	// closest vertex is the one that is needed

	// one edge, if two vertices find a vertex each, fill those

	// if only one vertex finds a corresponding vertex, check its neighbouring edge 2nd vertex, they form a corner

	// if no vertices find a corresponding vertex, its probably a standalone face, find its vertices and fill 


	// all selected edges
	std::unordered_map<DEdge*, int> edges;

	std::unordered_set<DEdge*> outerEdges;
	std::unordered_set<DEdge*> innerEdges;

	// outer edges have int=1, inner edges have more
	for (auto face : faces)
	{

		for (auto edge : face->getEdges())
		{
			auto it = edges.find(edge);

			if (it == edges.end())
			{
				edges.insert({ edge,1 });
			}
			else it->second += 1;
		}
	}

	// separate inner and outer edges;
	for (auto edge : edges)
	{
		if (edge.second == 1)
			outerEdges.insert(edge.first);
		else
			innerEdges.insert(edge.first);

	}

	// vertices to check if they lie on the specified direction
	std::unordered_set<DVertex*> middleVertices;
	for (auto innerEdge : innerEdges)
	{
		DVertex* v1 = innerEdge->v1;
		DVertex* v2 = innerEdge->v2;
		if (!std::count_if(outerEdges.begin(), outerEdges.end(), [&v1](DEdge* outerEdge)
			{
				return outerEdge->v1 == v1 || outerEdge->v2 == v1;

			}))
			middleVertices.insert(v1);

		if (!std::count_if(outerEdges.begin(), outerEdges.end(), [&v2](DEdge* outerEdge)
			{
				return outerEdge->v1 == v2 || outerEdge->v2 == v2;

			}))
			middleVertices.insert(v2);
	}
	deleteOnlyEdgesAndFaces(innerEdges);

	// extrude edges
	std::unordered_set<DEdge*> extrudedEdges = extrudeEdges(outerEdges);

	std::unordered_map<DVertex*, glm::vec3> vertexDirection;

	for (DEdge* edge : extrudedEdges)
	{

		if (!vertexDirection.count(edge->v1))
			for (auto vertEdges : edge->v1->getAdjecentEdges())
			{
				// if its the directedEdge
				if (!std::count(extrudedEdges.begin(), extrudedEdges.end(), vertEdges))
				{
					if (edge->v1 == vertEdges->v1)
					{
						vertexDirection.insert({ edge->v1,glm::normalize(edge->v1->position - vertEdges->v2->position) });
					}
					else if (edge->v1 == vertEdges->v2)
					{
						vertexDirection.insert({ edge->v1,glm::normalize(edge->v1->position - vertEdges->v1->position) });
					}
					else std::cout << "\n\n\t ERROR mesh.inset --- problem with finding directedEdge 1";
				}
			}

		if (!vertexDirection.count(edge->v2))
			for (auto vertEdges : edge->v2->getAdjecentEdges())
			{
				// if its the directedEdge
				if (!std::count(extrudedEdges.begin(), extrudedEdges.end(), vertEdges))
				{
					if (edge->v2 == vertEdges->v1)
					{
						vertexDirection.insert({ edge->v2,glm::normalize(edge->v2->position - vertEdges->v2->position) });
					}
					else if (edge->v2 == vertEdges->v2)
					{
						vertexDirection.insert({ edge->v2,glm::normalize(edge->v2->position - vertEdges->v1->position) });
					}
					else std::cout << "\n\n\t ERROR mesh.inset --- problem with finding directedEdge 2";
				}
			}

	}

	bool v1Found, v2Found;
	std::vector<int>fillIndices;
	// check for colinear verts
	for (DEdge* edge : extrudedEdges)
	{
		DVertex* v1 = edge->v1;
		DVertex* v2 = edge->v2;

		v1Found = false;
		v2Found = false;

		fillIndices.push_back(getVertexIndex(v1));
		fillIndices.push_back(getVertexIndex(v2));

		for (DVertex* middleVertex : middleVertices)
		{

			if (vertexDirection[v1] == glm::normalize(middleVertex->position - v1->position) && !v1Found)
			{
				v1Found = true;
				fillIndices.push_back(getVertexIndex(middleVertex));
			}
			if (vertexDirection[v2] == glm::normalize(middleVertex->position - v2->position) && !v2Found)
			{
				v2Found = true;
				fillIndices.push_back(getVertexIndex(middleVertex));
			}
		}

		if (v1Found && v2Found)
		{
			faceFill(fillIndices);
			continue;
		}

		if (!v1Found && !v2Found);

		if (v1Found)
		{
			DEdge* nextEdge = edge->d2.next;
			while (!std::count(extrudedEdges.begin(), extrudedEdges.end(), nextEdge))
			{
				if (nextEdge->v1 == edge->v2)
					nextEdge = nextEdge->d1.next;
				else if (nextEdge->v2 == edge->v2)
					nextEdge = nextEdge->d2.next;
				else std::cout << "\n\n\t ERROR Mesh.inset.. v1Found";
			}

			DVertex* otherVertex = (nextEdge->v1 == edge->v2) ? nextEdge->v2 : nextEdge->v1;

			for (DVertex* middleVertex : middleVertices)
			{

				if (vertexDirection[otherVertex] == glm::normalize(middleVertex->position - otherVertex->position))
				{
					fillIndices.push_back(getVertexIndex(middleVertex));
					fillIndices.push_back(getVertexIndex(otherVertex));
					break;
				}

			}

		}
	}

}

void Mesh::insetIndividual(std::vector<DFace*> faces)
{
	selectedVertexIndices.clear();
	this->getSelectedEdges().clear();
	this->getSelectedFaces().clear();

	for (DFace* face : faces)
	{
		std::unordered_set<DEdge*>  edges = face->getEdges();
		eraseFace(face);


		std::unordered_set<int> vertIndices;

		for (auto edge : extrudeEdges(edges))
		{
			vertIndices.insert(getVertexIndex(edge->v1));
			vertIndices.insert(getVertexIndex(edge->v2));
			selectedEdges.push_back(edge);
		}

		std::vector<int> vertIndicesVec{ vertIndices.begin(),vertIndices.end() };
		faceFill(vertIndicesVec);


		selectedVertexIndices.insert(selectedVertexIndices.begin(), vertIndices.begin(), vertIndices.end());
	}

	this->updateEBO();
	this->updateEdgeEBO();
	vbo.bufferData(vertices);

	//this->getSelectedVertices().clear();


	FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	VertexBVHSingleton->BuildBottomUp(*this);

}

std::unordered_set<DVertex*> Mesh::linearSubdivision()
{
	double time = glfwGetTime();
	std::unordered_map<DFace*, std::pair<std::vector<DVertex*>, FaceType>> facesToFill;

	// create vertices at center of quads
	for (DFace* face : selectedFaces)
	{

		auto faceVerts = face->getVerticesVector();

		int size = faceVerts.size();

		if (size == 3)
			facesToFill.insert({ face,{faceVerts,FaceType::TRI} });
		else if (size == 4)
		{
			// vertex at the center of the quad
			glm::vec3 vertexPos(0.0f);
			for (DVertex* v : faceVerts)
			{
				vertexPos += v->position;
			}
			vertices.push_back(new DVertex(0.25f * vertexPos));
			faceVerts.push_back(vertices.back());
			facesToFill.insert({ face,{faceVerts,FaceType::QUAD} });
		}
		else
			facesToFill.insert({ face,{faceVerts,FaceType::NGON} });

	}

	for (DEdge* edge : selectedEdges)
	{

		vertices.push_back(new DVertex((edge->v1->position + edge->v2->position) / 2.0f));

		auto faces = edge->getFaces();

		if (faces.size())
			for (DFace* face : faces)
			{
				if (!facesToFill.count(face))
				{
					auto temp = face->getVerticesVector();
					int size = temp.size();
					if (size == 3)
						facesToFill.insert({ face,{temp,FaceType::TRI} });
					else if (size == 4)
						facesToFill.insert({ face,{temp,FaceType::QUAD} });
					else
						facesToFill.insert({ face,{temp,FaceType::NGON} });

				}

				facesToFill[face].first.push_back(vertices.back());

			}
		else
		{
			new DEdge(edge->v1, vertices.back());
			new DEdge(edge->v2, vertices.back());

			edgeIndices.push_back(getVertexIndex(edge->v1));
			edgeIndices.push_back(vertices.size() - 1);
			edgeIndices.push_back(getVertexIndex(edge->v2));
			edgeIndices.push_back(vertices.size() - 1);

			eraseEdge(edge, false);
		}

	}

	deleteOnlyEdgesAndFaces(selectedEdges);
	for (auto it : facesToFill)
	{
		std::vector<int>fillVec;
		for (auto vert : it.second.first)
			fillVec.push_back(getVertexIndex(vert));

		FaceType type = it.second.second;


		int size = fillVec.size();

		if (size == 4)
		{
			faceFill(fillVec);
		}
		else if (size == 5)
		{
			if (type == FaceType::TRI)
			{

				DVertex* vert = GeometryUtils::findClosestVertex({ vertices[fillVec[0]],vertices[fillVec[1]],vertices[fillVec[2]] },
					vertices[fillVec[3]], vertices[fillVec[4]]);
				if (vert)
				{

					int closestVert = getVertexIndex(vert);

					// triangle
					std::vector<int> tempFillVec{ fillVec[3],fillVec[4],closestVert };
					faceFill(tempFillVec);

					// quad
					fillVec.erase(std::remove(fillVec.begin(), fillVec.end(), closestVert), fillVec.end());
					faceFill(fillVec);
				}

			}
			else if (type == FaceType::QUAD)
			{

				faceFill(fillVec);
			}
		}
		else if (size == 6)
		{
			if (type == FaceType::TRI)
			{

				// one
				int closestVert = getVertexIndex(GeometryUtils::findClosestVertex({ vertices[fillVec[0]],vertices[fillVec[1]],vertices[fillVec[2]] },
					vertices[fillVec[3]], vertices[fillVec[4]]));

				std::vector<int> tempFillVec{ fillVec[3],fillVec[4],closestVert };
				faceFill(tempFillVec);


				// two
				closestVert = getVertexIndex(GeometryUtils::findClosestVertex({ vertices[fillVec[0]],vertices[fillVec[1]],vertices[fillVec[2]] },
					vertices[fillVec[3]], vertices[fillVec[5]]));

				tempFillVec = { fillVec[3],fillVec[5],closestVert };
				faceFill(tempFillVec);

				// three
				closestVert = getVertexIndex(GeometryUtils::findClosestVertex({ vertices[fillVec[0]],vertices[fillVec[1]],vertices[fillVec[2]] },
					vertices[fillVec[4]], vertices[fillVec[5]]));

				tempFillVec = { fillVec[4],fillVec[5],closestVert };
				faceFill(tempFillVec);


				// middle
				tempFillVec = { fillVec[3],fillVec[4],fillVec[5] };
				faceFill(tempFillVec);


			}
			else if (type == FaceType::QUAD)
			{
				std::cout << "\n\n\t size = 6 -- QUAD \n";
				DVertex* closestVert = GeometryUtils::findClosestVertex({ vertices[fillVec[0]],vertices[fillVec[1]],vertices[fillVec[2]],vertices[fillVec[3]] },
					vertices[fillVec[4]], vertices[fillVec[5]]);

				if (closestVert)
				{


					// triangle
					std::vector<int> tempFillVec{ fillVec[4],fillVec[5],getVertexIndex(closestVert) };
					faceFill(tempFillVec);

					// quad
					fillVec.erase(std::remove(fillVec.begin(), fillVec.end(), getVertexIndex(closestVert)), fillVec.end());
					faceFill(fillVec);
				}
				else
				{
					std::vector<DVertex*> out1;
					std::vector<DVertex*> out2;
					GeometryUtils::splitQuadAlongMidpointsOpposite({ vertices[fillVec[0]],vertices[fillVec[1]],vertices[fillVec[2]],vertices[fillVec[3]] },
						vertices[fillVec[4]], vertices[fillVec[5]], out1, out2);

					fillVec.clear();
					for (auto x : out1)
						fillVec.push_back(getVertexIndex(x));
					faceFill(fillVec);

					fillVec.clear();
					for (auto x : out2)
						fillVec.push_back(getVertexIndex(x));
					faceFill(fillVec);
				}
			}
			else // type == FaceType::NGON
			{
				faceFill(fillVec);
			}
		}
		else if (size == 7)
		{
			if (type == FaceType::QUAD)
			{
				std::vector<DVertex*>temp{ vertices[fillVec[0]], vertices[fillVec[1]], vertices[fillVec[2]],vertices[fillVec[3]] };
				DVertex* a = nullptr, * b = nullptr;
				int midA, midB;
				for (int i = 4;i < 7;i++)
				{
					int index = (i == 6) ? 4 : i + 1;
					DVertex* v = GeometryUtils::findClosestVertex(temp, vertices[fillVec[i]], vertices[fillVec[index]]);

					if (!v)
					{
						midA = fillVec[i];
						midB = fillVec[index];
						continue;
					}

					if (!a)
						a = v;
					else b = v;

					std::vector<int>fillTemp{ getVertexIndex(v),fillVec[i],fillVec[index] };

					faceFill(fillTemp);

				}

				fillVec = { fillVec[4],fillVec[5],fillVec[6] };
				faceFill(fillVec);


				std::vector<DVertex*> tempTemp;
				for (auto x : temp)
					if (x != a && x != b) tempTemp.push_back(x);

				fillVec = { midA,midB,getVertexIndex(tempTemp[0]),getVertexIndex(tempTemp[1]) };
				faceFill(fillVec);

			}
			else // type == FaceType::NGON
			{
				faceFill(fillVec);
			}
		}
		else if (size == 8)
		{
			// NGON only
			faceFill(fillVec);
		}
		else if (size == 9)
		{
			if (type == FaceType::QUAD)
			{

				std::vector<DVertex*> originalFour{ vertices[fillVec[0]],vertices[fillVec[1]],vertices[fillVec[2]],vertices[fillVec[3]] };
				int midPoint = getVertexIndex(vertices[fillVec[4]]);
				std::vector<int> other{ fillVec[5],fillVec[6],fillVec[7],fillVec[8] };

				setWindingOrder(other);

				for (int i = 0;i < 4;i++)
				{

					int index = (i == 3) ? 0 : i + 1;
					DVertex* v = GeometryUtils::findClosestVertex(originalFour, vertices[other[i]], vertices[other[index]]);

					std::vector<int> helpFill{ midPoint,other[i],other[index],getVertexIndex(v) };

					faceFill(helpFill);

				}

			}
			else // type == FaceType::NGON
			{
				faceFill(fillVec);
			}
		}
		else // NGON ONLY
		{
			faceFill(fillVec);
		}
	}

	std::cerr << "\n\n\t TIME to subdivide (without bvh building) = " << glfwGetTime() - time << "\n\n";

	this->updateEBO();
	this->updateEdgeEBO();
	vbo.bufferData(vertices);

	this->getSelectedVertices().clear();
	this->getSelectedEdges().clear();
	this->getSelectedFaces().clear();


	FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	VertexBVHSingleton->BuildBottomUp(*this);





	return std::unordered_set<DVertex*>();
}

void Mesh::loopCut(DEdge* edge, int numberOfCuts)
{
	// stop at an edge that has more than 2 faces;
	// stop at a non quad face

	auto faces = edge->getFaces();
	int size = faces.size();
	if (size != 1 && size != 2)return;

	// new vertices for the start edge
	std::vector<std::vector<int>> newVertices(1);

	glm::vec3 position = edge->v2->position - edge->v1->position;
	position /= (numberOfCuts + 1);

	for (int i = 1;i <= numberOfCuts;i++)
	{
		newVertices[0].push_back(this->vertices.size());
		this->vertices.push_back(new DVertex(edge->v1->position + position * float(i)));
	}
	// created vertices span out from edge.v1 to edge.v2 respectively


	DFace* face = *faces.begin();
	std::unordered_set<DFace*>facesToDelete{ faces.begin(),faces.end() };
	auto faceVertices = face->getVertices();
	if (faceVertices.size() == 4)
	{
		DEdge* nextEdge;
		DEdge* oldEdge;

		for (int j = 0;j < 2;j++)
		{
			nextEdge = edge;
			oldEdge = edge;

			while (true)
			{
				nextEdge = getOpossingEdge(nextEdge, face);



				position = nextEdge->v2->position - nextEdge->v1->position;
				position /= (numberOfCuts + 1);
				std::vector<int> current;

				// vertex creation
				if (nextEdge != edge)
					for (int i = 1;i <= numberOfCuts;i++)
					{
						current.push_back(this->vertices.size());
						this->vertices.push_back(new DVertex(nextEdge->v1->position + position * float(i)));
					}
				else current = newVertices[0];


				auto old = newVertices.back();

				auto incidentEdges = getTwoIncidentEdges(nextEdge, face);

				// face fill
				if (numberOfCuts == 1)
				{
					std::cout << "\n\nNumber of cuts is 1\n\n";
					std::vector<DVertex*> out1;
					std::vector<DVertex*> out2;
					std::vector<DVertex*> temp{ incidentEdges.first->v1,incidentEdges.first->v2,incidentEdges.second->v1,incidentEdges.second->v2 };

					setWindingOrder(temp);
					GeometryUtils::splitQuadAlongMidpointsOpposite(temp,
						vertices[old[0]], vertices[current[0]], out1, out2);

					std::vector<int>fillVec;
					for (auto x : out1)
						fillVec.push_back(getVertexIndex(x));
					faceFill(fillVec);

					fillVec.clear();

					for (auto x : out2)
						fillVec.push_back(getVertexIndex(x));
					faceFill(fillVec);
				}
				else
				{
					if (getEdge(nextEdge->v1, oldEdge->v1))
					{


						std::vector<int>fillVec{ old[0],current[0],getVertexIndex(nextEdge->v1),getVertexIndex(oldEdge->v1) };
						faceFill(fillVec);

						fillVec = { old.back(),current.back(),getVertexIndex(nextEdge->v2),getVertexIndex(oldEdge->v2) };
						faceFill(fillVec);


						for (int i = 0;i < numberOfCuts - 1;i++)
						{
							std::vector<int>fillVec{ old[i],old[i + 1],current[i],current[i + 1] };
							faceFill(fillVec);
						}
					}
					else
					{
						std::vector<int>fillVec{ old.back(),current[0],getVertexIndex(nextEdge->v1),getVertexIndex(oldEdge->v2) };
						faceFill(fillVec);

						fillVec = { old.front(),current.back(),getVertexIndex(nextEdge->v2),getVertexIndex(oldEdge->v1) };
						faceFill(fillVec);


						for (int i = 0;i < numberOfCuts - 1;i++)
						{
							std::vector<int>fillVec{ old[numberOfCuts - 1 - i],old[numberOfCuts - 2 - i],current[i],current[i + 1] };
							faceFill(fillVec);
						}
					}
				}

				if (nextEdge == edge)break;



				newVertices.push_back(current);



				auto edgeFaces = nextEdge->getFaces();
				int numOfFaces = edgeFaces.size();
				// kako fillat taj drugi face kad se brejka ovde
				if (numOfFaces != 1 && numOfFaces != 2)break;

				// get the next face
				for (DFace* f : edgeFaces)
					if (f != face)
					{
						face = f;
						break;
					}
				facesToDelete.insert(face);
				auto faceVerts = face->getVertices();
				if (faceVerts.size() != 4)
				{
					for (auto vert : faceVerts)
						current.push_back(getVertexIndex(vert));

					faceFill(current);
					newVertices = { newVertices[0] };
					break;
				}

				//nextEdge = getOpossingEdge(nextEdge, face);

				oldEdge = nextEdge;

			}

			if (nextEdge == edge)break;

			face = *(++faces.begin());
		}
		// if nextEdge!=edge zavrti jos jedan krug od edge al sa kontra strane
	}
	else
	{
		for (auto f : faces)
		{
			std::cout << "\n first entry";
			std::vector<int>fillVec = newVertices[0];
			for (auto x : f->getVertices())
			{
				fillVec.push_back(getVertexIndex(x));

			}
			for (auto x : fillVec)
				std::cout << " " << x;
			std::cout << "\n ";
			faceFill(fillVec);

			fillVec = newVertices[0];

		}



		// fill both faces
	}

	deleteFaces(facesToDelete);

	this->updateEBO();
	this->updateEdgeEBO();
	vbo.bufferData(vertices);

	this->getSelectedVertices().clear();
	this->getSelectedEdges().clear();
	this->getSelectedFaces().clear();


	FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	VertexBVHSingleton->BuildBottomUp(*this);

}

void Mesh::mergeVertices(std::vector<int>& verts)
{
	if (verts.size() < 2)return;
	// map value are those vertices that are supposed to be merged, that belong to its corresponding face(key)
	std::unordered_map<DFace*, std::unordered_set<DVertex*>> faceMap;



	DVertex* v;
	glm::vec3 position(0.0f);
	for (auto index : verts)
	{
		v = vertices[index];
		for (auto face : v->getAdjecentFaces())
		{
			faceMap[face].insert(v);
		}

		position += v->position;
	}

	vertices.push_back(new DVertex(position / float(verts.size())));
	for (auto map : faceMap)
	{
		auto faceVerts = map.first->getVertices();
		auto mergedVerts = map.second;

		if (mergedVerts.size() == 1)
		{
			//std::cout << "\n\n\t new iteration [1]: ";
			faceVerts.erase(*mergedVerts.begin());

			std::vector<int>fillVec;
			for (auto x : faceVerts)
			{
				std::cout << getVertexIndex(x) << " ";
				fillVec.push_back(getVertexIndex(x));
			}

			fillVec.push_back(vertices.size() - 1);
			faceFill(fillVec);

		}
		else
		{
			DLoop* l = map.first->loop;

			// get to the starting point
			while (!mergedVerts.count(l->tip))
			{
				l = l->next;
			}
			//std::cout << "\n\n\t Starting point: "<< getVertexIndex(l->tip);



			std::vector<int>helpFill;
			DLoop* start = l;
			do
			{
				/*std::cout << "\n\n\t Hi im the new problem";*/
				//std::cout << "\n\n\t new iteration [2]: ";
				l = l->next;
				while (!mergedVerts.count(l->tip))
				{
					//std::cout << getVertexIndex(l->tip) << " ";
					helpFill.push_back(getVertexIndex(l->tip));
					l = l->next;
				}
				if (helpFill.size() == 0)
					continue;
				else if (helpFill.size() == 1)
				{
					helpFill.push_back(vertices.size() - 1);

					edgeFill(helpFill);
				}
				else
				{
					helpFill.push_back(vertices.size() - 1);
					faceFill(helpFill);
				}
				helpFill.clear();



			} while (l != start);

		}
		std::vector<DFace*>del{ map.first };
		deleteFaces(del);
	}




	this->updateEBO();
	this->updateEdgeEBO();
	vbo.bufferData(vertices);

	this->getSelectedVertices().clear();
	this->getSelectedEdges().clear();
	this->getSelectedFaces().clear();


	FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	VertexBVHSingleton->BuildBottomUp(*this);


}

std::vector<glm::vec3> Mesh::getSlideClampMax(std::unordered_set<DVertex*> neighbours)
{
	std::vector<glm::vec3> max;

	for (auto x : neighbours)
		max.push_back(x->position);

	return max;
}

std::vector<glm::vec3> Mesh::getSlideDirections(DVertex* vert, std::unordered_set<DVertex*> neighbours)
{

	std::vector<glm::vec3> directions;

	for (auto x : neighbours)
	{
		directions.push_back(glm::normalize(x->position - vert->position));
		//directions.push_back(glm::normalize(vert->position - x->position));

	}

	return directions;
}

std::vector<glm::vec2> Mesh::getSlideUnprojectedDirections(DVertex* vert, std::unordered_set<DVertex*> neighbours)
{
	auto camera = cameraSingleton->getCamera("Viewport");
	int screenWidth = camera->getWidth();
	int screenHeight = camera->getHeight();

	glm::mat4 projectionViewModel = camera->getProjectionMatrix() * camera->getViewMatrix() * model;


	std::vector<glm::vec2> directions;


	glm::vec4 clipSpace = projectionViewModel * glm::vec4(vert->position, 1.0f);

	if (clipSpace.w == 0.0f)
		std::cout << "\n\n\t ERROR mesh.getSlideUnprojectedDirections 1 "; // or handle error

	glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;

	glm::vec2 vert2D;
	vert2D.x = (ndc.x + 1.0f) * 0.5f * screenWidth;
	vert2D.y = (1.0f - ndc.y) * 0.5f * screenHeight;

	std::cout << "\n\n\t vert2D " << vert2D.x << " " << vert2D.y;



	for (auto x : neighbours)
	{
		clipSpace = projectionViewModel * glm::vec4(x->position, 1.0f);

		if (clipSpace.w == 0.0f)
			std::cout << "\n\n\t ERROR mesh.getSlideUnprojectedDirections 2 "; // or handle error

		ndc = glm::vec3(clipSpace) / clipSpace.w;

		glm::vec2 x2D;
		x2D.x = (ndc.x + 1.0f) * 0.5f * screenWidth;
		x2D.y = (1.0f - ndc.y) * 0.5f * screenHeight;

		std::cout << "\n\n\t x2D " << x2D.x << " " << x2D.y;

		directions.push_back(glm::normalize(x2D - vert2D));

		std::cout << "\n\n\t unprojected direction " << directions.back().x << " " << directions.back().y;
		//directions.push_back(glm::normalize(vert2D - x2D));
	}


	return directions;
}


void Mesh::findUVIslands(std::vector<std::unordered_set<DFace*>>& islands)
{
	std::unordered_set<DFace*>faces = getAllFaces();
	std::unordered_map<DFace*, bool> facesVisited;
	facesVisited.reserve(faces.size());
	for (DFace* f : faces) {
		facesVisited[f] = false;
	}

	for (auto face : facesVisited)
	{
		if (face.second)continue;

		std::unordered_set<DFace*> island;

		std::queue<DFace*> q;


		face.second = true;
		q.push(face.first);

		while (!q.empty())
		{
			DFace* f = q.front();
			q.pop();
			island.insert(f);

			for (DLoop* loop : f->getLoops())
			{
				if (loop->edge->isSeam)continue;

				for (DFace* neighbour : loop->edge->getFaces())
				{
					if (facesVisited[neighbour]) continue;
					facesVisited[neighbour] = true;
					q.push(neighbour);
				}
			}

		}
		islands.push_back(island);


	}


}

std::vector<int> Mesh::getFaceIndices(DFace* face)
{
	std::vector<int> returnVec;

	DLoop* temp = face->loop;

	do {
		returnVec.push_back(this->getVertexIndex(temp->tip));
		temp = temp->next;
	} while (temp != face->loop);

	return returnVec;
}

std::pair<int, int> Mesh::getEdgeIndices(DEdge* edge)
{
	return std::pair<int, int>{this->getVertexIndex(edge->v1), this->getVertexIndex(edge->v2)};
}





std::vector<GLuint> Mesh::formTrianglesForDrawing()
{
	std::vector<GLuint> returnVec = std::vector<GLuint>();
	//std::cout<<"\n";
	for (int i = 0; i < selectedFaces.size(); i++)
	{
		std::vector<DVertex*>faceVertices = selectedFaces[i]->getVerticesVector();
		//std::cout << selectedFaces[i] << " ---- number of vertices of face\n";
		if (faceVertices.size() == 3)
		{
			returnVec.push_back(this->getVertexIndex(faceVertices[0]));
			returnVec.push_back(this->getVertexIndex(faceVertices[1]));
			returnVec.push_back(this->getVertexIndex(faceVertices[2]));
		}
		else if (faceVertices.size() == 4)
		{
			returnVec.push_back(this->getVertexIndex(faceVertices[0]));
			returnVec.push_back(this->getVertexIndex(faceVertices[1]));
			returnVec.push_back(this->getVertexIndex(faceVertices[2]));

			returnVec.push_back(this->getVertexIndex(faceVertices[2]));
			returnVec.push_back(this->getVertexIndex(faceVertices[3]));
			returnVec.push_back(this->getVertexIndex(faceVertices[0]));

			//std::cout << "\n" << this->getVertexIndex(faceVertices[0]) << " " << this->getVertexIndex(faceVertices[1]) << " " << this->getVertexIndex(faceVertices[2]) << "\n"
			//	<< this->getVertexIndex(faceVertices[2])<<" " << this->getVertexIndex(faceVertices[0]) << " " << this->getVertexIndex(faceVertices[3]) << "\n\n";

		}
		else if (faceVertices.size() > 4)
		{
			/*for (auto x : selectedFaces)
				std::cout << " " << x;
				std::cout << "\n" ;*/

				//for (int j = i + 2;j <i+ selectedFaces[i]-1;j++)
			for (int j = 1; j < faceVertices.size() - 1; j++)
			{
				returnVec.push_back(this->getVertexIndex(faceVertices[0])); // anchor
				returnVec.push_back(this->getVertexIndex(faceVertices[j])); // 2nd
				returnVec.push_back(this->getVertexIndex(faceVertices[j + 1])); // 3rd


			}


		}
		else std::cout << "\n\n MISTAKE Mesh::formTrianglesForDrawing() \n\n";

	}


	return returnVec;
}






DVertex* Mesh::duplicateVertex(DVertex& vertex)
{
	vertices.push_back(new DVertex(vertex));

	return vertices.back();
}

DEdge* Mesh::getOpossingEdge(DEdge* edge, DFace* face)
{

	DLoop* l = face->loop;

	while (l->edge != edge)
	{
		l = l->next;
	}


	return l->next->next->edge;
}

std::pair<DEdge*, DEdge*> Mesh::getTwoIncidentEdges(DEdge* edge, DFace* face)
{

	DLoop* l = face->loop;

	while (l->edge != edge)
	{
		l = l->next;
	}

	if (l->tip == edge->v1)
		return { l->next->edge,l->prev->edge };
	else
		return { l->prev->edge, l->next->edge };
}

void Mesh::lscmFaceIndicesHelper(std::vector<GLuint>& F, int index, DLoop* loop)
{


	//std::cout << "\n\n\t OLD \n";
	//for (auto i = 0;i < F.size();i += 3)
	//{
	//	std::cout << F[i] << " " << F[i + 1] << " " << F[i + 2];
	//	std::cout << "\n";
	//}


	int indexToSwap = getVertexIndex(loop->tip);


	std::unordered_set<int> setOfVertices;

	for (DVertex* vertex : loop->face->getVertices())
	{
		setOfVertices.insert(this->getVertexIndex(vertex));
	}

	int numberOfTriplets = setOfVertices.size() - 2;


	const size_t span = 3 * static_cast<size_t>(numberOfTriplets);
	const size_t total = F.size();
	if (span > total) return;

	size_t required_unique = setOfVertices.size();

	for (size_t j = 0; j + span <= total; j += 3) {
		std::unordered_map<int, int> freq;
		size_t present_unique = 0;
		bool outside_found = false;

		for (size_t k = j; k < j + span; ++k) {
			int val = indices[k];
			if (setOfVertices.count(val) != 0) {
				int prev = freq[val];
				freq[val] = prev + 1;
				if (prev == 0) {
					present_unique++;
				}
			}
			else {
				outside_found = true;
				break;
			}
		}

		if (outside_found) continue;
		if (present_unique != required_unique) continue;


		//std::cout << "\n\n\n indices \n\t";
		for (size_t offset = 0; offset < span; offset++) {

			if (F[j + offset] == indexToSwap)
				F[j + offset] = index;

		}
	}


	//std::cout << "\n\n\t NEW \n";
	//for (auto i = 0;i < F.size();i += 3)
	//{
	//	std::cout << F[i] << " " << F[i + 1] << " " << F[i + 2];
	//	std::cout << "\n";
	//}

}

void Mesh::spitUVsAlongSeams()
{
	std::unordered_set<std::shared_ptr<UVVertex>>uvs;

	for (DEdge* e : getAllEdges())
	{
		if (!e->loop)continue;

		if (!e->isSeam)
		{
			uvs.insert(e->loop->uvVertex);
			uvs.insert(e->loop->radialNext->uvVertex);

			continue;
		}

		if (e->loop->radialNext == e->loop)
		{

			uvs.insert(e->loop->uvVertex);
			continue;
		}

		DLoop* loop = e->loop;

		do
		{
			if (loop->next->edge->isSeam)
			{
				if (loop->uvVertex.use_count() != 1)
				{
					loop->uvVertex = std::make_shared<UVVertex>();
				}
				else std::cout << "\n\n\t The count is 1";
				uvs.insert(loop->uvVertex);

			}

			loop = loop->radialNext;

		} while (loop != e->loop);

	}

	std::cout << "\n\n\tNumber of UVs: " << uvs.size();

}

void Mesh::mergeUVs()
{
	uvCoords.clear();

	std::unordered_map<DVertex*, std::shared_ptr<UVVertex>> map;
	for (DVertex* vertex : vertices)
	{
		map.insert({ vertex,std::make_shared<UVVertex>() });
	}

	for (DFace* face : getAllFaces())
	{
		for (DLoop* loop : face->getLoops())
			loop->uvVertex = map[loop->tip];

	}

	std::cout << "\n\n\t MergeUVs size: " << map.size();

}

void Mesh::formUVTopology()
{


}

void Mesh::lscmUVUnwrap()
{
	mergeUVs();


	auto tempV = vertices;
	auto tempF = indices;

	// DLoop , global index
	std::unordered_map<DLoop*, int> newUVs;


	// Split UVs along seams 
	for (DEdge* e : getAllEdges())
	{
		if (!e->loop)continue;

		if (!e->isSeam)
			continue;


		if (e->loop->radialNext == e->loop)
			continue;

		DLoop* loop = e->loop;

		do
		{
			if (loop->next->edge->isSeam)
			{
				if (loop->uvVertex.use_count() != 1)
				{
					loop->uvVertex = std::make_shared<UVVertex>();
					tempV.push_back(loop->tip);
					newUVs.insert({ loop, tempV.size() - 1 });
					lscmFaceIndicesHelper(tempF, tempV.size() - 1, loop);

				}
				else std::cout << "\n\n\t The count is 1";

			}

			loop = loop->radialNext;

		} while (loop != e->loop);

	}

	//std::cout << "\n\n\t tempV size: " << tempV.size()<< "\t\t newUVs size:"<<newUVs.size();



	// Find islands
	std::vector<std::unordered_set<DFace*>> islands;
	findUVIslands(islands);

	int islandsSize = islands.size();
	double offset = 1.0 / islandsSize;

	int offsetCounter = 0;
	this->uvCoords.clear();
	for (auto island : islands)
	{
		// local index, DLoop* which points to the vert with local index
		std::map< int, DLoop*> loopUVmap;
		// global index, local index
		std::map<int, int> vHelper;
		std::vector<int> vHelperVector;
		std::vector<std::vector<int>> fHelper;

		int counter = 0;
		for (DFace* face : island)
		{
			// per face UV indices
			std::vector<int> temp;
			for (DLoop* loop : face->getLoops())
			{
				int globalIndex;
				if (newUVs.count(loop))
					globalIndex = newUVs[loop];
				else globalIndex = std::find(tempV.begin(), tempV.end(), loop->tip) - tempV.begin();

				if (!vHelper.count(globalIndex))
				{
					vHelper[globalIndex] = vHelper.size();
					loopUVmap.insert({ vHelper[globalIndex], loop });

				}
				temp.push_back(vHelper[globalIndex]);// pushing back local index
			}


			// creating fHelper
			if (temp.size() == 3)
			{
				fHelper.push_back(std::move(temp));

			}
			else if (temp.size() == 4)
			{
				fHelper.emplace_back(std::initializer_list<int>{temp[0], temp[1], temp[2]});
				fHelper.emplace_back(std::initializer_list<int>{temp[2], temp[3], temp[0]});

			}
			else if (temp.size() > 4)
			{
				for (int j = 1; j < temp.size() - 1; j++)
					fHelper.emplace_back(std::initializer_list<int>{temp[0], temp[j], temp[j + 1]});

			}
			else std::cout << "\n\n MISTAKE Mesh::lscmUVUnwrap() ---> Fhelper matrix formation error \n\n";

		}


		Eigen::MatrixXd V(vHelper.size(), 3);
		for (auto vHelp : vHelper)
			V.row(vHelp.second) <<
			tempV[vHelp.first]->position.x,
			tempV[vHelp.first]->position.y,
			tempV[vHelp.first]->position.z;




		Eigen::MatrixXi F(fHelper.size(), 3);
		for (int i = 0;i < fHelper.size();i++)
			F.row(i) <<
			fHelper[i][0],
			fHelper[i][1],
			fHelper[i][2];


		// Find boundary loop
		Eigen::VectorXi bnd;
		igl::boundary_loop(F, bnd); // OVDJE NEGDJE ERROR KAD NEMA SEAMA;

		if (bnd.size() == 0) {
			std::cerr << "Error: UV island has no boundary (missing seams)." << std::endl;
			return;
		}

		// Appoint boundary vertices
		Eigen::VectorXi b(2);
		b(0) = bnd(0);
		b(1) = bnd(bnd.size() / 2);

		// Appoint boundary vertices UV coordinates
		Eigen::MatrixXd bc(2, 2);
		bc <<
			offset * offsetCounter, 0.0,  // UV for b(0)
			offset* (offsetCounter + 1), 0.0;  // UV for b(1)
		offsetCounter++;
		// Compute lscm
		Eigen::MatrixXd V_uv;
		if (!igl::lscm(V, F, b, bc, V_uv))
			return;

		// Normalize UV coordinates to [0, 1] range
		Eigen::Vector2d minUV = V_uv.colwise().minCoeff();
		Eigen::Vector2d maxUV = V_uv.colwise().maxCoeff();
		V_uv = (V_uv.rowwise() - minUV.transpose());
		V_uv = V_uv.array().rowwise() / (maxUV - minUV).transpose().array();




		// Update UVs in the mesh and DLoops

		for (auto it : loopUVmap)
		{
			int index = it.first;
			DLoop* l = it.second;
			l->uvVertex->uv.x = V_uv(index, 0);
			l->uvVertex->uv.y = V_uv(index, 1);
			if (std::find(uvCoords.begin(), uvCoords.end(), l->uvVertex) == uvCoords.end())// mozda belaj ... ovdje je bio unordered_set prije
				uvCoords.push_back(l->uvVertex);

		}


	}

	//std::cout << "\n\n\t Number of UV islands is: " << islands.size();
	//std::cout << "\n\n\t Number of UV coords is: " << uvCoords.size();


	// ne znam zasto ovo radi ali NE DIRAJ!!!!! ako se ovo izbaci onda topologija ne valja
	std::unordered_map<std::shared_ptr<UVVertex>, int> uvIndexMap;
	uvIndexMap.reserve(uvCoords.size());
	for (int i = 0; i < (int)uvCoords.size(); i++)
		uvIndexMap[uvCoords[i]] = i;

	// form UV topology
	uvEdgeindices.clear();
	for (DFace* face : getAllFaces())
	{
		std::cout << "\n ";
		bool flag = true;
		for (DLoop* loop : face->getLoops())
			if (!loop->edge->isSeam)
			{
				flag = false;
				break;
			}
		//if (flag)
		//	std::cout << "\n\t\tfound it ";

		for (DLoop* loop : face->getLoops())
		{
			int index1 = uvIndexMap[loop->uvVertex];
			int index2 = uvIndexMap[loop->next->uvVertex];

			if (flag)
			{

				//	std::cout << "\n Vert the uv  " << index1 << " belongs to " << getVertexIndex(tempV[index1]);
				//	std::cout << "\n Vert the uv  " << index2 << " belongs to " << getVertexIndex(tempV[index2]);
			}
			uvEdgeindices.push_back(index1);
			uvEdgeindices.push_back(index2);
		}


	}

	//std::cout << "\n\n\t Number of uvEdgeIndices: " << uvEdgeindices.size() << "\n";
	//std::cout << "\n\n\t Number of newUVs: " << newUVs.size() << "\n";

	//for (int i = 0;i < uvEdgeindices.size();i += 2)
	//{
	//	std::cout << "\n\t" << uvEdgeindices[i] << "  " << uvEdgeindices[i + 1];
	//}


	// pack the islands

	// grid of n x n cells, where each islands takes up one cell
	int n = std::ceil(std::sqrt(islands.size()));
	double sizeOfCell = 1.0 / n;

	int row = 0;


	for (int i = 0;i < islands.size();i++)
	{
		glm::vec2 min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min());
		for (DFace* face : islands[i])
		{
			auto loops = face->getLoops();
			for (DLoop* loop : loops)
			{
				glm::vec2 uv = loop->uvVertex->uv;
				if (uv.x < min.x) min.x = uv.x;
				if (uv.y < min.y) min.y = uv.y;
				if (uv.x > max.x) max.x = uv.x;
				if (uv.y > max.y) max.y = uv.y;
			}

			max -= min;
			double scaleX = sizeOfCell / (max.x * 1.12f);
			double scaleY = sizeOfCell / (max.y * 1.12f);


			for (DLoop* loop : loops)
			{
				// shift to the 0 0 cell
				loop->uvVertex->uv.x -= min.x;
				loop->uvVertex->uv.y -= min.y;


				// scale to the size of the cell

				loop->uvVertex->uv.x *= scaleX;
				loop->uvVertex->uv.y *= scaleY;
			}

			// shift to the correct cell

			for (DLoop* loop : loops)
			{
				loop->uvVertex->uv.x += (i % n) * sizeOfCell;
				loop->uvVertex->uv.y += row * sizeOfCell;
			}
			if ((i + 1) % n == 0)row++;

		}
	}

	buildGPUVertices();
}

//if (V_uv.rows() == 4) std::cout << "\n\n\tQUAD\n";
//else std::cout << "\n\n\t OTHER\n";
//
//std::cout << l->uvVertex->uv.x << " " << l->uvVertex->uv.y << "\n";
void Mesh::extrudeVertices(std::vector<int>& verts, bool update)
{
	DVertex* vertex;
	DVertex* duplicate;

	int size = verts.size();

	for (auto x : verts)
	{
		vertex = vertices[x];
		duplicate = duplicateVertex(*vertex);

		DEdge* e = new DEdge(vertex, duplicate);

		edgeIndices.push_back(x);
		edgeIndices.push_back(vertices.size() - 1);

	}

	if (!update)return;

	selectedVertexIndices.clear();
	for (int i = vertices.size() - size;i < vertices.size();i++)
		selectedVertexIndices.push_back(i);

	updateEdgeEBO();
	vbo.bufferData(vertices);

	VertexBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);

}
template<typename Container, typename>
void Mesh::pokeFaces(Container& faces, bool update)
{
	//std::cout << "\n\n\t Beggining of pokeFaces... vertices.size= " << vertices.size();

	for (DFace* face : faces)
	{
		auto verts = face->getVertices();

		glm::vec3 midPoint(0.0f);
		for (auto vert : verts)
		{
			midPoint += vert->position;
		}

		midPoint /= verts.size();

		vertices.push_back(new DVertex(midPoint));

		auto edges = face->getEdges();

		eraseFace(face);

		std::vector<int> fillVec;

		for (auto edge : edges)
		{
			fillVec.push_back(getVertexIndex(edge->v1));
			fillVec.push_back(getVertexIndex(edge->v2));
			fillVec.push_back(vertices.size() - 1);

			faceFill(fillVec);

			fillVec.clear();
		}


	}
	if (!update)return;

	vbo.bufferData(vertices);
	updateEBO();
	updateEdgeEBO();

	selectedFaces.clear();

	VertexBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	FaceBVHSingleton->BuildBottomUp(*this);
	//std::cout << "\n\n\t End of pokeFaces... vertices.size= " << vertices.size();

}

template<typename Container, typename>
void Mesh::triangulateFaces(Container& faces, bool update)
{
	std::vector<int> vertIndices;
	std::vector<int> fillVec;
	for (DFace* face : faces)
	{

		auto verts = face->getVertices();

		if (verts.size() == 3)continue;
		eraseFace(face);

		vertIndices.clear();
		for (auto vert : verts)
			vertIndices.push_back(getVertexIndex(vert));

		setWindingOrder(vertIndices);

		int stride = 1;
		int num = vertIndices.size();

		while (true) {
			bool anyPrinted = false;

			for (int i = 0; i < num; i += 2 * stride) {
				fillVec.clear();
				int a = i;
				int b = i + stride;
				int c = i + 2 * stride;

				if (b >= num) break;

				if (c >= num) c = 0;

				if (a == b || b == c || a == c)
					continue;

				fillVec.push_back(vertIndices[a]);
				fillVec.push_back(vertIndices[b]);
				fillVec.push_back(vertIndices[c]);

				faceFill(fillVec, true);

				anyPrinted = true;
			}

			if (!anyPrinted) break;

			stride *= 2;
		}

	}

	if (!update)return;


	updateEBO();
	updateEdgeEBO();

	selectedFaces.clear();

	//VertexBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	FaceBVHSingleton->BuildBottomUp(*this);

}

template<typename Container, typename >
void Mesh::flipFaceNormals(Container& faces)
{
	for (DFace* face : faces)
	{
		face->flip();

		std::vector<int> newIndices;
		bool flag;
		std::unordered_set<int> setOfVertices;

		for (DVertex* vertex : face->getVertices())
		{
			setOfVertices.insert(this->getVertexIndex(vertex));
		}

		int numberOfTriplets = setOfVertices.size() - 2;

		//for (int j = 0; j < this->indices.size(); j += 3 * numberOfTriplets)
		//{
		//	flag = false;
		//	for (int k = j; k < j + 3 * numberOfTriplets /* && k<this->indices.size() */ ; k++)
		//	{

		//		if (!setOfVertices.count(this->indices[k]))
		//		{
		//			flag = true;
		//			break;
		//		}
		//	}
		//	if (flag)continue;

		//		std::cout <<"\n\n\n indices \n\t";
		//	for (int i = 0;i < 3 * numberOfTriplets;i += 3)
		//	{
		//		std::cout << indices[j + i] << " " << indices[j + i + 1] << " " << indices[j + i + 2] << "\n\t";

		//		std::swap(*(this->indices.begin() + j + i), *(this->indices.begin() + j + i + 2));
		//	}

		//	//newIndices.insert(newIndices.begin(), this->indices.begin() + j, this->indices.begin() + j + 3 * numberOfTriplets);
		//	//this->indices.erase(this->indices.begin() + j, this->indices.begin() + j + 3 * numberOfTriplets);


		//	//break;  /// dodaj ovaj break i u eraseFace

		//}



		const size_t span = 3 * static_cast<size_t>(numberOfTriplets);
		const size_t total = indices.size();
		if (span > total) return;

		size_t required_unique = setOfVertices.size();


		for (size_t j = 0; j + span <= total; j += 3) {
			std::unordered_map<int, int> freq;
			size_t present_unique = 0;
			bool outside_found = false;

			for (size_t k = j; k < j + span; ++k) {
				int val = indices[k];
				if (setOfVertices.count(val) != 0) {
					int prev = freq[val];
					freq[val] = prev + 1;
					if (prev == 0) {
						present_unique++;
					}
				}
				else {
					outside_found = true;
					break;
				}
			}

			if (outside_found) continue;
			if (present_unique != required_unique) continue;


			std::cout << "\n\n\n indices \n\t";
			for (size_t offset = 0; offset < span; offset += 3) {
				size_t base = j + offset;
				std::cout << indices[base] << " " << indices[base + 1] << " " << indices[base + 2] << "\n\t";
				std::swap(*(this->indices.begin() + base), *(this->indices.begin() + base + 2));
			}
		}


	}


	updateEBO();

}


void Mesh::bridgeFaces(DFace* faceA, DFace* faceB, bool update)
{
	auto vertVecA = faceA->getVerticesVector();
	auto vertVecB = faceB->getVerticesVector();

	if (vertVecA.size() != vertVecB.size())
	{
		std::cout << " \n\n\tFaces do not have equal number of vertices\n";
		return;
	}

	int size = vertVecA.size();

	std::vector<int> vertIndicesA;
	std::vector<int> vertIndicesB;

	for (int i = 0;i < size;i++)
	{
		vertIndicesA.push_back(getVertexIndex(vertVecA[i]));
		vertIndicesB.push_back(getVertexIndex(vertVecB[i]));
	}

	glm::vec3 normalA = setWindingOrder(vertIndicesA);
	glm::vec3 normalB = setWindingOrder(vertIndicesB);

	if (glm::dot(normalA, normalB) < 0.0f)
		std::reverse(vertIndicesB.begin(), vertIndicesB.end());

	std::vector<int>fillVec;
	int temp;

	for (int i = 0;i < size;i++)
	{

		temp = (i + 1) % size;

		fillVec.push_back(vertIndicesA[i]);
		fillVec.push_back(vertIndicesA[temp]);
		fillVec.push_back(vertIndicesB[i]);
		fillVec.push_back(vertIndicesB[temp]);


		faceFill(fillVec);
		fillVec.clear();
	}

	// if there were adjecentFaces before the bridge operation
	if (faceA->getAdjecentFaces().size() != size)
		eraseFace(faceA);

	if (faceB->getAdjecentFaces().size() != size)
		eraseFace(faceB);



	if (!update)return;


	updateEBO();
	updateEdgeEBO();

	selectedFaces.clear();

	//VertexBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	FaceBVHSingleton->BuildBottomUp(*this);


}

void Mesh::trisToQuads(std::unordered_set<DFace*>& faces, bool update)
{

	std::unordered_map<DFace*, bool> temp;

	for (DFace* face : faces)
		temp.insert({ face,false });





	std::vector<int> fillVec;

	for (auto& pair : temp)
	{
		if (pair.second) continue;

		DFace* face = pair.first;
		fillVec.clear();

		auto loops = face->getLoops();
		if (loops.size() != 3)continue;

		std::vector<std::pair<DLoop*, float>> v;


		for (DLoop* loop : loops)
			v.push_back({ loop,glm::length(loop->tip->position - loop->prev->tip->position) });


		std::sort(v.begin(), v.end(), [](auto& a, auto& b) { return a.second > b.second; });

		DLoop* longest = v[0].first;
		DLoop* middle = v[1].first;
		DLoop* shortest = v[2].first;

		DLoop* commonSide = nullptr;
		DFace* otherFace = nullptr;

		for (int i = 0; i < 3; ++i)
		{
			DLoop* candidate = v[i].first;
			DLoop* radial = candidate->radialNext;

			if (radial == candidate)
				continue;

			DFace* radialFace = radial->face;
			if (!radialFace || radialFace->getLoops().size() != 3)
				continue;

			if (!faces.count(radialFace))
				continue;


			commonSide = candidate;
			otherFace = radialFace;
			break;
		}

		if (!commonSide || !otherFace)
			continue;

		auto verts = face->getVertices();
		auto verts1 = otherFace->getVertices();
		verts.insert(verts1.begin(), verts1.end());

		for (auto x : verts)
			fillVec.push_back(getVertexIndex(x));

		temp[commonSide->radialNext->face] = true;
		temp[face] = true;

		std::unordered_set<DEdge*> deleteSet{ getEdge(getVertexIndex(commonSide->tip),getVertexIndex(commonSide->prev->tip)) };
		deleteOnlyEdgesAndFaces(deleteSet);

		faceFill(fillVec);



	}
	if (!update)return;


	updateEBO();
	updateEdgeEBO();

	selectedFaces.clear();

	//VertexBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	FaceBVHSingleton->BuildBottomUp(*this);

}
template <typename Container, typename>
std::unordered_set<DEdge*>  Mesh::extrudeEdges(Container& edges, bool update)
{

	DVertex* v1, * v2;
	DVertex* duplicate1, * duplicate2;
	int index1, index2;
	DEdge* e1, * e2, * middle;

	// middle edges
	std::unordered_set<DEdge*> returnEdges;

	std::unordered_map<DVertex*, DVertex*> visited;

	for (DEdge* e : edges)
	{
		v1 = e->v1;
		v2 = e->v2;
		index1 = this->getVertexIndex(v1);
		index2 = this->getVertexIndex(v2);

		auto it = visited.find(v1);
		if (it == visited.end())
		{
			duplicate1 = duplicateVertex(*v1);
			visited.insert({ v1,duplicate1 });
			e1 = new DEdge(v1, duplicate1);

			edgeIndices.push_back(index1);
			edgeIndices.push_back(vertices.size() - 1);
		}
		else // edge alread exists
		{
			duplicate1 = it->second;
			e1 = getEdge(index1, this->getVertexIndex(duplicate1));
		}


		it = visited.find(v2);
		if (it == visited.end())
		{
			duplicate2 = duplicateVertex(*v2);
			visited.insert({ v2,duplicate2 });
			e2 = new DEdge(v2, duplicate2);
			edgeIndices.push_back(index2);
			edgeIndices.push_back(vertices.size() - 1);
		}
		else // edge alread exists
		{
			duplicate2 = it->second;
			e2 = getEdge(index2, this->getVertexIndex(duplicate2));
		}

		middle = new DEdge(duplicate1, duplicate2);

		returnEdges.insert(middle);

		int duplicateIndex1 = getVertexIndex(duplicate1);
		int duplicateIndex2 = getVertexIndex(duplicate2);

		edgeIndices.push_back(duplicateIndex1);
		edgeIndices.push_back(duplicateIndex2);


		DFace* face = new DFace();

		DLoop* l1 = new DLoop(duplicate1, e1, face);
		DLoop* l2 = new DLoop(duplicate2, middle, face);
		DLoop* l3 = new DLoop(e->v2, e2, face);
		DLoop* l4 = new DLoop(e->v1, e, face);

		e1->connectLoopToEdge(l1);
		middle->connectLoopToEdge(l2);
		e2->connectLoopToEdge(l3);
		e->connectLoopToEdge(l4);

		l1->next = l2; l1->prev = l4;
		l2->next = l3; l2->prev = l1;
		l3->next = l4; l3->prev = l2;
		l4->next = l1; l4->prev = l3;

		face->loop = l1;

		this->indices.push_back(index1);
		this->indices.push_back(duplicateIndex1);
		this->indices.push_back(index2);

		this->indices.push_back(index2);
		this->indices.push_back(duplicateIndex1);
		this->indices.push_back(duplicateIndex2);


	}

	if (!update)return returnEdges;

	int size = selectedVertexIndices.size();
	edges.clear();
	selectedVertexIndices.clear();
	for (int i = vertices.size() - size;i < vertices.size();i++)
		selectedVertexIndices.push_back(i);

	updateEdgeEBO();
	vbo.bufferData(vertices);
	updateEBO();

	VertexBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	FaceBVHSingleton->BuildBottomUp(*this);


	return returnEdges;

}

template <typename Container, typename>
void Mesh::extrudeFaces(Container& faces, bool update)
{
	std::unordered_map<DEdge*, int> edges;

	for (auto face : faces)
	{

		for (auto edge : face->getEdges())
		{
			auto it = edges.find(edge);

			if (it == edges.end())
			{
				edges.insert({ edge,1 });
			}
			else it->second += 1;
		}
	}

	std::vector<DFace*> separatedFaces = separate(faces);


	std::unordered_map<DEdge*, int> edgesSeparated;

	for (auto face : separatedFaces)
	{

		for (auto edge : face->getEdges())
		{
			auto it = edgesSeparated.find(edge);

			if (it == edgesSeparated.end())
			{
				edgesSeparated.insert({ edge,1 });
			}
			else it->second += 1;
		}
	}

	std::vector<int> fillVector;

	for (auto it : edges)
	{
		if (it.second != 1)continue;

		auto found = std::find_if(edgesSeparated.begin(), edgesSeparated.end(),
			[&it](const auto& pair) {

				if (pair.second != 1)return false;

				DEdge* e1 = pair.first;
				DEdge* e2 = it.first;

				return
					(
						(*e1->v1 == *e2->v1 && *e1->v2 == *e2->v2) ||
						(*e1->v1 == *e2->v2 && *e1->v2 == *e2->v1)
						);


			});
		if (found == edgesSeparated.end())continue;

		fillVector = { getVertexIndex(it.first->v1),getVertexIndex(it.first->v2) };

		if (*found->first->v2 == *it.first->v2)
		{
			fillVector.push_back(getVertexIndex(found->first->v2));
			fillVector.push_back(getVertexIndex(found->first->v1));
		}
		else
		{
			fillVector.push_back(getVertexIndex(found->first->v1));
			fillVector.push_back(getVertexIndex(found->first->v2));
		}


		faceFill(fillVector, true);

	}

	if (!update)return;

	this->updateEBO();
	this->updateEdgeEBO();
	vbo.bufferData(vertices);

	//this->getSelectedVertices().clear();
	//this->getSelectedEdges().clear();
	//this->getSelectedFaces().clear();

	FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	VertexBVHSingleton->BuildBottomUp(*this);


}

template <typename Container, typename>
void Mesh::extrudeIndividualFaces(Container& faces, bool update)
{
	std::vector<DFace*> vec;
	for (auto face : faces)
	{
		vec = { face };
		extrudeFaces(vec);
	}

	this->updateEBO();
	this->updateEdgeEBO();
	vbo.bufferData(vertices);

	//this->getSelectedVertices().clear();
	//this->getSelectedEdges().clear();
	//this->getSelectedFaces().clear();

	FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	VertexBVHSingleton->BuildBottomUp(*this);
}

void Mesh::extrudeManifold()
{
}

void Mesh::extrudeAlongNormals()
{
}

void Mesh::extrudeRepeat()
{
}

void Mesh::spin()
{
}

template <typename Container, typename>
std::vector<DFace*> Mesh::separate(Container faces)
{
	std::unordered_set<DEdge*> edges;
	selectedFaces = duplicateFaces(faces);

	for (auto face : faces)
	{
		for (auto edge : face->getEdges())
			edges.insert(edge);

		eraseFace(face);

	}
	std::unordered_set<DEdge*> edgesToDelete;
	for (auto edge : edges)
		if (!edge->loop)
			edgesToDelete.insert(edge);

	deleteEdges(edgesToDelete);


	setSelectedVertexIndicesFromFaces(selectedFaces);

	vbo.bufferData(vertices);
	updateEBO();
	updateEdgeEBO();

	VertexBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	FaceBVHSingleton->BuildBottomUp(*this);

	return selectedFaces;
}

std::vector<DVertex*> Mesh::duplicateVertices(std::vector<int>& verts, bool update)
{
	std::vector<DVertex*> returnVec;
	std::vector<int> vertIndices;
	for (auto x : verts)
	{
		returnVec.push_back(duplicateVertex(*vertices[x]));
		vertIndices.push_back(vertices.size() - 1);
	}

	if (!update)return returnVec;


	selectedVertexIndices = vertIndices;
	vbo.bufferData(vertices);

	VertexBVHSingleton->BuildBottomUp(*this);

	return returnVec;
}

template <typename Container, typename>
std::vector<DEdge*> Mesh::duplicateEdges(Container& edges, bool update)
{
	std::vector<DEdge*> returnVec;
	std::unordered_map<DVertex*, DVertex*> visited;
	std::unordered_set<int> vertIndices;
	DVertex* v1, * v2;
	for (auto edge : edges)
	{
		auto it = visited.find(edge->v1);
		if (it != visited.end())
		{
			v1 = it->second;
			edgeIndices.push_back(getVertexIndex(v1));
			vertIndices.insert(edgeIndices.back());
		}
		else
		{
			v1 = duplicateVertex(*edge->v1);
			visited.insert({ edge->v1,v1 });
			edgeIndices.push_back(vertices.size() - 1);
			vertIndices.insert(edgeIndices.back());
		}


		it = visited.find(edge->v2);

		if (it != visited.end())
		{
			v2 = it->second;
			edgeIndices.push_back(getVertexIndex(v2));
			vertIndices.insert(edgeIndices.back());
		}
		else
		{
			v2 = duplicateVertex(*edge->v2);
			visited.insert({ edge->v2,v2 });
			edgeIndices.push_back(vertices.size() - 1);
			vertIndices.insert(edgeIndices.back());
		}

		returnVec.push_back(new DEdge(v1, v2));

	}

	if (!update)return returnVec;

	selectedEdges = returnVec;
	selectedVertexIndices.clear();
	selectedVertexIndices.insert(selectedVertexIndices.begin(), vertIndices.begin(), vertIndices.end());

	vbo.bufferData(vertices);
	//updateEBO();
	updateEdgeEBO();

	VertexBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	//FaceBVHSingleton->BuildBottomUp(*this);

	return returnVec;
}

template <typename Container, typename>
std::vector<DFace*> Mesh::duplicateFaces(Container& faces, bool update)
{
	std::vector<DFace*> newFaces;
	std::unordered_map<DVertex*, DVertex*> visited;
	std::vector<int> toFill;
	std::unordered_set<int> vertIndices;

	for (auto face : faces)
	{
		for (auto vertex : face->getVertices())
		{
			auto it = visited.find(vertex);
			if (it != visited.end())
			{
				toFill.push_back(getVertexIndex(it->second));
			}
			else
			{
				visited.insert({ vertex,duplicateVertex(*vertex) });
				toFill.push_back(vertices.size() - 1);
			}
		}
		newFaces.push_back(faceFill(toFill));

		vertIndices.insert(toFill.begin(), toFill.end());
		toFill.clear();

	}

	if (!update)return newFaces;

	selectedFaces = newFaces;
	selectedVertexIndices.clear();
	selectedVertexIndices.insert(selectedVertexIndices.begin(), vertIndices.begin(), vertIndices.end());

	vbo.bufferData(vertices);
	updateEBO();
	updateEdgeEBO();

	VertexBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	FaceBVHSingleton->BuildBottomUp(*this);

	return newFaces;
}

template <typename Container, typename>
void Mesh::deleteVertices(Container& vertIndices, bool update)
{
	std::vector<DEdge*>& selectedEdges = this->getSelectedEdges();

	std::unordered_set<DEdge*>edges;
	std::unordered_set<DVertex*>verticesToDelete;

	for (auto x : vertIndices)
	{
		auto temp = vertices[x]->getAdjecentEdges();
		if (temp.empty())
		{
			verticesToDelete.insert(vertices[x]);
		}
		else if (temp.size() == 1)
		{
			verticesToDelete.insert(vertices[x]);
			eraseEdge(*temp.begin(), false);
		}
		else
			edges.insert(temp.begin(), temp.end());
	}

	for (auto x : verticesToDelete)
		eraseVertex(x);


	deleteEdges(edges, update);
}

template <typename Container, typename>
void Mesh::deleteEdges(Container& edges, bool update)
{


	for (DEdge* edge : edges)
	{

		for (DFace* face : edge->getFaces())
			eraseFace(face);

		eraseEdge(edge);

	}

	if (!update)return;

	this->updateEBO();
	this->updateEdgeEBO();
	vbo.bufferData(vertices);

	this->getSelectedVertices().clear();
	edges.clear();
	this->getSelectedFaces().clear();

	FaceBVHSingleton->BuildBottomUp(*this);

	EdgeBVHSingleton->BuildBottomUp(*this);
	VertexBVHSingleton->BuildBottomUp(*this);

}

template <typename Container, typename>
void Mesh::deleteFaces(Container& faces, bool update)
{

	std::unordered_set<DEdge*>edges;

	for (DFace* face : faces)
	{
		std::unordered_set<DEdge*> temp = face->getEdges();
		edges.insert(temp.begin(), temp.end());
	}


	deleteOnlyFaces(faces);


	// deleting edges
	for (DEdge* edge : edges)
	{
		// if it has a face attached 
		if (edge->loop)continue;

		eraseEdge(edge);

	}

	if (!update)return;

	this->updateEBO();
	this->updateEdgeEBO();
	vbo.bufferData(vertices);

	this->getSelectedVertices().clear();
	this->getSelectedEdges().clear();
	this->getSelectedFaces().clear();

	FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	VertexBVHSingleton->BuildBottomUp(*this);

}

template <typename Container, typename>
void Mesh::deleteOnlyEdgesAndFaces(Container& edges, bool update)
{
	for (DEdge* edge : edges)
	{
		for (DFace* face : edge->getFaces())
			eraseFace(face);

		eraseEdge(edge, false);
	}

	if (!update)return;

	this->updateEBO();
	this->updateEdgeEBO();

	this->getSelectedVertices().clear();
	this->getSelectedEdges().clear();
	this->getSelectedFaces().clear();

	FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);

}

template <typename Container, typename>
void Mesh::deleteOnlyFaces(Container& faces, bool update)
//	eraseFace ne radi za ngone nikako
{


	for (auto face : faces)
		eraseFace(face);


	if (!update)return;

	this->updateEBO();

	this->getSelectedVertices().clear();
	faces.clear();

	FaceBVHSingleton->BuildBottomUp(*this);
}





void Mesh::dissolveVertices() {}
void Mesh::dissolveEdges() {}
void Mesh::dissolveFaces() {} // remove shared edges

DEdge* Mesh::edgeFill(std::vector<int>& verts, bool update)
{
	if (verts.size() != 2)return nullptr;

	// Edge fill

	DEdge* e = nullptr;
	if (!this->getEdge(verts[0], verts[1]))
	{
		e = new DEdge(vertices[verts[0]], vertices[verts[1]]);

		edgeIndices.push_back(verts[0]);
		edgeIndices.push_back(verts[1]);
	}

	if (!update) return e;

	this->updateEdgeEBO();
	EdgeBVHSingleton->BuildBottomUp(*this);
	return e;

}


DFace* Mesh::faceFill(std::vector<int>& verts, bool windingOrderSet, bool update)
{
	if (verts.size() < 3)return nullptr;


	std::unordered_set<int> indices = { verts.begin(),verts.end() };
	if (getFace(indices))
	{
		std::cout << "\n\n\t Selected vertices already form a face \t";
		return nullptr;
	}

	// Face fill
	if (!windingOrderSet)
		setWindingOrder(verts);

	DFace* face = new DFace();
	if (verts.size() == 3)
	{
		//std::cerr << "\n\n\tFill 3\t";
		DLoop* l1, * l2, * l3;

		DEdge* e1 = this->getEdge(verts[0], verts[1]);
		if (e1)
		{
			l1 = new DLoop(vertices[verts[1]], e1, face);
			e1->connectLoopToEdge(l1);
		}
		else
		{
			e1 = createEdgeForFill(verts[0], verts[1], face);
			l1 = e1->loop;
		}

		DEdge* e2 = this->getEdge(verts[1], verts[2]);
		if (e2)
		{
			l2 = new DLoop(vertices[verts[2]], e2, face);
			e2->connectLoopToEdge(l2);
		}
		else
		{
			e2 = createEdgeForFill(verts[1], verts[2], face);
			l2 = e2->loop;
		}


		DEdge* e3 = this->getEdge(verts[2], verts[0]);
		if (e3)
		{
			l3 = new DLoop(vertices[verts[0]], e3, face);
			e3->connectLoopToEdge(l3);
		}
		else

		{
			e3 = createEdgeForFill(verts[2], verts[0], face);
			l3 = e3->loop;
		}


		l1->next = l2;l1->prev = l3;
		l2->next = l3;l2->prev = l1;
		l3->next = l1;l3->prev = l2;

		face->loop = l1;


		this->indices.push_back(verts[0]);
		this->indices.push_back(verts[1]);
		this->indices.push_back(verts[2]);
		//std::cerr << "\n\n\tFill 3\t end";
	}
	else if (verts.size() == 4)
	{
		DLoop* l1, * l2, * l3, * l4;

		DEdge* e1 = this->getEdge(verts[0], verts[1]);
		if (e1)
		{
			l1 = new DLoop(vertices[verts[1]], e1, face);
			e1->connectLoopToEdge(l1);
		}
		else
		{
			e1 = createEdgeForFill(verts[0], verts[1], face);
			l1 = e1->loop;
		}

		DEdge* e2 = this->getEdge(verts[1], verts[2]);
		if (e2)
		{
			l2 = new DLoop(vertices[verts[2]], e2, face);
			e2->connectLoopToEdge(l2);
		}
		else
		{
			e2 = createEdgeForFill(verts[1], verts[2], face);
			l2 = e2->loop;
		}


		DEdge* e3 = this->getEdge(verts[2], verts[3]);
		if (e3)
		{
			l3 = new DLoop(vertices[verts[3]], e3, face);
			e3->connectLoopToEdge(l3);
		}
		else

		{
			e3 = createEdgeForFill(verts[2], verts[3], face);
			l3 = e3->loop;
		}

		DEdge* e4 = this->getEdge(verts[3], verts[0]);
		if (e4)
		{
			l4 = new DLoop(vertices[verts[0]], e4, face);
			e4->connectLoopToEdge(l4);
		}
		else
		{
			e4 = createEdgeForFill(verts[3], verts[0], face);
			l4 = e4->loop;
		}

		l1->next = l2; l1->prev = l4;
		l2->next = l3; l2->prev = l1;
		l3->next = l4; l3->prev = l2;
		l4->next = l1; l4->prev = l3;

		face->loop = l1;

		this->indices.push_back(verts[0]);
		this->indices.push_back(verts[1]);
		this->indices.push_back(verts[2]);

		this->indices.push_back(verts[2]);
		this->indices.push_back(verts[3]);
		this->indices.push_back(verts[0]);
	}
	else // ngon fill
	{
		DEdge* e1 = nullptr;
		DLoop* l1 = nullptr;
		std::vector<DLoop*>loopVec(verts.size());
		for (int i = 0;i < verts.size();i++)
		{
			if (i == verts.size() - 1)
			{
				e1 = this->getEdge(verts[i], verts[0]);
				if (e1)
					l1 = new DLoop(vertices[verts[0]], e1, face);
				else
				{
					e1 = createEdgeForFill(verts[i], verts[0], face);
					l1 = e1->loop;
				}


			}
			else
			{
				e1 = this->getEdge(verts[i], verts[i + 1]);
				if (e1)
					l1 = new DLoop(vertices[verts[i + 1]], e1, face);
				else
				{
					e1 = createEdgeForFill(verts[i], verts[i + 1], face);
					l1 = e1->loop;
				}


				// dupli loop creation ako edge ne postoji
				// za onaj bug, delete edges facea, pa fill pa opet delete
			}

			loopVec[i] = l1;

			e1->connectLoopToEdge(l1);

			if (i >= verts.size() - 2)continue;
			this->indices.push_back(verts[0]);
			this->indices.push_back(verts[i + 1]);
			this->indices.push_back(verts[i + 2]);

			//std::cout << "\n\n\tnew triangle\t" << verts[0] << " " << verts[i+1] << " " << verts[i + 2] << "\n";
		}

		face->loop = loopVec.front();

		for (int i = 0;i < loopVec.size();i++)
		{
			if (i == 0)
			{
				loopVec[0]->next = loopVec[1];
				loopVec[0]->prev = loopVec.back();

			}
			else if (i == loopVec.size() - 1)
			{
				loopVec.back()->next = loopVec.front();
				loopVec.back()->prev = loopVec[i - 1];
			}
			else
			{
				loopVec[i]->next = loopVec[i + 1];
				loopVec[i]->prev = loopVec[i - 1];
			}
		}

	}



	if (!update)return face;

	this->updateEBO();
	this->updateEdgeEBO();


	FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	//std::cout << "\n\n\tKRAJ.fill";


	return face;
}

void Mesh::formTrianglesForRaytracing()
{
	triangles.clear();
	triangleMaterialData.clear();

	int rnd = std::rand() % 3;

	for (auto& it : materials)
	{
		for (auto& face : it.second)
		{
			//auto faceVerts = face->getVerticesVector();

			// napravi razliku ako je trokut, quad ili ngon



			// ne radi sada
			// umjesto da ide 0 1 2 , 0 2 3, 0 3 4, itd... ide 012,123,234, itd...

			// duplira trouglove.. ne treba ici puni krug

			DLoop* loop = face->loop;

			DLoop* l0 = loop->prev;
			
			DVertex* t0 = l0->tip;
			
			glm::vec4 point0 = model * glm::vec4(t0->position, 1.0f);

			do {

				DLoop* l1 = loop, * l2 = loop->next;
				DVertex* t1 = l1->tip, * t2 = l2->tip;

				Triangle tri;

				tri.v0x = point0.x;
				tri.v0y = point0.y;
				tri.v0z = point0.z;

				glm::vec4 point1 = model * glm::vec4(t1->position, 1.0f);
				tri.v1x = point1.x;
				tri.v1y = point1.y;
				tri.v1z = point1.z;

				glm::vec4 point2 = model * glm::vec4(t2->position, 1.0f);
				tri.v2x = point2.x;
				tri.v2z = point2.z;
				tri.v2y = point2.y;

				tri.cx = (tri.v0x + tri.v1x + tri.v2x) * 0.33f;
				tri.cy = (tri.v0y + tri.v1y + tri.v2y) * 0.33f;
				tri.cz = (tri.v0z + tri.v1z + tri.v2z) * 0.33f;

				triangles.push_back(tri); // kopija .. koristi emplace back 


				TriangleMaterial triMat;

				triMat.u0 = l0->uvVertex->uv.x;
				triMat.v0 = l0->uvVertex->uv.y;

				triMat.u1 = l1->uvVertex->uv.x;
				triMat.v1 = l1->uvVertex->uv.y;

				triMat.u2 = l2->uvVertex->uv.x;
				triMat.v2 = l2->uvVertex->uv.y;

				triMat.n1x = t0->normal.x;
				triMat.n1y = t0->normal.y;
				triMat.n1z = t0->normal.z;

				triMat.n2x = t1->normal.x;
				triMat.n2y = t1->normal.y;
				triMat.n2z = t1->normal.z;

				triMat.n3x = t2->normal.x;
				triMat.n3y = t2->normal.y;
				triMat.n3z = t2->normal.z;

			
					triMat.mirror = rnd;
					//std::cout << "\n\n\t MIRRORING TRIANGLE MATERIAL DATA \t";
				


				triangleMaterialData.push_back(triMat);

			/*	if(loop==face->loop)
					loop = loop->next;*/


				loop = loop->next;



			} while (loop->next->next != face->loop);

		}
	}
}
void Mesh::eraseFace(DFace* face)
{
	if (!face)return;


	bool flag;

	std::unordered_set<int> setOfVertices;

	for (DVertex* vertex : face->getVertices())
	{
		setOfVertices.insert(this->getVertexIndex(vertex));
	}

	int numberOfTriplets = setOfVertices.size() - 2;

	for (int j = 0; j + 3 * numberOfTriplets <= this->indices.size(); j += 3 * numberOfTriplets)
	{
		flag = false;
		for (int k = j; k < j + 3 * numberOfTriplets; k++)
		{

			if (!setOfVertices.count(this->indices[k]))
			{
				flag = true;
				break;
			}
		}
		if (flag)continue;

		this->indices.erase(this->indices.begin() + j, this->indices.begin() + j + 3 * numberOfTriplets);
		break;
	}

	for (DLoop* l : face->getLoops())
	{
		l->removeLoopFromRadial();
		delete l;
	}

	delete face;

	//mesh->updateEBO();
}

void Mesh::eraseEdge(DEdge* edge, bool vertex)
{


	if (!edge)return;

	std::pair<int, int> indexPair = this->getEdgeIndices(edge);

	for (int i = 0; i < this->edgeIndices.size(); i += 2)
	{
		if ((this->edgeIndices[i] == indexPair.first && this->edgeIndices[i + 1] == indexPair.second) || (this->edgeIndices[i + 1] == indexPair.first && this->edgeIndices[i] == indexPair.second))
		{
			this->edgeIndices.erase(this->edgeIndices.begin() + i, this->edgeIndices.begin() + i + 2);
			//std::cout << "\n\nERASED EDGE " << this->edgeIndices[i] << " " << this->edgeIndices[i + 1] << "\n";

			edge->removeFromDisk();

			if (edge->v1->e == edge)
			{

				if (!edge->d1.next)
				{
					edge->v1->e = nullptr;
					if (vertex)
						eraseVertex(edge->v1);

				}
				else
					edge->v1->e = edge->d1.next;
			}
			if (edge->v2->e == edge)
			{
				if (!edge->d2.next)
				{
					edge->v2->e = nullptr;
					if (vertex)
						eraseVertex(edge->v2);

				}
				else
					edge->v2->e = edge->d2.next;
			}



			delete edge;


			return;

		}
	}
	//std::cout << "\n\n NOT erased edge " << indexPair.first << " " << indexPair.second << "\n";
}

void Mesh::eraseVertex(DVertex* v)
{
	if (!v)return;

	int index = getVertexIndex(v);

	vertices.erase(std::find(vertices.begin(), vertices.end(), v));

	std::cout << "\n\n\t Erased vertex.. number of vertices = " << vertices.size();

	// sanity check
	if (std::find(indices.begin(), indices.end(), index) != indices.end())
		std::cerr << "\n\n mesh.eraseVertex  the vertex index is still inside mesh.indices attribute\n\n";

	if (std::find(edgeIndices.begin(), edgeIndices.end(), index) != edgeIndices.end())
		std::cerr << "\n\n mesh.eraseVertex  the vertex index is still inside mesh.edgeIndices attribute\n\n";


	for (auto& x : indices)
	{
		if (x > index) x--;
	}
	for (auto& x : edgeIndices)
	{
		if (x > index) x--;
	}

}

glm::vec3 Mesh::setWindingOrder(std::vector<int>& verts)
{
	// Compute centroid
	glm::vec3 centroid = glm::vec3(0.0f);
	for (auto x : verts)
		centroid += vertices[x]->position;
	centroid /= static_cast<float>(verts.size());


	// Covariance matrix
	glm::mat3 cov(0.0f);
	for (auto x : verts) {
		glm::vec3 d = vertices[x]->position - centroid;
		cov[0] += d.x * d; // first column
		cov[1] += d.y * d;
		cov[2] += d.z * d;
	}
	cov /= static_cast<float>(verts.size());

	// Find normal = eigenvector of smallest eigenvalue
	// Using cross products only (sufficient for symmetric 3x3)
	glm::vec3 r0(cov[0][0], cov[1][0], cov[2][0]);
	glm::vec3 r1(cov[0][1], cov[1][1], cov[2][1]);
	glm::vec3 r2(cov[0][2], cov[1][2], cov[2][2]);

	glm::vec3 u = glm::cross(r0, r1);
	glm::vec3 v = glm::cross(r0, r2);
	glm::vec3 w = glm::cross(r1, r2);

	// Pick most stable cross as eigenvector
	glm::vec3 normal = (glm::length2(u) > glm::length2(v)) ?
		((glm::length2(u) > glm::length2(w)) ? u : w) :
		((glm::length2(v) > glm::length2(w)) ? v : w);

	normal = glm::normalize(normal);

	// define local 2D frame (u,v) in plane
	u = glm::cross(normal, glm::vec3(0, 0, 1));
	if (glm::length2(u) < 1e-6f)
		u = glm::vec3(1, 0, 0);
	else
		u = glm::normalize(u);
	v = glm::normalize(glm::cross(normal, u));
	/*std::cout << "\n\tNormal vector" << normal.x << " " << normal.y << " " << normal.z;
	std::cout << "\n\tu vector" << u.x << " " << u.y << " " << u.z;
	std::cout << "\n\tv vector" << v.x << " " << v.y << " " << v.z;*/

	std::sort(verts.begin(), verts.end(),
		[&](int a, int b) {
			glm::vec3 offsetA = this->vertices[a]->position - centroid;
			glm::vec3 offsetB = this->vertices[b]->position - centroid;

			float angleA = std::atan2(glm::dot(offsetA, v), glm::dot(offsetA, u));
			float angleB = std::atan2(glm::dot(offsetB, v), glm::dot(offsetB, u));

			return angleA > angleB;
		});



	return normal;

}

glm::vec3 Mesh::setWindingOrder(std::vector<DVertex*>& verts)
{
	// Compute centroid
	glm::vec3 centroid = glm::vec3(0.0f);
	for (auto x : verts)
		centroid += x->position;
	centroid /= static_cast<float>(verts.size());


	// Covariance matrix
	glm::mat3 cov(0.0f);
	for (auto x : verts) {
		glm::vec3 d = x->position - centroid;
		cov[0] += d.x * d; // first column
		cov[1] += d.y * d;
		cov[2] += d.z * d;
	}
	cov /= static_cast<float>(verts.size());

	// Find normal = eigenvector of smallest eigenvalue
	// Using cross products only (sufficient for symmetric 3x3)
	glm::vec3 r0(cov[0][0], cov[1][0], cov[2][0]);
	glm::vec3 r1(cov[0][1], cov[1][1], cov[2][1]);
	glm::vec3 r2(cov[0][2], cov[1][2], cov[2][2]);

	glm::vec3 u = glm::cross(r0, r1);
	glm::vec3 v = glm::cross(r0, r2);
	glm::vec3 w = glm::cross(r1, r2);

	// Pick most stable cross as eigenvector
	glm::vec3 normal = (glm::length2(u) > glm::length2(v)) ?
		((glm::length2(u) > glm::length2(w)) ? u : w) :
		((glm::length2(v) > glm::length2(w)) ? v : w);

	normal = glm::normalize(normal);

	// define local 2D frame (u,v) in plane
	u = glm::cross(normal, glm::vec3(0, 0, 1));
	if (glm::length2(u) < 1e-6f)
		u = glm::vec3(1, 0, 0);
	else
		u = glm::normalize(u);
	v = glm::normalize(glm::cross(normal, u));
	/*std::cout << "\n\tNormal vector" << normal.x << " " << normal.y << " " << normal.z;
	std::cout << "\n\tu vector" << u.x << " " << u.y << " " << u.z;
	std::cout << "\n\tv vector" << v.x << " " << v.y << " " << v.z;*/

	std::sort(verts.begin(), verts.end(),
		[&](DVertex* a, DVertex* b) {
			glm::vec3 offsetA = a->position - centroid;
			glm::vec3 offsetB = b->position - centroid;

			float angleA = std::atan2(glm::dot(offsetA, v), glm::dot(offsetA, u));
			float angleB = std::atan2(glm::dot(offsetB, v), glm::dot(offsetB, u));

			return angleA > angleB;
		});



	return normal;

}


template <typename Container, typename>
void Mesh::setSelectedVertexIndicesFromFaces(const Container& faces)
{

	std::cout << "\n\n\t setVertexIndices with faces works\n\n\t";
	std::unordered_set<int>verts;


	for (auto face : faces)
		for (auto x : face->getVertices())
			verts.insert(getVertexIndex(x));


	selectedVertexIndices.clear();
	selectedVertexIndices.insert(selectedVertexIndices.begin(), verts.begin(), verts.end());


}


template <typename Container, typename>
void Mesh::setSelectedVertexIndicesFromEdges(const Container& edges)
{
	std::cout << "\n\n\t setVertexIndices with faces works\n\n\t";
	std::unordered_set<int>verts;


	for (auto edge : edges)
	{

		verts.insert(getVertexIndex(edge->v1));
		verts.insert(getVertexIndex(edge->v2));
	}

	selectedVertexIndices.clear();
	selectedVertexIndices.insert(selectedVertexIndices.begin(), verts.begin(), verts.end());


}



DEdge* Mesh::createEdgeForFill(int a, int b, DFace* face)
{
	// face BVH  ne radi nesto kako treba
	// nekad hoce nekad nece

	DEdge* e = new DEdge(vertices[a], vertices[b]);

	DLoop* l = new DLoop(e->v2, e, face);

	e->loop = l;

	edgeIndices.push_back(a);
	edgeIndices.push_back(b);

	return e;
}






// ----------------------------
// EXPLICIT INSTANTIATIONS
// ----------------------------

// deleteVertices<int>
template void Mesh::deleteVertices<std::vector<int>>(std::vector<int>&, bool update);
template void Mesh::deleteVertices<std::unordered_set<int>>(std::unordered_set<int>&, bool update);

// deleteEdges<DEdge*>
template void Mesh::deleteEdges<std::vector<DEdge*>>(std::vector<DEdge*>&, bool update);
template void Mesh::deleteEdges<std::unordered_set<DEdge*>>(std::unordered_set<DEdge*>&, bool update);

// deleteFaces<DFace*>
template void Mesh::deleteFaces<std::vector<DFace*>>(std::vector<DFace*>&, bool update);
template void Mesh::deleteFaces<std::unordered_set<DFace*>>(std::unordered_set<DFace*>&, bool update);

// deleteOnlyEdgesAndFaces<DEdge*>
template void Mesh::deleteOnlyEdgesAndFaces<std::vector<DEdge*>>(std::vector<DEdge*>&, bool update);
template void Mesh::deleteOnlyEdgesAndFaces<std::unordered_set<DEdge*>>(std::unordered_set<DEdge*>&, bool update);

// deleteOnlyFaces<DFace*>
template void Mesh::deleteOnlyFaces<std::vector<DFace*>>(std::vector<DFace*>&, bool update);
template void Mesh::deleteOnlyFaces<std::unordered_set<DFace*>>(std::unordered_set<DFace*>&, bool update);




// extrudeEdges
template std::unordered_set<DEdge*>  Mesh::extrudeEdges(std::vector<DEdge*>&, bool);
template std::unordered_set<DEdge*>  Mesh::extrudeEdges(std::unordered_set<DEdge*>&, bool);

// extrudeFaces
template void Mesh::extrudeFaces(std::vector<DFace*>&, bool);
template void Mesh::extrudeFaces(std::unordered_set<DFace*>&, bool);

// extrudeIndividualFaces
template void Mesh::extrudeIndividualFaces(std::vector<DFace*>&, bool);
template void Mesh::extrudeIndividualFaces(std::unordered_set<DFace*>&, bool);



// duplicateEdges
template std::vector<DEdge*> Mesh::duplicateEdges<std::vector<DEdge*>>(std::vector<DEdge*>&, bool);
template std::vector<DEdge*> Mesh::duplicateEdges<std::unordered_set<DEdge*>>(std::unordered_set<DEdge*>&, bool);

// duplicateFaces
template std::vector<DFace*> Mesh::duplicateFaces<std::vector<DFace*>>(std::vector<DFace*>&, bool);
template std::vector<DFace*> Mesh::duplicateFaces<std::unordered_set<DFace*>>(std::unordered_set<DFace*>&, bool);



// separate
template std::vector<DFace*> Mesh::separate<std::vector<DFace*>>(std::vector<DFace*>);
template std::vector<DFace*> Mesh::separate<std::unordered_set<DFace*>>(std::unordered_set<DFace*>);

// pokeFaces
template void Mesh::pokeFaces<std::vector<DFace*>>(std::vector<DFace*>&, bool);
template void Mesh::pokeFaces<std::unordered_set<DFace*>>(std::unordered_set<DFace*>&, bool);

// triangulateFaces
template void Mesh::triangulateFaces<std::vector<DFace*>>(std::vector<DFace*>&, bool);
template void Mesh::triangulateFaces<std::unordered_set<DFace*>>(std::unordered_set<DFace*>&, bool);

// flipFaceNormals
template void Mesh::flipFaceNormals<std::vector<DFace*>>(std::vector<DFace*>&);
template void Mesh::flipFaceNormals<std::unordered_set<DFace*>>(std::unordered_set<DFace*>&);
