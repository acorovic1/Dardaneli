#pragma once

#include "ShadingNodes.h"
#include <Application.h>

struct ColorOutputNode : public ShadingNodes
{
	bool enabled;
	static int activeNodeId;

	ColorOutputNode(int n) : ShadingNodes(n),enabled(false) {}

	void emitCode(ShaderBuilder& builder) override
	{

		int inputId;
		bool found = app->activeMaterial->getOutputAttributeId(id + 1, inputId);
		std::string color = builder.nodeVars[inputId];

		builder.body << "\tvec3 ColorFinal = vec3(" << color<<".xyz);\n";

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
		ImGui::TextUnformatted("Color Output");
		ImNodes::EndNodeTitleBar();


		if (ImGui::Checkbox("Active", &enabled))
		{
			if (enabled)
			{
				activeNodeId = id;

				std::cout << "\n\t\tShape is "<<ImNodes::GetAttributePinShape(id + 1);

			}
		}

		if (activeNodeId != id)
			enabled = false;

		
		ImNodes::BeginInputAttribute(id + 1, ImNodesPinShape_TriangleFilled);
		ImGui::Text("Color");
		ImNodes::EndInputAttribute();

		ImNodes::EndNode();

		ImNodes::PopColorStyle();
	}
};


