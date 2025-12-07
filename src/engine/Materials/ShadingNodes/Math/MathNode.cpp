#include "ShadingNodes/Math/MathNode.h"
#include "Material.h"
MathNode::MathNode(int n) : ShadingNodes(n) {}


void MathNode::emitCode(ShaderBuilder& builder, bool visited)
{

	const std::vector<std::pair<int, int>>& links = app->activeMaterial->getLinks();


	int inputAId, inputBId;
	bool foundA = app->activeMaterial->getOutputAttributeId(id + 1, inputAId);
	bool foundB = app->activeMaterial->getOutputAttributeId(id + 2, inputBId);

	std::string varA = (foundA) ? builder.nodeVars[inputAId] : "0.5f";
	std::string varB = (foundB) ? builder.nodeVars[inputBId] : "0.5f";
	std::string resultVar = "tmp" + std::to_string(builder.tempCounter++);
	builder.body << "\tfloat " << resultVar << " = " << varA << " + " << varB << ";\n";
	builder.nodeVars[id + 3] = resultVar;



};

void MathNode::draw() {
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
std::vector<int>MathNode::getInputIds() { return { id + 1, id + 2 }; };
