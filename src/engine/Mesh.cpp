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
			l = l->radialNext;

			if (!l)break;

		} while (l != x->loop);
		//returnSet.insert(x->loop->face);



	}

	returnSet.erase(nullptr);


	return returnSet;
}

std::unordered_set<DEdge*> Mesh::getAllEdges()
{
	std::cout << "\n\t\tMesh.getAllEdges\tVertices size " << vertices.size();
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






void Mesh::duplicateVertex(DVertex& vertex)
{

}


GLuint Mesh::extrudeVertex(GLuint vertex)
{
	return GLuint();
}

void Mesh::deleteVertices()
{
	std::vector<DEdge*>& selectedEdges = this->getSelectedEdges();

	std::unordered_set<DEdge*>edges;
	std::unordered_set<DVertex*>verticesToDelete;
	for (auto x : selectedVertexIndices)
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

	selectedEdges.clear();
	selectedEdges.insert(selectedEdges.begin(), edges.begin(), edges.end());
	deleteEdges();
}
void Mesh::deleteEdges()
{
	std::vector<DEdge*>& selectedEdges = this->getSelectedEdges();
	std::vector<DFace*>& selectedFaces = this->getSelectedFaces();

	for (DEdge* edge : selectedEdges)
	{

		for (DFace* face : edge->getFaces())
			eraseFace(face);

		eraseEdge(edge);

		updateDiskLink(edge, edge->v1, edge->d1);
		updateDiskLink(edge, edge->v2, edge->d2);

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



	this->updateEBO();
	this->updateEdgeEBO();
	VBO.bufferData(vertices);

	this->getSelectedVertices().clear();
	selectedEdges.clear();
	this->getSelectedFaces().clear();

	FaceBVHSingleton->BuildBottomUp(*this);

	EdgeBVHSingleton->BuildBottomUp(*this);
	VertexBVHSingleton->BuildBottomUp(*this);

}
void Mesh::deleteFaces()
{
	std::vector<DFace*>& selectedFaces = this->getSelectedFaces();
	std::unordered_set<DEdge*>selectedEdges;

	std::unordered_set<DEdge*>edgesToDelete;


	for (DFace* face : selectedFaces)
	{
		std::unordered_set<DEdge*> temp = face->getEdges();
		selectedEdges.insert(temp.begin(), temp.end());
	}
	// two bvh build calls ... maybe have a bool as function variable to call it
	deleteOnlyFaces();


	// deleting edges
	for (DEdge* edge : selectedEdges)
	{
		if (edge->loop)continue;

		edgesToDelete.insert(edge);
		eraseEdge(edge);


		updateDiskLink(edge, edge->v1, edge->d1);
		updateDiskLink(edge, edge->v2, edge->d2);
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


	this->updateEBO();
	this->updateEdgeEBO();
	VBO.bufferData(vertices);

	this->getSelectedVertices().clear();
	this->getSelectedEdges().clear();
	this->getSelectedFaces().clear();

	//FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	VertexBVHSingleton->BuildBottomUp(*this);

}
void Mesh::deleteOnlyEdgesAndFaces()
{
	std::vector<DEdge*>& selectedEdges = this->getSelectedEdges();

	for (DEdge* edge : selectedEdges)
	{
		eraseEdge(edge);

		for (DFace* face : edge->getFaces())
		{
			eraseFace(face);
		}

		updateDiskLink(edge, edge->v1, edge->d1);
		updateDiskLink(edge, edge->v2, edge->d2);

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


	for (auto edge : selectedEdges)
		delete edge;

	this->updateEBO();
	this->updateEdgeEBO();

	this->getSelectedVertices().clear();
	selectedEdges.clear();
	this->getSelectedFaces().clear();

	FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);

}
void Mesh::deleteOnlyFaces()
//  kada je zadnji face ostao, izbacuje error ---> GRESKA JE U FACE BVH, 
//	eraseFace ne radi za ngone nikako
{
	std::vector<DFace*>& selectedFaces = this->getSelectedFaces();

	for (auto& face : selectedFaces)
		eraseFace(face);

	this->updateEBO();

	this->getSelectedVertices().clear();
	selectedFaces.clear();

	FaceBVHSingleton->BuildBottomUp(*this);
}

void Mesh::dissolveVertices() {}
void Mesh::dissolveEdges() {}
void Mesh::dissolveFaces() {} // remove shared edges

void Mesh::fill()
{
	if (selectedVertexIndices.size() < 3)return;

	std::unordered_set<int> indices = { selectedVertexIndices.begin(),selectedVertexIndices.end() };
	if (getFace(indices))
	{
		std::cout << "\n\n\t Selected vertices already form a face \t";
		return;
	}

	setWindingOrder();

	std::vector<int>& verts = selectedVertexIndices;

	DFace* face = new DFace();
	if (verts.size() == 3)
	{
		DEdge* e1 = this->getEdge(verts[0], verts[1]);
		if (!e1)
			e1 = createEdgeForFill(verts[0], verts[1], face);

		DEdge* e2 = this->getEdge(verts[1], verts[2]);
		if (!e2)
			e2 = createEdgeForFill(verts[1], verts[2], face);

		DEdge* e3 = this->getEdge(verts[2], verts[0]);
		if (!e3)
			e3 = createEdgeForFill(verts[2], verts[0], face);

		DLoop* l1 = new DLoop(vertices[verts[1]], e1, face);
		DLoop* l2 = new DLoop(vertices[verts[2]], e2, face);
		DLoop* l3 = new DLoop(vertices[verts[0]], e3, face);


		l1->next = l2;l1->prev = l3;
		l2->next = l3;l2->prev = l1;
		l3->next = l1;l3->prev = l2;

		face->loop = l1;

		// connect the structure to the rest of the mesh
		connectLoopToEdge(e1, l1);
		connectLoopToEdge(e2, l2);
		connectLoopToEdge(e3, l3);

		this->indices.push_back(verts[0]);
		this->indices.push_back(verts[1]);
		this->indices.push_back(verts[2]);
	}
	else if (verts.size() == 4)
	{

		DEdge* e1 = this->getEdge(verts[0], verts[1]);
		if (!e1)
			e1 = createEdgeForFill(verts[0], verts[1], face);
		DEdge* e2 = this->getEdge(verts[1], verts[2]);
		if (!e2)
			e2 = createEdgeForFill(verts[1], verts[2], face);
		DEdge* e3 = this->getEdge(verts[2], verts[3]);
		if (!e3)
			e3 = createEdgeForFill(verts[2], verts[3], face);
		DEdge* e4 = this->getEdge(verts[3], verts[0]);
		if (!e4)
			e4 = createEdgeForFill(verts[3], verts[0], face);

		DLoop* l1 = new DLoop(vertices[verts[1]], e1, face);
		DLoop* l2 = new DLoop(vertices[verts[2]], e2, face);
		DLoop* l3 = new DLoop(vertices[verts[3]], e3, face);
		DLoop* l4 = new DLoop(vertices[verts[0]], e4, face);

		l1->next = l2; l1->prev = l4;
		l2->next = l3; l2->prev = l1;
		l3->next = l4; l3->prev = l2;
		l4->next = l1; l4->prev = l3;

		face->loop = l1;

		connectLoopToEdge(e1, l1);
		connectLoopToEdge(e2, l2);
		connectLoopToEdge(e3, l3);
		connectLoopToEdge(e4, l4);

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
				if (!e1)
					e1 = createEdgeForFill(verts[i], verts[0], face);

				l1 = new DLoop(vertices[verts[0]], e1, face);
			}
			else
			{
				e1 = this->getEdge(verts[i], verts[i + 1]);
				if (!e1)
					e1 = createEdgeForFill(verts[i], verts[i + 1], face);

				l1 = new DLoop(vertices[verts[i + 1]], e1, face);
			}

			loopVec[i] = l1;

			connectLoopToEdge(e1, l1);

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

	this->updateEBO();
	this->updateEdgeEBO();


	FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	//std::cout << "\n\n\tKRAJ.fill";
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
		if (l->edge->loop == l)
			l->edge->loop = l->radialNext;

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

void Mesh::updateDiskLink(DEdge* edge, DVertex* vert, DDiskLink& disk)
{


	if (disk.prev == edge && disk.next == edge) // if this is the only edge in the disk
	{
		disk.prev = nullptr;
		disk.next = nullptr;

		return;
	}
	if (disk.prev == disk.next) // if there are only 2 edges in a disk ( and 1 of them will be deleted)
	{

	}

	if (disk.prev)
	{
		if (disk.prev->v1 == vert)
			disk.prev->d1.next = disk.next;
		else if (disk.prev->v2 == vert)
			disk.prev->d2.next = disk.next;

	}

	if (disk.next)
	{
		if (disk.next->v1 == vert)
			disk.next->d1.prev = disk.prev;
		else if (disk.next->v2 == vert)
			disk.next->d2.prev = disk.prev;
	}


}

void Mesh::addEdgeToDisk(DEdge* edge, DEdge* pivotEdge, DVertex* pivot)
{
	DDiskLink& disk = (pivotEdge->v1 == pivot) ? pivotEdge->d1 : pivotEdge->d2;

	if (disk.next)// more than one edge around the vertex
	{
		DEdge* temp = disk.next;

		disk.next = edge;

		if (edge->v1 == pivot)
		{

			edge->d1.next = temp;
			edge->d1.prev = pivotEdge;

		}
		else if (edge->v2 == pivot)
		{
			edge->d2.next = temp;
			edge->d2.prev = pivotEdge;

		}
		else std::cerr << "\n\n\tERROR 1\t addEdgeToDisk\n\n";


		if (temp->v1 == pivot)
		{
			temp->d1.prev = edge;
		}
		else if (temp->v2 == pivot)
		{
			temp->d2.prev = edge;
		}
		else std::cerr << "\n\n\tERROR 2\t addEdgeToDisk\n\n";
	}
	else { // if the pivot vertex only has 1 edge --  pivotEdge

		disk.next = edge;
		disk.prev = edge;


		if (edge->v1 == pivot)
		{

			edge->d1.next = pivotEdge;
			edge->d1.prev = pivotEdge;

		}
		else if (edge->v2 == pivot)
		{
			edge->d2.next = pivotEdge;
			edge->d2.prev = pivotEdge;

		}
		else std::cerr << "\n\n\tERROR 1\t addEdgeToDisk\n\n";


	}


}

void Mesh::setWindingOrder()
{
	// Compute centroid
	glm::vec3 centroid = glm::vec3(0.0f);
	for (auto x : indices)
		centroid += vertices[x]->position;
	centroid /= static_cast<float>(indices.size());


	// Covariance matrix
	glm::mat3 cov(0.0f);
	for (auto x : indices) {
		glm::vec3 d = vertices[x]->position - centroid;
		cov[0] += d.x * d; // first column
		cov[1] += d.y * d;
		cov[2] += d.z * d;
	}
	cov /= static_cast<float>(indices.size());

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

	//glm::vec3 cameraPosition = cameraSingleton->getCamera(0)->getPosition();
	//cameraPosition = glm::normalize(cameraPosition-centroid);
	//float angle = glm::angle(normal, cameraPosition);

	//std::cout << "\n\n\t"<<glm::degrees(angle) << " degrees between normal and camera orientation\n";

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

	std::sort(selectedVertexIndices.begin(), selectedVertexIndices.end(),
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


}

DEdge* Mesh::createEdgeForFill(int a, int b, DFace* face)
{
	// face BVH  ne radi nesto kako treba
	// nekad hoce nekad nece

	DEdge* e = new DEdge();
	e->v1 = vertices[a];
	e->v2 = vertices[b];

	if (e->v1->e)
		addEdgeToDisk(e, e->v1->e, e->v1);
	else e->v1->e = e;

	if (e->v2->e)
		addEdgeToDisk(e, e->v2->e, e->v2);
	else e->v2->e = e;

	//if (!face)std::cout << "\n\n \face is somehow nullptr" << std::flush;
	//else std::cout << "\n\n\tpffff";
	DLoop* l = new DLoop(e->v2, e, face);


	e->loop = l;

	edgeIndices.push_back(a);
	edgeIndices.push_back(b);

	return e;
}

void Mesh::connectLoopToEdge(DEdge* edge, DLoop* loop)
{
	if (!edge->loop)
	{
		edge->loop = loop;

		return;
	}
	if (edge->loop == loop)return;


	if (edge->loop->radialNext)
	{
		DLoop* temp = edge->loop->radialNext;

		edge->loop->radialNext = loop;
		loop->radialNext = temp;

		temp->radialPrev = loop;
		loop->radialPrev = edge->loop;
	}
	else
	{
		edge->loop->radialNext = loop;
		edge->loop->radialPrev = loop;

		loop->radialNext = edge->loop;
		loop->radialPrev = edge->loop;
	}



}


