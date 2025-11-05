#pragma once

#include "ShaderBuilder.h"
#include "ShadingNodes.h"

#include "algorithm"
#include "Texture.h"
#include "EditorModes.h"

// ONE HELL OF A REFACTOR IS NEEDED



class Material {
	std::string name;
	int id = 0;

	std::vector<std::unique_ptr<ShadingNodes>> nodes;
	// output attribute ---> input attribute
	std::vector<std::pair<int, int>> links;
	std::vector<ImVec2>nodePositions;
	std::vector<int> selectedNodes;
	std::vector<int> selectedLinks;



	//ShaderBuilder builder;
	GLuint texUnitCounter = 0;


public:

	std::vector<std::unique_ptr<Texture>> textures;
	Material(std::string n);

	void getNodePositions()
	{
		nodePositions.clear();
		for (auto& n : nodes)
		{
			ImVec2 pos = ImNodes::GetNodeEditorSpacePos(n->getId());
			nodePositions.push_back(pos);
		}
	}

	void setNodePositions()
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			ImNodes::SetNodeEditorSpacePos(nodes[i]->getId(), nodePositions[i]);
		}
	}



	template<typename T>
	T* createNode() {


		auto node = std::make_unique<T>(id);
		T* ptr = node.get();
		nodes.push_back(std::move(node));


		ImVec2 cursorPos = ImGui::GetMousePos();
		ImNodes::SetNodeEditorSpacePos(id, cursorPos);

		id += 10;

		return ptr;
	}

	void deleteSelectedNodes() {


		for (auto& id : selectedNodes)
		{
			links.erase(
				std::remove_if(links.begin(), links.end(),
					[&id,this](const std::pair<int, int>& l) {
						// If any link’s start or end matches any of the node’s attributes, delete it

						
						
						return (l.first - id > 0 && l.first - id < 10) || (l.second - id > 0 && l.second - id < 10);

					}),
				links.end()
			);

		}



		nodes.erase(
			std::remove_if(nodes.begin(), nodes.end(),
				[this](auto& n) {
					return std::find(selectedNodes.begin(), selectedNodes.end(), n->getId()) != selectedNodes.end();
				}),
			nodes.end()
		);
		selectedNodes.clear();




		std::cout << "\n\n\t now there are " << nodes.size() << " nodes\n";
		std::cout << "\n\n\t now there are " << links.size() << " links\n";
	}

	void deleteSelectedLinks()
	{
		// Sort so erase-remove works cleanly
		std::sort(selectedLinks.begin(), selectedLinks.end(), std::greater<int>());

		for (int idx : selectedLinks)
		{
			if (idx >= 0 && idx < static_cast<int>(links.size()))
			{
				links.erase(links.begin() + idx);
			}
		}

		selectedLinks.clear();

		std::cout << "\n\n\t now there are " << links.size() << " links\n";
	}

	std::vector<std::pair<int, int>>& getLinks() {
		return links;
	}
	const std::vector<std::pair<int, int>>& getLinks()const {
		return links;
	}
	std::vector<int >& getSelectedNodes() {
		return selectedNodes;
	}
	std::vector<int>& getSelectedLinks()
	{
		return selectedLinks;
	}

	GLuint getTexUnitCounter() { return texUnitCounter++; }
	std::string getName() { return name; }
	void setName(std::string n) { name = n; }


	void drawNodes() {

		for (auto& n : nodes)
			n->draw();

	}
	/* takes the id of the input attribute and assigns the id of the output attribute that is connected to it
	  used to find out which output is connected to a given input */
	bool getOutputAttributeId(int inputId, int& outputId)
	{
		for (const auto& link : links)
		{
			if (link.second == inputId)
			{
				outputId = link.first;
				return true;
			}
		}
		return false;
	};

	ShadingNodes* getNodeById(int outputId)
	{
		for (auto& n : nodes)
		{
			if (n->getId() == outputId)
				return n.get();
		}
		return nullptr;
	}

	void buildShader(ShadingNodes* node, ShaderBuilder& builder, std::unordered_set<int>& visited)
	{
		if (!node || visited.count(node->getId()))
			return;


		for (auto input : node->getInputIds())
		{
			int connectedId;
			if (getOutputAttributeId(input, connectedId))
			{
				ShadingNodes* upstream = getNodeById(int(connectedId/10)*10);
				buildShader(upstream, builder, visited);
			}
		}

		bool visitedFlag = visited.count(node->getId());
		node->emitCode(builder, visitedFlag);

		visited.insert(node->getId());
	}


	std::unique_ptr<Shader> compileShader(RenderMode mode);
	

};
