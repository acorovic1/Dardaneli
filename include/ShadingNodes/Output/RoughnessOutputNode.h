#pragma once

#include "ShadingNodes.h"
#include <Application.h>

struct RoughnessOutputNode : public ShadingNodes
{
	bool enabled;
	static int activeNodeId;

	RoughnessOutputNode(int n) : ShadingNodes(n), enabled(false) {}

	void emitCode(ShaderBuilder& builder, bool visited) override
	{

		int inputId;
		bool found = app->activeMaterial->getOutputAttributeId(id + 1, inputId);
		std::string roughness = builder.nodeVars[inputId];


		auto shape = ImNodes::GetAttributePinShape(inputId);
		if (shape == ImNodesPinShape_CircleFilled)
			builder.body << "\t float RoughnessFinal = " << roughness << ";\n";
		else if (shape == ImNodesPinShape_TriangleFilled)
			builder.body << "\t float RoughnessFinal = " << roughness << ".r;\n";

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
		ImGui::TextUnformatted("Roughness Output");
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


		ImNodes::BeginInputAttribute(id + 1, ImNodesPinShape_Triangle);
		ImGui::Text("Roughness");
		ImNodes::EndInputAttribute();

		ImNodes::EndNode();

		ImNodes::PopColorStyle();
	}
};


