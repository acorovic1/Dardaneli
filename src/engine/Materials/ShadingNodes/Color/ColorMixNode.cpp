#include "ShadingNodes/Color/ColorMixNode.h"

ColorMixNode::ColorMixNode(int n) : ShadingNodes(n) {}



void ColorMixNode::draw() {
	ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(150, 0, 200, 255)); // purple

	ImNodes::BeginNode(id);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Color Mix");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(id + 1, ImNodesPinShape_TriangleFilled);
	ImGui::Text("Color A");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(id + 2, ImNodesPinShape_TriangleFilled);
	ImGui::Text("Color B");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(id + 3);
	ImGui::Text("Factor");
	ImNodes::EndInputAttribute();

	ImNodes::BeginOutputAttribute(id + 4, ImNodesPinShape_TriangleFilled);
	ImGui::Text("Out");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();
	ImNodes::PopColorStyle();
}


void ColorMixNode::emitCode(ShaderBuilder& builder, bool visited) {
	int aId, bId, fId;
	bool hasA = app->activeMaterial->getOutputAttributeId(id + 1, aId);
	bool hasB = app->activeMaterial->getOutputAttributeId(id + 2, bId);
	bool hasF = app->activeMaterial->getOutputAttributeId(id + 3, fId);

	std::string aVar = hasA ? builder.nodeVars[aId] : "vec3(1.0)";
	std::string bVar = hasB ? builder.nodeVars[bId] : "vec3(0.0)";
	std::string fVar = hasF ? builder.nodeVars[fId] : "0.5";

	std::string outVar = "tmp" + std::to_string(builder.tempCounter++);
	builder.body << "vec3 " << outVar << " = mix(" << aVar << ", " << bVar << ", " << fVar << ");\n";

	builder.nodeVars[id + 4] = outVar;
}

std::vector<int>ColorMixNode::getInputIds() { return { id + 1,id + 2,id + 3 }; }