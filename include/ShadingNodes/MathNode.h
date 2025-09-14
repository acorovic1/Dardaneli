#pragma once
#include "ShadingNodes.h"


struct MathNode : public ShadingNodes {





	MathNode(int n) : ShadingNodes(n) {}




	void emitCode(ShaderBuilder& builder) override
	{

		const std::vector<std::pair<int, int>>& links = app->activeMaterial->getLinks();

		// these input variables are some nodes' output variables
		int inputAId, inputBId;
		bool foundA = app->activeMaterial->getOutputAttributeId(id + 1, inputAId);
		bool foundB = app->activeMaterial->getOutputAttributeId(id + 2, inputBId);

		std::string varA = (foundA) ? builder.nodeVars[inputAId] : "0.5f";
		std::string varB = (foundB) ? builder.nodeVars[inputBId] : "0.5f";
		std::string resultVar = "tmp" + std::to_string(builder.tempCounter++);
		builder.body << "\tfloat " << resultVar << " = " << varA << " + " << varB << ";\n";
		builder.nodeVars[id + 3] = resultVar;



	};
	std::vector<int>getInputIds() override
	{
		return { id + 1, id + 2 };
	};
	void draw() override {
		ImNodes::BeginNode(id);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("Math Node");
		ImNodes::EndNodeTitleBar();

		ImNodes::BeginInputAttribute(id + 1);
		ImGui::Text("A");
		ImNodes::EndInputAttribute();

		ImNodes::BeginInputAttribute(id + 2);
		ImGui::Text("B");
		ImNodes::EndInputAttribute();

		ImNodes::BeginOutputAttribute(id + 3);
		ImGui::Text("Result");
		ImNodes::EndOutputAttribute();

		ImNodes::EndNode();
	}
};