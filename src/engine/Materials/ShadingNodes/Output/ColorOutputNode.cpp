#include "ShadingNodes/Output/ColorOutputNode.h"
#include "Material.h"

int ColorOutputNode::activeNodeId = -1;

ColorOutputNode::ColorOutputNode(int n) : ShadingNodes(n), enabled(false) {}

void ColorOutputNode::draw()
{

	ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(255, 0, 0, 255));

	ImNodes::BeginNode(id);

	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Color Output");
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

void ColorOutputNode::emitCode(ShaderBuilder& builder, bool visited)
{

	int inputId;
	bool found = app->activeMaterial->getOutputAttributeId(id + 1, inputId);
	std::string color = builder.nodeVars[inputId];

	builder.body << "\tvec3 ColorFinal = vec3(" << color << ".xyz);\n";

}

std::vector<int>ColorOutputNode::getInputIds() { return { id + 1 }; };


