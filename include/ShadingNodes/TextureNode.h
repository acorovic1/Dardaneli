#pragma once


#include "ShadingNodes.h"
#include "ImGUI/ImGuiFileDialog.h"




struct TextureNode : public ShadingNodes {

	std::string filePath;

	std::unique_ptr<Texture> texture;
	GLuint texID;


	TextureNode(int n) : ShadingNodes(n) {}

	std::string getFilePath() { return filePath; }


	void emitCode(ShaderBuilder& builder) override
	{
		std::string var = "tmp" + std::to_string(builder.tempCounter++);
		std::string uniformName = "tex" + std::to_string(texID);


		// insert returns a pair<iterator, bool>, where iterator points to the element 
		// and bool is set according to wheter the elemnt is newly inserted 
		builder.header << "uniform sampler2D " << uniformName << ";\n";
		/*if (builder.declaredUniforms.insert(uniformName).second)
		{
		}*/

		builder.body << "\tvec4 " << var << " = texture(" << uniformName << ", fragUV);\n";
		builder.nodeVars[id+1] = var;

	}

	void generateTexture()
	{


		app->activeMaterial->textures.push_back(std::make_unique<Texture>(filePath.c_str(), app->activeMaterial->getTexUnitCounter(), GL_UNSIGNED_BYTE));
		texID = app->activeMaterial->textures.back().get()->ID;

	}
	std::vector<int>getInputIds() override { return {}; };

	void draw() override {


		ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(101, 67, 33, 255));


		ImNodes::BeginNode(id);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("Texture Node");
		ImNodes::EndNodeTitleBar();

		ImGui::TextWrapped("File: %s", filePath.empty() ? "<none>" : filePath.c_str());

		std::string dlgId = "ChooseTexture##" + std::to_string(id); // unique per node


		if (ImGui::Button("Browse"))
		{
			IGFD::FileDialogConfig cfg;
			cfg.path = ".";
			ImGuiFileDialog::Instance()->OpenDialog(
				dlgId.c_str(),
				"Select Texture",
				"All Images (*.png, *.jpg){.png,.jpg}",
				cfg
			);
		}

		// File dialog update
		if (ImGuiFileDialog::Instance()->Display(dlgId.c_str()))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				filePath = ImGuiFileDialog::Instance()->GetFilePathName();
				generateTexture();
			}
			ImGuiFileDialog::Instance()->Close();
		}
		ImNodes::BeginOutputAttribute(id + 1, ImNodesPinShape_TriangleFilled); // unique pin id
		ImGui::Text("Out");
		ImNodes::EndOutputAttribute();

		ImNodes::EndNode();

		ImNodes::PopColorStyle();
	}
};