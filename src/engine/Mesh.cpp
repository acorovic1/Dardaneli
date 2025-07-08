#include "Mesh.h"
#include "DFace.h"
#include "DLoop.h"
#include "DVertex.h"

#include "ObjectModeBVH.h"
#include "VertexBVH.h"
#include "EdgeBVH.h"
#include "FaceBVH.h"



Mesh::Mesh(std::string&& name, std::vector <DVertex>* vertices,
	std::vector <GLuint>& indices, const std::vector<GLuint>& edgeIndices, const  std::vector <Texture>& textures) :Object(name) {
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::edgeIndices = edgeIndices;
	Mesh::textures = textures;

	VAO.Bind();
	VBO.bufferData(*Mesh::vertices);
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

		DLoop* l = x->loop;
		do {

			returnSet.insert(l->face);
			l = l->radialNext;

		} while (l != x->loop);
		returnSet.insert(x->loop->face);



	}

	returnSet.erase(nullptr);


	return returnSet;
}

std::unordered_set<DEdge*> Mesh::getAllEdges()
{
	std::unordered_set<DEdge*> returnSet;
	for (auto& x : *vertices)
	{
		
		std::unordered_set<DEdge*> temp = x.getAdjecentEdges();
		returnSet.insert(temp.begin(), temp.end());
	}

	returnSet.erase(nullptr);

	return returnSet;
}

DFace* Mesh::getFace(std::unordered_set<int> indices)
{


	DVertex* vert = &(*vertices)[*indices.begin()];

	auto temp = vert->getAdjecentFaces();

	for (auto x : temp)
	{
		std::unordered_set<int> faceIndices;
		auto faceVerts = x->getVertices();
		for (auto v : faceVerts)
			faceIndices.insert(this->getVertexIndex(v));
		if (faceIndices == indices)
			return x;
	}

	

	std::cerr << "\n\n\n Mesh.getFace(indices) returns nullptr\n\n";
	return nullptr;
}

DEdge* Mesh::getEdge(int start, int end)
{
	DVertex* v1 = &(*vertices)[start];
	DVertex* v2 = &(*vertices)[end];

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
		glDrawArrays(mode, 0, vertices->size());
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
	vertices->push_back(vertex);
	VBO.bufferData(*vertices);
}


GLuint Mesh::extrudeVertex(GLuint vertex)
{
	return GLuint();
}

void Mesh::deleteVertices()
{
	std::vector<DEdge*>& selectedEdges = this->getSelectedEdges();

	std::unordered_set<DEdge*>edges;
	for (auto x : selectedVertexIndices)
	{
		auto temp = (*vertices)[x].getAdjecentEdges();
		edges.insert(temp.begin(), temp.end());
	}

	selectedEdges.clear();
	selectedEdges.insert(selectedEdges.begin(), edges.begin(), edges.end());
	deleteEdges();
}

