#include "ShadingNodes/AmbientOcclusionOutputNode.h"


int AmbientOcclusionOutputNode::activeNodeId = -1;

AmbientOcclusionOutputNode::AmbientOcclusionOutputNode(int n) : ShadingNodes(n), enabled(false) {}

void AmbientOcclusionOutputNode::draw()
{
	ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(255, 0, 0, 255));

	ImNodes::BeginNode(id);

	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("AmbientOcclusion Output");
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
	ImGui::Text("AmbientOcclusion");
	ImNodes::EndInputAttribute();

	ImNodes::EndNode();

	ImNodes::PopColorStyle();
}

void AmbientOcclusionOutputNode::emitCode(ShaderBuilder& builder, bool visited)
{

	int inputId;
	bool found = app->activeMaterial->getOutputAttributeId(id + 1, inputId);
	std::string ao = builder.nodeVars[inputId];

	auto shape = ImNodes::GetAttributePinShape(inputId);
	if (shape == ImNodesPinShape_CircleFilled)
		builder.body << "\t float AoFinal = " << ao << ";\n";
	else if (shape == ImNodesPinShape_TriangleFilled)
		builder.body << "\t float AoFinal = " << ao << ".r;\n";

}
std::vector<int>AmbientOcclusionOutputNode::getInputIds() { return { id + 1 }; };
