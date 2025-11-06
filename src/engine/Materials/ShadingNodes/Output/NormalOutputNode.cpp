#include "ShadingNodes/Output/NormalOutputNode.h"

int NormalOutputNode::activeNodeId = -1;


NormalOutputNode::NormalOutputNode(int n) : ShadingNodes(n), enabled(false) {}

void NormalOutputNode::draw()
{
	ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(255, 0, 0, 255));

	ImNodes::BeginNode(id);

	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Normal Output");
	ImNodes::EndNodeTitleBar();


	if (ImGui::Checkbox("Active", &enabled))
		if (enabled) activeNodeId = id;

	if (activeNodeId != id) enabled = false;


	ImNodes::BeginInputAttribute(id + 1, ImNodesPinShape_TriangleFilled);
	ImGui::Text("Color");
	ImNodes::EndInputAttribute();

	ImNodes::EndNode();

	ImNodes::PopColorStyle();
}

void NormalOutputNode::emitCode(ShaderBuilder& builder, bool visited)
{

	int inputId;
	bool found = app->activeMaterial->getOutputAttributeId(id + 1, inputId);
	std::string normal = builder.nodeVars[inputId];

	builder.body << "\tvec3 NormalFinal = vec3(" << normal << ".xyz);\n";

}
std::vector<int>NormalOutputNode::getInputIds() { return { id + 1 }; };
