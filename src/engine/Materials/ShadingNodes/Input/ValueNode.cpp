#include "ShadingNodes/Input/ValueNode.h"

ValueNode::ValueNode(int n) : ShadingNodes(n), value(0.5f) {}


void ValueNode::draw()  {
	ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(0, 200, 0, 255));

	ImNodes::BeginNode(id);

	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Value Node");
	ImNodes::EndNodeTitleBar();


	ImGui::PushItemWidth(100.0f);
	ImGui::InputFloat("##val", &value, 0.0f, 0.0f, "%.3f");
	ImGui::PopItemWidth();

	ImNodes::BeginOutputAttribute(id + 1);
	ImGui::Text("Result");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();

	ImNodes::PopColorStyle();
}

void ValueNode::emitCode(ShaderBuilder& builder, bool visited)
{
	std::string varName = "tmp" + std::to_string(builder.tempCounter++);
	builder.body << "\tfloat " << varName << " = " << value << ";\n";
	builder.nodeVars[id + 1] = varName;
};

std::vector<int>ValueNode::getInputIds() { return {}; };
