#include "ShadingNodes/Output/MetallicOutputNode.h"
#include "Material.h"

int MetallicOutputNode::activeNodeId = -1;


MetallicOutputNode::MetallicOutputNode(int n) : ShadingNodes(n), enabled(false) {}

void MetallicOutputNode::draw()
{
	ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(255, 0, 0, 255));

	ImNodes::BeginNode(id);

	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Metallic Output");
	ImNodes::EndNodeTitleBar();


	if (ImGui::Checkbox("Active", &enabled))
		if (enabled) activeNodeId = id;
		
	if (activeNodeId != id) enabled = false;


	ImNodes::BeginInputAttribute(id + 1, ImNodesPinShape_Triangle);
	ImGui::Text("Metallic");
	ImNodes::EndInputAttribute();

	ImNodes::EndNode();

	ImNodes::PopColorStyle();
}

void MetallicOutputNode::emitCode(ShaderBuilder& builder, bool visited)
{

	int inputId;
	bool found = app->activeMaterial->getOutputAttributeId(id + 1, inputId);
	std::string metallic = builder.nodeVars[inputId];


	auto shape = ImNodes::GetAttributePinShape(inputId);
	if (shape == ImNodesPinShape_CircleFilled)
		builder.body << "\t float MetallicFinal = " << metallic << ";\n";
	else if (shape == ImNodesPinShape_TriangleFilled)
		builder.body << "\t float MetallicFinal = " << metallic << ".r;\n";

}
std::vector<int>MetallicOutputNode::getInputIds(){	return { id + 1 };};
