#pragma once

#include "algorithm"

#include "ShaderBuilder.h"
#include "ShadingNodes/ShadingNodes.h"
#include "ShadingNodes/Texture/Texture.h"

#include "Enums.h"



class Material {
	std::string name;
	int id = 0;

	std::vector<std::unique_ptr<ShadingNodes>> nodes;

	// output attribute ---> input attribute
	std::vector<std::pair<int, int>> links;

	std::vector<ImVec2>nodePositions;
	std::vector<int> selectedNodes;
	std::vector<int> selectedLinks;


	GLuint texUnitCounter = 0;


public:

	std::vector<std::unique_ptr<Texture>> textures;
	Material(std::string n);


	// errors when the implementation is moved into .cpp
	template<typename T>
	T* createNode(ImVec2 cursorPos) {

		auto node = std::make_unique<T>(id);
		T* ptr = node.get();
		nodes.push_back(std::move(node));


		 
		ImNodes::SetNodeScreenSpacePos(id, cursorPos);

		id += 10;

		return ptr;
	}
	void deleteSelectedNodes();
	void deleteSelectedLinks();

	std::vector<std::pair<int, int>>& getLinks() { return links; }
	const std::vector<std::pair<int, int>>& getLinks()const { return links; }
	std::vector<int >& getSelectedNodes() { return selectedNodes; }
	std::vector<int>& getSelectedLinks() { return selectedLinks; }

	GLuint getTexUnitCounter() { return texUnitCounter++; }
	std::string getName() { return name; }
	void setName(std::string n) { name = n; }

	void setNodePositions();
	void getNodePositions();





	void drawNodes();
	/* takes the id of the input attribute and assigns the id of the output attribute that is connected to it
	  used to find out which output is connected to a given input */
	bool getOutputAttributeId(int inputId, int& outputId);

	ShadingNodes* getNodeById(int outputId);

	void buildShader(ShadingNodes* node, ShaderBuilder& builder, std::unordered_set<int>& visited);


	std::unique_ptr<Shader> compileShader(RenderMode mode);


};
