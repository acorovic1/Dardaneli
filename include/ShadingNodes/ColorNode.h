#pragma once

#include "ShadingNodes.h"
#include <Application.h>


struct ColorNode : public ShadingNodes {
	float color[3] = { 1.0f, 1.0f, 1.0f }; // RGB


	ColorNode(int n) : ShadingNodes(n) {}

	void draw() override {
		ImNodes::BeginNode(id);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("Color Node");
		ImNodes::EndNodeTitleBar();


		ImGui::PushItemWidth(100.0f); // <-- control the width here
		ImGui::ColorEdit3("Color", color);
		if (color[0] < 0)color[0] = 0;
		if (color[1] < 0)color[1] = 0;
		if (color[2] < 0)color[2] = 0;
		if (color[0] > 1)color[0] = 1;
		if (color[1] > 1)color[1] = 1;
		if (color[2] > 1)color[2] = 1;

		ImGui::PopItemWidth();

		// Output pin
		ImNodes::BeginOutputAttribute(id + 1, ImNodesPinShape_TriangleFilled);
		ImGui::Text("Output");
		ImNodes::EndOutputAttribute();

		ImNodes::EndNode();
	}

	void emitCode(ShaderBuilder& builder, bool visited) override {
		std::string varName = "tmp" + std::to_string(builder.tempCounter++);
		builder.nodeVars[id + 1] = varName;
		builder.body << "vec3 " << varName << " = vec3("
			<< color[0] << ", " << color[1] << ", " << color[2] << ");\n";
	}

	std::vector<int> getInputIds() override {
		return {}; // no input
	}
};