void Mesh::deleteEdges()
{
	std::vector<DEdge*>& selectedEdges = this->getSelectedEdges();

	std::vector<DLoop*> loopsToDelete;
	std::vector<DLoop*> loopNextToNull;
	std::vector<DLoop*> loopPrevToNull;


	for (DEdge* edge : selectedEdges)
	{
		eraseEdge(edge);

		DLoop* l = edge->loop;

		std::unordered_set<DEdge*>adjecentEdges;


		do
		{
			if (!l)break;
			if (!l->face) {
				auto temp1 = l->tip->getAdjecentEdges();
				l = l->radialNext;
				adjecentEdges.insert(temp1.begin(), temp1.end());
				continue;
			}

			eraseFace(l->face);
			delete l->face;

			auto temp = l->tip->getAdjecentEdges();
			adjecentEdges.insert(temp.begin(), temp.end());

			DLoop* tempLoop = l;
			loopsToDelete.push_back(l);

			do
			{
				//std::cout << "Hoop\n";
				tempLoop->face = nullptr;
				tempLoop = tempLoop->next;
			} while (tempLoop != l);

			l = l->radialNext;
			//std::cout << "Hi\n";
		} while (l != edge->loop);

		adjecentEdges.erase(edge);


		for (auto x : adjecentEdges)
		{
			if (x->loop->next)
				if (x->loop->next->edge == edge)
				{
					//std::cout << "--next\n";
					//x->loop->next = nullptr;
					loopNextToNull.push_back(x->loop);

				}
			if (x->loop->prev)
				if (x->loop->prev->edge == edge)
				{
					//std::cout << "--prev\n";
					loopPrevToNull.push_back(x->loop);
					//x->loop->prev = nullptr;
				}

			updateDiskLink(edge, edge->v1, edge->d1);
			updateDiskLink(edge, edge->v2, edge->d2);

		}

		if (edge->v1->e == edge)
		{
			std::cout << "\nyooo ho";

			if (!edge->d1.next)
			{
				std::cout << "			whoops";
				eraseVertex(edge->v1);
				continue;
			}
			edge->v1->e = edge->d1.next;
		}
		else if (edge->v2->e == edge)
		{
			std::cout << "\nyoooooo";
			if (!edge->d2.next)
			{
				std::cout << "			whoops";
				eraseVertex(edge->v2);
				continue;
			}
			edge->v2->e = edge->d2.next;
		}

		//std::cout << "Hello\n";
	}



	for (auto loop : loopNextToNull)
		loop->next = nullptr;

	for (auto loop : loopPrevToNull)
		loop->prev = nullptr;

	for (auto edge : selectedEdges)
		delete edge;

	for (auto loop : loopsToDelete)
		delete loop;


	this->updateEBO();
	this->updateEdgeEBO();
	VBO.bufferData(*vertices);

	this->getSelectedVertices().clear();
	selectedEdges.clear();
	this->getSelectedFaces().clear();

	FaceBVHSingleton->BuildBottomUp(*this);
	EdgeBVHSingleton->BuildBottomUp(*this);
	//VertexBVHSingleton->BuildBottomUp(*this);

}

void Mesh::deleteFaces() {


	std::vector<DFace*>& selectedFaces = this->getSelectedFaces();
	std::unordered_set<DEdge*>selectedEdges;

	std::unordered_set<DEdge*>edgesToDelete;
	std::vector<DLoop*> loopsToDelete;

	std::vector<DLoop*> loopNextToNull;
	std::vector<DLoop*> loopPrevToNull;

	for (DFace* face : selectedFaces)
	{
		std::unordered_set<DEdge*> temp = face->getEdges();
		selectedEdges.insert(temp.begin(), temp.end());
	}
	deleteOnlyFaces();


	// deleting edges
	bool flag = false;
	for (DEdge* edge : selectedEdges)
	{

		DLoop* l = edge->loop;
		do {
			if (l->face)
			{
				flag = true;
				break;
			}
			l = l->radialNext;


		} while (l != edge->loop);

		if (flag)
		{
			flag = false;
			continue;
		}

		eraseEdge(edge);
		edgesToDelete.insert(edge);
		std::unordered_set<DEdge*>adjecentEdges;
		do
		{
			auto temp = l->tip->getAdjecentEdges();
			adjecentEdges.insert(temp.begin(), temp.end());

			loopsToDelete.push_back(l);

			l = l->radialNext;

		} while (l != edge->loop);

		adjecentEdges.erase(edge);

		for (auto x : adjecentEdges)
		{
			if (x->loop->next)
				if (x->loop->next->edge == edge)
				{
					//std::cout << "--next\n";
					//x->loop->next = nullptr;
					loopNextToNull.push_back(x->loop);

				}
			if (x->loop->prev)
				if (x->loop->prev->edge == edge)
				{
					//std::cout << "--prev\n";
					loopPrevToNull.push_back(x->loop);
					//x->loop->prev = nullptr;
				}

			updateDiskLink(edge, edge->v1, edge->d1);
			updateDiskLink(edge, edge->v2, edge->d2);

		}

		if (edge->v1->e == edge)
		{

			if (!edge->d1.next)
			{
				eraseVertex(edge->v1);
				continue;
			}
			edge->v1->e = edge->d1.next;
		}
		else if (edge->v2->e == edge)
		{
			if (!edge->d2.next)
			{
				eraseVertex(edge->v2);
				continue;
			}
			edge->v2->e = edge->d2.next;
		}



	}

	for (auto edge : edgesToDelete)
		delete edge;

	for (auto loop : loopNextToNull)
		loop->next = nullptr;

	for (auto loop : loopPrevToNull)
		loop->prev = nullptr;

	for (auto loop : loopsToDelete)
		delete loop;

	this->updateEBO();
	this->updateEdgeEBO();
	VBO.bufferData(*vertices);

	this->getSelectedVertices().clear();
	selectedEdges.clear();
	this->getSelectedFaces().clear();

	//FaceBVHSingleton->BuildBottomUp(*this);
	//EdgeBVHSingleton->BuildBottomUp(*this);
	//VertexBVHSingleton->BuildBottomUp(*this);

}

