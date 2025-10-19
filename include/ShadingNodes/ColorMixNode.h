#pragma once
#include "ShadingNodes.h"
#include <Application.h>

struct ColorMixNode : public ShadingNodes {
    int inputAId;
    int inputBId;
    int inputFactorId;
    int outputPinId;

    ColorMixNode(int n) : ShadingNodes(n) {
        inputAId = n + 1;
        inputBId = n + 2;
        inputFactorId = n + 3;
        outputPinId = n + 4;
    }

    std::vector<int> getInputIds() override {
        return { inputAId, inputBId, inputFactorId };
    }

    void emitCode(ShaderBuilder& builder,bool visited) override {
        int aId, bId, fId;
        bool hasA = app->activeMaterial->getOutputAttributeId(inputAId, aId);
        bool hasB = app->activeMaterial->getOutputAttributeId(inputBId, bId);
        bool hasF = app->activeMaterial->getOutputAttributeId(inputFactorId, fId);

        std::string aVar = hasA ? builder.nodeVars[aId] : "vec3(1.0)";
        std::string bVar = hasB ? builder.nodeVars[bId] : "vec3(0.0)";
        std::string fVar = hasF ? builder.nodeVars[fId] : "0.5";

        std::string outVar = "tmp" + std::to_string(builder.tempCounter++);
        builder.body << "vec3 " << outVar << " = mix(" << aVar << ", " << bVar << ", " << fVar << ");\n";

        builder.nodeVars[outputPinId] = outVar;
    }

    void draw() override {
        ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(150, 0, 200, 255)); // purple

        ImNodes::BeginNode(id);
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("Color Mix");
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginInputAttribute(inputAId, ImNodesPinShape_TriangleFilled);
        ImGui::Text("Color A");
        ImNodes::EndInputAttribute();

        ImNodes::BeginInputAttribute(inputBId, ImNodesPinShape_TriangleFilled);
        ImGui::Text("Color B");
        ImNodes::EndInputAttribute();

        ImNodes::BeginInputAttribute(inputFactorId);
        ImGui::Text("Factor");
        ImNodes::EndInputAttribute();

        ImNodes::BeginOutputAttribute(outputPinId, ImNodesPinShape_TriangleFilled);
        ImGui::Text("Out");
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();
        ImNodes::PopColorStyle();
    }
};
