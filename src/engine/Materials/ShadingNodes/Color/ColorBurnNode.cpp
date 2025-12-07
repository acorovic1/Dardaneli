#include "ShadingNodes/Color/ColorBurnNode.h"

#include "Material.h"





ColorBurnNode::ColorBurnNode(int n) : ShadingNodes(n) {}

void ColorBurnNode::draw() {
	ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(15, 200, 0, 255));

	ImNodes::BeginNode(id);

	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Color Burn");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(id + 1, ImNodesPinShape_TriangleFilled);
	ImGui::Text("Base Color");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(id + 2, ImNodesPinShape_TriangleFilled);
	ImGui::Text("Blend Color");
	ImNodes::EndInputAttribute();

	ImNodes::BeginOutputAttribute(id + 3, ImNodesPinShape_TriangleFilled);
	ImGui::Text("Out");
	ImNodes::EndOutputAttribute();

	ImNodes::EndNode();

	ImNodes::PopColorStyle();
}


void ColorBurnNode::emitCode(ShaderBuilder& builder, bool visited) {
	int baseId, blendId;
	bool hasBase = app->activeMaterial->getOutputAttributeId(id + 1, baseId);
	bool hasBlend = app->activeMaterial->getOutputAttributeId(id + 2, blendId);

	std::string baseVar = hasBase ? builder.nodeVars[baseId] : "vec3(1.0)";
	std::string blendVar = hasBlend ? builder.nodeVars[blendId] : "vec3(1.0)";

	std::string outVar = "tmp" + std::to_string(builder.tempCounter++);
	builder.body << "vec3 " << outVar << " = 1.0 - (1.0 - " << baseVar << ") / " << blendVar << ";\n";

	builder.nodeVars[id + 1] = outVar;
}

std::vector<int> ColorBurnNode::getInputIds() { return { id + 1, id + 2 }; }


