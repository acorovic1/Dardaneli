#include "Material.h"
#include "MaterialManager.h"

#include "ShadingNodes/Output/ColorOutputNode.h"
#include "ShadingNodes/Output/NormalOutputNode.h"
#include "ShadingNodes/Output/MetallicOutputNode.h"
#include "ShadingNodes/Output/RoughnessOutputNode.h"
#include "ShadingNodes/Output/AmbientOcclusionOutputNode.h"

const std::string pbrFunctions = R"(

struct Light {
    int type;           // 0 = directional, 1 = point, 2 = spot
    vec3 position;      // for point & spot
    vec3 direction;     // for directional & spot
    vec3 color;
    float intensity;
    float innerCutoff;  // spotlight
    float outerCutoff;  // spotlight
};

uniform Light lights[32];
uniform int numLights;  // actual number of lights active

uniform vec3 camPos;

in mat3 TBN;

const float PI = 3.14159265359;

vec3 getNormalFromMap(vec3 norm)
{
    vec3 tangentNormal = norm * 2.0 - 1.0;

    return normalize(TBN * tangentNormal);
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

vec3 lightRadiance(Light l, vec3 fragPos)
{
    vec3 radiance = vec3(0.0);

    if (l.type == 0) {
        // Directional light
        // no attenuation, no position-based falloff
        radiance = l.color * l.intensity;

    } else if (l.type == 1) {
        // Point light
        float distance = length(l.position - fragPos);
        float attenuation = 1.0 / (distance * distance); // inverse-square falloff
        radiance = l.color * l.intensity * attenuation;

    } else if (l.type == 2) {
        // Spot light
        vec3 toLight = l.position - fragPos;
        float distance = length(toLight);
        vec3 L = normalize(toLight);

        float attenuation = 1.0 / (distance * distance);

        // spotlight intensity falloff
        float theta = dot(L, normalize(-l.direction)); // angle between light dir and fragment
        float epsilon = l.innerCutoff - l.outerCutoff;
        float spotFactor = clamp((theta - l.outerCutoff) / epsilon, 0.0, 1.0);

        radiance = l.color * l.intensity * attenuation * spotFactor;
    }

    return radiance;
}




vec3 pbr(vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, float ao)
{
   
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < numLights; ++i) 
    {
        // calculate per-light radiance
        vec3 L;
        if (lights[i].type == 0) {
            // directional
            L = normalize(-lights[i].direction);
           // L = normalize(-lights[i].direction);
        } else {
            // point & spot
            L = normalize(lights[i].position - WorldPos);
        }
        vec3 H = normalize(V + L);

 
        vec3 radiance = lightRadiance(lights[i],WorldPos);

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 


    return color;
}

)";




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


std::unique_ptr<Shader> Material::compileShader(RenderMode mode)
{
	std::stringstream baseHeader;
	ShaderBuilder builder;



	baseHeader
		<< "\n#version 460 core\n\n"
		<< "in vec2 fragUV;\n"
		<< "in vec3 WorldPos;\n"
		<< "in vec3 Normal;\n"
		<< "out vec4 FragColor;\n";

	if (mode == RenderMode::RENDER)
		baseHeader << pbrFunctions;


	std::unordered_set<int> visited;
	for (auto& n : nodes)
	{
		if (dynamic_cast<ColorOutputNode*>(n.get()))
		{
			if (dynamic_cast<ColorOutputNode*>(n.get())->enabled)
			{
				buildShader(n.get(), builder, visited);

				builder.body << "\n\n";

			}
		}

		if (dynamic_cast<NormalOutputNode*>(n.get()))
		{
			if (dynamic_cast<NormalOutputNode*>(n.get())->enabled)
			{
				buildShader(n.get(), builder, visited);
				builder.body << "\n\n";

			}
		}


		if (dynamic_cast<RoughnessOutputNode*>(n.get()))
		{
			if (dynamic_cast<RoughnessOutputNode*>(n.get())->enabled)
			{
				buildShader(n.get(), builder, visited);
				builder.body << "\n\n";

			}
		}

		if (dynamic_cast<MetallicOutputNode*>(n.get()))
		{
			if (dynamic_cast<MetallicOutputNode*>(n.get())->enabled)
			{
				buildShader(n.get(), builder, visited);
				builder.body << "\n\n";

			}
		}

		if (dynamic_cast<AmbientOcclusionOutputNode*>(n.get()))
		{
			if (dynamic_cast<AmbientOcclusionOutputNode*>(n.get())->enabled)
			{
				buildShader(n.get(), builder, visited);
				builder.body << "\n\n";

			}
		}

	}


	std::string builderBody = builder.body.str();

	std::stringstream shaderCode;

	//if an output is not defined, define it with a default value

	if (builderBody.find("ColorFinal") == std::string::npos)
		builder.body << "\tvec3 ColorFinal = vec3(1.0,1.0,1.0);\n";

	if (builderBody.find("NormalFinal") == std::string::npos)
		builder.body << "\tvec3 NormalFinal = normalize(Normal);\n";

	if (builderBody.find("RoughnessFinal") == std::string::npos)
		builder.body << "\tfloat RoughnessFinal = 0.5;\n";

	if (builderBody.find("MetallicFinal") == std::string::npos)
		builder.body << "\tfloat MetallicFinal = 0.1;\n";

	if (builderBody.find("AoFinal") == std::string::npos)
		builder.body << "\tfloat AoFinal = 1.0;\n";


	shaderCode
		<< baseHeader.str() << "\n\n"
		<< builder.header.str() << "\n\n"
		<< "\nvoid main() \n{\n"
		<< builder.body.str() << "\n\n";

	if (mode == RenderMode::RENDER)
		shaderCode << "\tFragColor = vec4(pbr(getNormalFromMap(NormalFinal), normalize(camPos - WorldPos), ColorFinal, MetallicFinal, RoughnessFinal, AoFinal), 1.0);\n}\n";
	//<< "    FragColor = vec4(pbr(NormalFinal, normalize(camPos - WorldPos), ColorFinal, MetallicFinal, RoughnessFinal, AoFinal), 1.0);\n"
	else shaderCode << "FragColor = vec4(ColorFinal,1.0);\n}\n";




	std::string shaderCodeString = shaderCode.str();

	static bool firstTime = true;

	if (firstTime)
	{

		std::cout << "\n\n========================\n" << shaderCodeString << "\n========================\n\n";
		firstTime = false;
	}
	return std::make_unique<Shader>(name, "pbr.vert", shaderCodeString.c_str(), "pbr.geom");

}



