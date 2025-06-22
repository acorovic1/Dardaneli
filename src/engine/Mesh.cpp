#include "Mesh.h"
#include "ObjectModeBVH.h"
#include "VertexBVH.h"
#include "DFace.h"
#include "DLoop.h"
#include "FaceBVH.h"

#include "DVertex.h"


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
		returnSet.insert(x->loop->face);

	returnSet.erase(nullptr);


	return returnSet;
}

std::unordered_set<DEdge*> Mesh::getAllEdges()
{
	std::unordered_set<DEdge*> returnSet;
	for (auto x : *vertices)
	{
		returnSet.insert(x.e->d1.next);
		returnSet.insert(x.e->d1.prev);

		returnSet.insert(x.e->d2.next);
		returnSet.insert(x.e->d2.prev);
	}

	returnSet.erase(nullptr);

	return returnSet;
}

DFace* Mesh::getFace(std::vector<int> indices)
{
	DVertex* vert = &(*vertices)[indices[0]];

	DEdge* edge = vert->e;

	do {
		DLoop* loop = edge->loop;
		do {
			if (std::find(indices.begin(), indices.end(), this->getVertexIndex(loop->tip)) == indices.end())
				break;
			loop = loop->next;
		} while (loop != edge->loop);

		if (loop == edge->loop)
			return loop->face;

		edge = (edge->v1 == vert) ? edge->d1.next : edge->d2.next;
	} while (edge != vert->e);

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
			if ( edge->v2 == v2)
				return edge;

			edge = edge->d1.next;
		}
		else // edge->v2 == v1
		{
			if ( edge->v1 == v2)
				return edge;

			edge = edge->d2.next;
		}
	} while (edge != v1->e);

	std::cerr << "\n\n\n Mesh.getEdge(start,end) returns nullptr\n " << start << " " << end<<"\n\n";
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
	for (int i = 0;i < selectedFaces.size();i++)
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

		}
		else if (faceVertices.size() > 4)
		{
			/*for (auto x : selectedFaces)
				std::cout << " " << x;
				std::cout << "\n" ;*/

				//for (int j = i + 2;j <i+ selectedFaces[i]-1;j++)
			for (int j = 1;j < faceVertices.size() - 1;j++)
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
}
void Mesh::deleteEdges() {}
void Mesh::deleteFaces() {}
void Mesh::deleteOnlyEdgesAndFaces()
{
}
void Mesh::deleteOnlyFaces()
//  kada je zadnji face ostao, izbacuje error ---> GRESKA JE U FACE BVH, 
//	eraseFace ne radi za ngone nikako
{


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

	for (int j = 0;j < this->indices.size();j += 3 * numberOfTriplets)
	{
		flag = false;
		for (int k = j;k < j + 3 * numberOfTriplets;k++)
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

	for (int i = 0;i < this->edgeIndices.size();i += 2)
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

