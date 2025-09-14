#pragma once

#include "ShadingNodes.h"
#include <Application.h>

struct MetallicOutputNode : public ShadingNodes
{
	bool enabled;
	static int activeNodeId;

	MetallicOutputNode(int n) : ShadingNodes(n), enabled(false) {}

	void emitCode(ShaderBuilder& builder) override
	{

		int inputId;
		bool found = app->activeMaterial->getOutputAttributeId(id + 2, inputId);
		std::string metallic = builder.nodeVars[inputId];

		builder.body << "\MetallicFinal " << metallic << ";\n";

	}
	std::vector<int>getInputIds() override
	{
		return { id + 1 };
	};
	void draw() override
	{
		ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(255, 0, 0, 255));

		ImNodes::BeginNode(id);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("Metallic Output");
		ImNodes::EndNodeTitleBar();


		if (ImGui::Checkbox("Active", &enabled))
		{
			if (enabled)
			{
				activeNodeId = id;
			}
		}

		if (activeNodeId != id)
			enabled = false;


		ImNodes::BeginInputAttribute(id + 1);
		ImGui::Text("Metallic");
		ImNodes::EndInputAttribute();

		ImNodes::EndNode();

		ImNodes::PopColorStyle();
	}
};


int MetallicOutputNode::activeNodeId = -1;