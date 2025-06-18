#include "Mesh.h"
#include "ObjectModeBVH.h"
#include "VertexBVH.h"
#include "Face.h"
#include "FaceBVH.h"

Mesh::Mesh(std::string&& name, std::vector <Vertex>* vertices,
	std::vector <GLuint>& indices, const std::vector<GLuint>& edgeIndices, const  std::vector <Texture>& textures) :Object(name) {
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::edgeIndices = edgeIndices;
	Mesh::textures = textures;

	VAO.Bind();
	VBO.bufferData(*Mesh::vertices);
	ebo.bufferData(Mesh::indices);
	edgeEBO.bufferData(Mesh::edgeIndices);

	VAO.LinkAttribute(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttribute(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttribute(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	VAO.LinkAttribute(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	VAO.Unbind();
	VBO.Unbind();
	ebo.Unbind();

	objectBVHSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
}

Mesh::~Mesh() {}

void Mesh::bindEBO() { ebo.Bind(); }
void Mesh::updateEBO() {


	ebo.bufferData(indices);
}

void Mesh::updateEdgeEBO()
{
	edgeEBO.bufferData(edgeIndices);
}

std::vector<Face*> Mesh::getAllFaces()
{
	std::unordered_set<Face*> faceSet;

	for (auto& x : *vertices)
	{
		for (auto& y : x.getAdjecentFaces())
			faceSet.insert(y);

	}
	std::cout << "\nnumber of faces " << faceSet.size();
	return std::vector<Face*>(faceSet.begin(), faceSet.end());

};
std::vector<Edge*> Mesh::getAllEdges()
{
	std::vector<Edge*> edges;

	for (int i = 0; i < edgeIndices.size(); i += 2)
	{
		Vertex* v_start = &(*vertices)[edgeIndices[i]];
		Vertex* v_end = &(*vertices)[edgeIndices[i + 1]];
		Edge* e = v_start->edge;
		Edge* found = nullptr;

		if (!e) continue;

		Edge* start = e;
		do {
			if (e->tip == v_end) {
				found = e;
				break;
			}
			e = e->pair ? e->pair->next : nullptr;
		} while (e && e != start);


		if (!found && v_end->edge) {
			// Try in the reverse direction
			e = v_end->edge;
			start = e;
			do {
				if (e->tip == v_start) {
					found = e->pair; // get the actual edge from v_start to v_end
					break;
				}
				e = e->pair ? e->pair->next : nullptr;
			} while (e && e != start);
		}

		if (found)
			edges.push_back(found);
		else
			std::cerr << "Warning: Edge between " << edgeIndices[i] << " and " << edgeIndices[i + 1] << " not found in half-edge structure.\n";
	}

	return edges;
}

Face* Mesh::getFace(std::vector<int> indices)
{
	if (indices.size() < 3) {
		std::cerr << "MESH::getFace() error: face must have at least 3 vertices\n";
		return nullptr;
	}

	std::unordered_set<Face*> faceSet = (*vertices)[indices[0]].getAdjecentFaces();
	std::unordered_set<Face*> helper = (*vertices)[indices[0]].getAdjecentFaces(); // 
	std::unordered_set<Face*> temp;


	for (int i = 1;i < indices.size();i++)
	{
		temp = (*vertices)[indices[i]].getAdjecentFaces();

		for (auto& x : helper)
		{

			if (temp.count(x) == 0) {
				faceSet.erase(x);
				//std::cout << std::boolalpha << " " << false << " " << x << "\n";
				continue;
			}
			//std::cout << std::boolalpha << " " << true << " " << x << "\n";
		}
	}
	if (!faceSet.size())std::cerr << "\n\nmesh.getFace() empty faceSet!";
	return *faceSet.begin();
}

Edge* Mesh::getEdge(int start, int end)
{
	if (start < 0 || start >= vertices->size() || end < 0 || end >= vertices->size()) {
		std::cerr << "Invalid vertex indices in getEdge(" << start << ", " << end << ")\n";
		return nullptr;
	}

	Vertex* startVertex = &(*vertices)[start];
	Vertex* endVertex = &(*vertices)[end];



	std::vector<Edge*>edges = startVertex->getAdjecentEdges();

	for (auto& x : edges)
		if (x->tip == endVertex)
			return x;

	std::cerr << "\nNo edge with start index " << start << " and end index " << end << "\n";

	return nullptr;
}


GLuint Mesh::extrudeVertex(GLuint vertex)
{
	this->addVertex((*vertices)[vertex]); // this duplicates the vertex
	// recalculate normals potentialy

	edgeIndices.push_back(vertex);				// makes a new edge
	edgeIndices.push_back(vertices->size() - 1);

	edgeEBO.bufferData(edgeIndices);			// updates the edge buffer

	return vertices->size() - 1;
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

std::vector<Edge*>& Mesh::getSelectedEdges()
{
	return selectedEdges;
}

std::vector<Face*>& Mesh::getSelectedFaces()
{

	return selectedFaces;
}

int Mesh::getVertexIndex(Vertex* v)
{
	auto it = std::find_if(vertices->begin(), vertices->end(), [v](const Vertex& vert) {return &vert == v;});
	if (it != vertices->end()) {
		return static_cast<int>(std::distance(vertices->begin(), it));
	}
	else {
		std::cerr << "\n\n		ERROR \n	Mesh.getVertexIndex.. Vertex does not exist";
		return -1; // Not found
	}
}

std::vector<int> Mesh::getFaceIndices(Face* face)
{
	std::vector<Vertex*> vertices = face->getVertices();
	std::vector<int> indices;

	for (auto& x : vertices)
		indices.push_back(this->getVertexIndex(x));

	return indices;
}

std::pair<int, int> Mesh::getEdgeIndices(Edge* edge)
{
	return { this->getVertexIndex(edge->pair->tip), this->getVertexIndex(edge->tip) };
}

std::vector<GLuint> Mesh::formTrianglesForDrawing()
{
	std::vector<GLuint> returnVec = std::vector<GLuint>();
	//std::cout<<"\n";
	for (int i = 0;i < selectedFaces.size();i++)
	{
		std::vector<Vertex*>faceVertices = selectedFaces[i]->getVertices();
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


void Mesh::deleteVertices()
{
	/*Mesh* mesh = static_cast<Mesh*>(objectSingleton->getObject(objectIndices.back()));
	std::vector<int>& selectedVertices = mesh->getSelectedVertices();
	std::vector<Vertex>& meshVertices = mesh->getVerticesReference();


	for (auto& x : selectedVertices)
	{
		std::vector<Face*>faces = meshVertices[x].getAdjecentFaces();
		for (auto& y : faces)
		{
			for (auto& z : y->getEdges())
				z->face = nullptr;
			y->edge = nullptr;
			delete y;
		}
		std::vector<Edge*>edges = meshVertices[x].getAdjecentEdges();

	}*/

}
void Mesh::deleteEdges() {}
void Mesh::deleteFaces() {}
void Mesh::deleteOnlyEdgesAndFaces()
{

	std::vector<Edge*>& selectedEdges = this->getSelectedEdges();

	std::vector<Edge*> edgeNextToNull;
	std::vector<Vertex*> vertexEdgeToNull;


	for (auto& edge : selectedEdges)
	{
		eraseEdge(edge);

		std::vector<Edge*>edgeAdjecent = edge->tip->getAdjecentEdges();
		std::vector<Edge*>edgePairAdjecent = edge->pair->tip->getAdjecentEdges();

		//std::cout << edgeAdjecent.size() << " " << edgePairAdjecent.size() << "\n";

		// remove access to this edge from its previous edge via edge.next
		for (auto x : edgePairAdjecent)
		{
			if (x->pair->next == edge)
			{
				//x->pair->next = nullptr;
				edgeNextToNull.push_back(x->pair);

				break;
			}
		}

		// same as above just for the edge.pair
		for (auto x : edgeAdjecent)
		{
			if (x->pair->next == edge->pair)
			{
				//x->pair->next = nullptr;
				edgeNextToNull.push_back(x->pair);
				break;
			}
		}


		// remove access to the edge via vertex.edge
		if (edge->tip->edge == edge->pair )
		{
			//edge->tip->edge = nullptr;
			vertexEdgeToNull.push_back(edge->tip);
		}

		if (edge->pair->tip->edge == edge)
		{
			//edge->pair->tip->edge = nullptr;
			vertexEdgeToNull.push_back(edge->pair->tip);
		}

		if (edge->face)
		{
			eraseFace(edge->face);
			for (auto& x : edge->face->getEdges())
				x->face = nullptr;
			delete edge->face;
		}

		if (edge->pair->face)
		{
			eraseFace(edge->pair->face);
			for (auto& x : edge->pair->face->getEdges())
				x->face = nullptr;
			delete edge->pair->face;
		}




	}
		
	std::cout << "\n\n \n" << edgeNextToNull.size() << " edges to null\n";
	for (auto x : edgeNextToNull)
		x->next = nullptr;

	for (auto x : vertexEdgeToNull)
		x->edge = nullptr;

	for (auto& edge : selectedEdges)
	{
		delete edge->pair;
		delete edge;
	}

	this->updateEBO();
	this->updateEdgeEBO();

	this->getSelectedVertices().clear();
	selectedEdges.clear();
	this->getSelectedFaces().clear();

	//FaceBVHSingleton->BuildBottomUp(*mesh);
	//EdgeBVHSingleton->BuildBottomUp(*mesh);
	//VertexBVHSingleton->BuildBottomUp(*mesh);
}
void Mesh::deleteOnlyFaces() 
//  kada je zadnji face ostao, izbacuje error ---> GRESKA JE U FACE BVH, 
//	eraseFace ne radi za ngone nikako
{




	std::vector<Face*>& selectedFaces = this->getSelectedFaces();



	for (auto& face : selectedFaces)
	{
		eraseFace(face);
		for (auto& x : face->getEdges())
		{
			x->face = nullptr;

		}
		face->edge = nullptr;
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

void Mesh::eraseFace(Face* face)
{
	if (!face)return;

	std::vector<Vertex*> faceVertices = face->getVertices();
	int size = faceVertices.size();

	bool flag;
	int numberOfTriplets = size - 2;
	std::unordered_set<int> setOfVertices;

	for (int j = 0;j < size;j++)
	{
		setOfVertices.insert(this->getVertexIndex(faceVertices[j]));
	}

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

void Mesh::eraseEdge(Edge* edge)
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



//void Application::deleteOnlyFaces() //  kada je zadnji face ostao, izbacuje error ---> GRESKA JE U FACE BVH 
//{
//
//
//	Mesh* mesh = static_cast<Mesh*>(objectSingleton->getObject(objectIndices.back()));
//
//	std::vector<int>& selectedFaceVertices = mesh->getSelectedFaceIndices();
//	if (!selectedFaceVertices.size())
//		return;
//
//	std::vector<Face*> faces = mesh->getSelectedFaces();
//	for (auto& face : faces)
//	{
//		for (auto& x : face->getEdges())
//		{
//			x->face = nullptr;
//
//		}
//		face->edge = nullptr;
//		delete face;
//	}
//	mesh->updateEBO();
//
//	mesh->getSelectedVertices().clear();
//	selectedFaceVertices.clear();
//
//	FaceBVHSingleton->BuildBottomUp(*mesh);
//
//}