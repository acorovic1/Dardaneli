#include "unordered_map"


#include "Mesh.h"
#include "DFace.h"
#include "DLoop.h"
#include "DEdge.h"
#include "DVertex.h"

#include "ObjectModeBVH.h"
#include "VertexBVH.h"
#include "EdgeBVH.h"
#include "FaceBVH.h"
#include "CameraManager.h"
#include "UnorderedPair.h"





Mesh::Mesh(std::string&& name, std::vector <DVertex*> vertices,
	std::vector <GLuint>& indices, const std::vector<GLuint>& edgeIndices, const  std::vector <Texture>& textures) :Object(name) {
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::edgeIndices = edgeIndices;
	Mesh::textures = textures;

	VAO.Bind();
	VBO.bufferData(Mesh::vertices);
	ebo.bufferData(Mesh::indices);
	edgeEBO.bufferData(Mesh::edgeIndices);

	VAO.LinkAttribute(VBO, 0, 3, GL_FLOAT, sizeof(DVertex), (void*)0);
	VAO.LinkAttribute(VBO, 1, 3, GL_FLOAT, sizeof(DVertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttribute(VBO, 2, 3, GL_FLOAT, sizeof(DVertex), (void*)(6 * sizeof(float)));
	VAO.LinkAttribute(VBO, 3, 2, GL_FLOAT, sizeof(DVertex), (void*)(9 * sizeof(float)));

	VAO.Unbind();
	VBO.Unbind();
	ebo.Unbind();

	objectBVHSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
}

Mesh::~Mesh() {}

void Mesh::bindEBO() { ebo.Bind(); }
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
		std::unordered_set<int> faceIndices;

		auto faceVerts = face->getVertices();

		bool found = true;
		for (auto v : faceVerts)
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

	std::cerr << "\n\n\n Mesh.getEdge(start,end) returns nullptr\n " << start << " " << end << "\n\n";
	return nullptr;
}





void Mesh::Draw(Shader& shader, Camera& camera, GLenum mode) {
	shader.Activate();
	VAO.Bind();

	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	for (unsigned int i = 0; i < textures.size(); i++) {
		std::string num;
		std::string type = textures[i].type;

		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}

		textures[i].textureUniform(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}
	//shader.setVector3f(false, "camPos", camera.Position);
	camera.CameraUniform(shader, "cameraMatrix");
	shader.setVector3f(true, "camPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	shader.setMat4(true, "model", model);

	if (mode == GL_TRIANGLES)
	{
		ebo.Bind();
		glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else if (mode == GL_LINES)
	{
		edgeEBO.Bind();
		glDrawElements(mode, edgeIndices.size(), GL_UNSIGNED_INT, 0);
	}
	else if (mode == GL_POINTS)
	{
		VBO.Bind();
		glDrawArrays(mode, 0, vertices.size());
	}

	VAO.Unbind();
}

void Mesh::Translate(glm::vec3& translateVector)
{
	model = glm::translate(model, translateVector);
}
void Mesh::Translate(float x, float y, float z)
{
	model = glm::translate(model, glm::vec3(x, y, z));
}

void Mesh::Rotate(float degrees, const glm::vec3& axisVector)
{
	model = glm::rotate(model, glm::radians(degrees), axisVector);
}

void Mesh::Scale(glm::vec3& scaleVector)
{
	model = glm::scale(model, scaleVector);
}
void Mesh::Scale(float x, float y, float z) {
	model = glm::scale(model, glm::vec3(x, y, z));
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


	VBO.bufferData(vertices);

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
	VBO.bufferData(vertices);

	//this->getSelectedVertices().clear();


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
	auto camera = cameraSingleton->getCamera(0);
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

std::vector<int>& Mesh::getSelectedVertices() {
	return selectedVertexIndices;
}

std::vector<DEdge*>& Mesh::getSelectedEdges()
{
	return selectedEdges;
}

std::vector<DFace*>& Mesh::getSelectedFaces()
{

	return selectedFaces;
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
			returnVec.push_back(this->getVertexIndex(faceVertices[0]));
			returnVec.push_back(this->getVertexIndex(faceVertices[3]));

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
	VBO.bufferData(vertices);

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

	VBO.bufferData(vertices);
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

template<typename Container,typename >
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

		for (int j = 0; j < this->indices.size(); j += 3 * numberOfTriplets)
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

			for (int i = 0;i < 3 * numberOfTriplets;i+=3)
			{
				std::swap(*(this->indices.begin() + j + i), *(this->indices.begin() + j + i + 2));
			}

			//newIndices.insert(newIndices.begin(), this->indices.begin() + j, this->indices.begin() + j + 3 * numberOfTriplets);
			//this->indices.erase(this->indices.begin() + j, this->indices.begin() + j + 3 * numberOfTriplets);


			break;  /// dodaj ovaj break i u eraseFace

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
	VBO.bufferData(vertices);
	updateEBO();

	VertexBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);


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
	VBO.bufferData(vertices);

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
	VBO.bufferData(vertices);

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

	VBO.bufferData(vertices);
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
	VBO.bufferData(vertices);

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

	VBO.bufferData(vertices);
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

	VBO.bufferData(vertices);
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
			eraseEdge(*temp.begin());
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

		edge->removeFromDisk();

		if (edge->v1->e == edge)
		{

			if (!edge->d1.next)
			{
				edge->v1->e = nullptr;
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
				eraseVertex(edge->v2);

			}
			else
				edge->v2->e = edge->d2.next;
		}

		delete edge;
	}



	if (!update)return;

	this->updateEBO();
	this->updateEdgeEBO();
	VBO.bufferData(vertices);

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

	std::unordered_set<DEdge*>selectedEdges;

	std::unordered_set<DEdge*>edgesToDelete;


	for (DFace* face : faces)
	{
		std::unordered_set<DEdge*> temp = face->getEdges();
		selectedEdges.insert(temp.begin(), temp.end());
	}


	deleteOnlyFaces(faces);


	// deleting edges
	for (DEdge* edge : selectedEdges)
	{
		if (edge->loop)continue;

		edgesToDelete.insert(edge);
		eraseEdge(edge);

		edge->removeFromDisk();

		if (edge->v1->e == edge)
		{

			if (!edge->d1.next)
			{

				edge->v1->e = nullptr;
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
				eraseVertex(edge->v2);

			}
			else
				edge->v2->e = edge->d2.next;
		}



	}
	//std::cout << "\n\n\t edgesToDelete " << edgesToDelete.size();
	for (auto edge : edgesToDelete)
		delete edge;

	if (!update)return;

	this->updateEBO();
	this->updateEdgeEBO();
	VBO.bufferData(vertices);

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
		eraseEdge(edge);

		for (DFace* face : edge->getFaces())
		{
			eraseFace(face);
		}

		edge->removeFromDisk();

		if (edge->v1->e == edge)
		{

			if (!edge->d1.next)
				edge->v1->e = nullptr;
			else
				edge->v1->e = edge->d1.next;
		}
		if (edge->v2->e == edge)
		{
			if (!edge->d2.next)
				edge->v2->e = nullptr;
			else
				edge->v2->e = edge->d2.next;
		}

	}


	for (auto edge : edges)
		delete edge;

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

DEdge* Mesh::edgeFill(std::vector<int>& verts)
{
	if (verts.size() != 2)return nullptr;

	// Edge fill

	DEdge* e = nullptr;
	if (!this->getEdge(verts[0], verts[1]))
	{
		e = new DEdge();
		e->v1 = vertices[verts[0]];
		e->v2 = vertices[verts[1]];

		if (e->v1->e)
			e->addToDisk(e->v1->e, e->v1);
		else e->v1->e = e;

		if (e->v2->e)
			e->addToDisk(e->v2->e, e->v2);
		else e->v2->e = e;

		edgeIndices.push_back(verts[0]);
		edgeIndices.push_back(verts[1]);
	}
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
		std::cerr << "\n\n\tFill 3\t";
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
		std::cerr << "\n\n\tFill 3\t end";
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

	for (int j = 0; j < this->indices.size(); j += 3 * numberOfTriplets)
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

	}

	for (DLoop* l : face->getLoops())
	{
		l->removeLoopFromRadial();
		delete l;
	}

	delete face;

	//mesh->updateEBO();
}

void Mesh::eraseEdge(DEdge* edge)
{


	if (!edge)return;

	std::pair<int, int> indexPair = this->getEdgeIndices(edge);

	for (int i = 0; i < this->edgeIndices.size(); i += 2)
	{
		if ((this->edgeIndices[i] == indexPair.first && this->edgeIndices[i + 1] == indexPair.second) || (this->edgeIndices[i + 1] == indexPair.first && this->edgeIndices[i] == indexPair.second))
		{
			std::cout << "\n\nERASED EDGE " << this->edgeIndices[i] << " " << this->edgeIndices[i + 1] << "\n";

			this->edgeIndices.erase(this->edgeIndices.begin() + i, this->edgeIndices.begin() + i + 2);
			return;

		}
	}
	std::cout << "\n\n NOT erased edge " << indexPair.first << " " << indexPair.second << "\n";
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



	/// belaj pravi brisanje iz vektora jer se pomjere ostali clanovi vektora i onda pokazivaci ne valjaju kurcu
	// ili koristiti deck ili naci neki drugi nacin ( mozda da nije *vector vec nesta drugo)

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


	/*std::cout << "\n\tsorted\t ";
	for (auto x : selectedVertexIndices)
		std::cout << " " << x;*/

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