void Material::deleteSelectedNodes()
{


	for (auto& id : selectedNodes)
	{
		links.erase(
			std::remove_if(links.begin(), links.end(),
				[&id, this](const std::pair<int, int>& l) {
					// If any link’s start or end matches any of the node’s attributes, delete it



					return (l.first - id > 0 && l.first - id < 10) || (l.second - id > 0 && l.second - id < 10);

				}),
			links.end()
		);

	}



	nodes.erase(
		std::remove_if(nodes.begin(), nodes.end(),
			[this](auto& n) {
				return std::find(selectedNodes.begin(), selectedNodes.end(), n->getId()) != selectedNodes.end();
			}),
		nodes.end()
	);
	selectedNodes.clear();




	std::cout << "\n\n\t now there are " << nodes.size() << " nodes\n";
	std::cout << "\n\n\t now there are " << links.size() << " links\n";
}

void Material::deleteSelectedLinks()
{
	// Sort so erase-remove works cleanly
	std::sort(selectedLinks.begin(), selectedLinks.end(), std::greater<int>());

	for (int idx : selectedLinks)
	{
		if (idx >= 0 && idx < static_cast<int>(links.size()))
		{
			links.erase(links.begin() + idx);
		}
	}

	selectedLinks.clear();

	std::cout << "\n\n\t now there are " << links.size() << " links\n";
}

void Material::setNodePositions()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		ImNodes::SetNodeEditorSpacePos(nodes[i]->getId(), nodePositions[i]);
	}
}

void Material::getNodePositions()
{
	nodePositions.clear();
	for (auto& n : nodes)
	{
		ImVec2 pos = ImNodes::GetNodeEditorSpacePos(n->getId());
		nodePositions.push_back(pos);
	}
}

void Material::drawNodes() {

	for (auto& n : nodes)
		n->draw();

}

bool Material::getOutputAttributeId(int inputId, int& outputId)
{
	for (const auto& link : links)
	{
		if (link.second == inputId)
		{
			outputId = link.first;
			return true;
		}
	}
	return false;
};

ShadingNodes* Material::getNodeById(int outputId)
{
	for (auto& n : nodes)
	{
		if (n->getId() == outputId)
			return n.get();
	}
	return nullptr;
}

void Material::buildShader(ShadingNodes* node, ShaderBuilder& builder, std::unordered_set<int>& visited)
{
	if (!node || visited.count(node->getId()))
		return;


	for (auto input : node->getInputIds())
	{
		int connectedId;
		if (getOutputAttributeId(input, connectedId))
		{
			ShadingNodes* upstream = getNodeById(int(connectedId / 10) * 10);
			buildShader(upstream, builder, visited);
		}
	}

	bool visitedFlag = visited.count(node->getId());
	node->emitCode(builder, visitedFlag);

	visited.insert(node->getId());
}



