#define _CRT_SECURE_NO_WARNINGS
#pragma once




#include "ShadingNodes/ShadingNodes.h"
#include "ImGUI/ImGuiFileDialog.h"





struct TextureNode : public ShadingNodes {

	std::string filePath;

	std::unique_ptr<Texture> texture;
	GLuint texID;


	TextureNode(int n);

	void draw() override;

	void emitCode(ShaderBuilder& builder, bool visited) override;


	std::vector<int>getInputIds() override;


	std::string getFilePath();
	void generateTexture();


};


std::string getDownloadsPath();