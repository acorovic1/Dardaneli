#include "ShadingNodes/Material.h"
#include "ShadingNodes/MaterialManager.h"
#include "ShadingNodes/ColorOutputNode.h"


Material::Material(std::string n) {

	int counter = 0;
	std::string tempName = n;
	for (Material* mat : materialSingleton->getAllMaterials())
	{
		if (mat->getName() == n) {
			tempName = n + "." + std::to_string(++counter);
			break;
		}
	}

	if (counter > 1)
		name = tempName;
	else
		name = n;

	materialSingleton->addMaterial(this);

}

std::unique_ptr<Shader> Material::compileShader()
{
	std::stringstream baseHeader;
	ShaderBuilder color;

	//layout(location = 1) in vec3 aNormal;
	// Emit common header
	baseHeader
		<< "\n#version 460 core\n\n"
		<< "in vec2 fragUV;\n"
		<< "out vec4 FragColor;\n";


	std::unordered_set<int> visited;
	for (auto& n : nodes)
	{
		if (dynamic_cast<ColorOutputNode*>(n.get()))
		{
			if (dynamic_cast<ColorOutputNode*>(n.get())->enabled)
			{
				buildShader(n.get(), color, visited);
				break;
			}
		}

	}

	std::string colorContent = color.body.str();
	std::stringstream shaderCode;
	if (colorContent.find("ColorFinal") != std::string::npos)
	{
		shaderCode
			<< baseHeader.str() << "\n\n"
			<< color.header.str() << "\n\n"
			<< "\nvoid main() \n{\n"
			<< colorContent
			<< "    FragColor = " << "vec4(ColorFinal,1.0)" << ";\n"
			<< "}\n";
	}
	else
	{
		shaderCode
			<< baseHeader.str() << "\n\n"
			<< color.header.str() << "\n\n"
			<< "\nvoid main() \n{\n"

			<< "    FragColor = vec4(1.0,1.0,1.0,1.0);\n"
			<< "}\n";
	}


	



	std::cout << shaderCode.str();

	std::string shaderCodeString = shaderCode.str();
	return std::make_unique<Shader>(name, "src/shaders/basic.vert", shaderCodeString.c_str());

}