void Mesh::deleteOnlyEdgesAndFaces()
{
	std::vector<DEdge*>& selectedEdges = this->getSelectedEdges();

	std::vector<DLoop*> loopsToDelete;
	std::vector<DLoop*> loopNextToNull;
	std::vector<DLoop*> loopPrevToNull;


	for (DEdge* edge : selectedEdges)
	{
		eraseEdge(edge);

		DLoop* l = edge->loop;

		std::unordered_set<DEdge*>adjecentEdges;


		do
		{
			if (!l)break;
			if (!l->face) {
				auto temp1 = l->tip->getAdjecentEdges();
				l = l->radialNext;
				adjecentEdges.insert(temp1.begin(), temp1.end());
				continue;
			}

			eraseFace(l->face);
			delete l->face;

			auto temp = l->tip->getAdjecentEdges();
			adjecentEdges.insert(temp.begin(), temp.end());

			DLoop* tempLoop = l;
			loopsToDelete.push_back(l);

			do
			{
				//std::cout << "Hoop\n";
				tempLoop->face = nullptr;
				tempLoop = tempLoop->next;
			} while (tempLoop != l);

			l = l->radialNext;
			//std::cout << "Hi\n";
		} while (l != edge->loop);

		adjecentEdges.erase(edge);


		for (auto x : adjecentEdges)
		{
			if (x->loop->next)
				if (x->loop->next->edge == edge)
				{
					//std::cout << "--next\n";
					//x->loop->next = nullptr;
					loopNextToNull.push_back(x->loop);

				}
			if (x->loop->prev)
				if (x->loop->prev->edge == edge)
				{
					//std::cout << "--prev\n";
					loopPrevToNull.push_back(x->loop);
					//x->loop->prev = nullptr;
				}

			updateDiskLink(edge, edge->v1, edge->d1);
			updateDiskLink(edge, edge->v2, edge->d2);

		}

		if (edge->v1->e == edge)
		{

			edge->v1->e = edge->d1.next;
		}
		else if (edge->v2->e == edge)
		{

			edge->v2->e = edge->d2.next;
		}

		//std::cout << "Hello\n";
	}


	for (auto loop : loopsToDelete)
		delete loop;

	for (auto loop : loopNextToNull)
		loop->next = nullptr;

	for (auto loop : loopPrevToNull)
		loop->prev = nullptr;

	for (auto edge : selectedEdges)
	{

		delete edge;
	}

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
	{
		eraseFace(face);
		for (auto& x : face->getEdges())
		{
			DLoop* l = x->loop;
			while (l->face != face)
			{
				l = l->radialNext;
			}
			l->face = nullptr;
		}
		face->loop = nullptr;
		delete face;
	}

	this->updateEBO();

	this->getSelectedVertices().clear();
	selectedFaces.clear();

	FaceBVHSingleton->BuildBottomUp(*this);
}
void Mesh::dissolveVertices() {}
void Mesh::dissolveEdges() {}
void Mesh::dissolveFaces() {} // remove shared edges

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

	vertices->erase(std::find(vertices->begin(), vertices->end(), *v));

	// sanity check
	if (std::find(indices.begin(), indices.end(), getVertexIndex(v)) != indices.end())
		std::cerr << "\n\n mesh.eraseVertex  the vertex index is still inside mesh.indices attribute\n\n";

	if (std::find(edgeIndices.begin(), edgeIndices.end(), getVertexIndex(v)) != edgeIndices.end())
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

void Mesh::updateDiskLink(DEdge* edge, DVertex* vert, DDiskLink& disk)
{


	if (disk.prev == edge && disk.next == edge) // if this is the only edge in the disk
	{
		disk.prev = nullptr;
		disk.next = nullptr;

		return;
	}

	if (disk.prev)
	{
		if (disk.prev->v1 == vert)
			disk.prev->d1.next = disk.next;
		else if (disk.prev->v2 == vert)
			disk.prev->d2.next = disk.next;


		if (disk.next)
		{
			if (disk.next->v1 == vert)
				disk.next->d1.prev = disk.prev;
			else if (disk.next->v2 == vert)
				disk.next->d2.prev = disk.prev;
		}



	}
}
