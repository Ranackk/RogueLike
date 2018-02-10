#pragma once
#include "Material.h"

class MaterialManager
{
private:
	std::map<std::string, std::shared_ptr<Material>> m_s_Materials;
public:
	MaterialManager();
	~MaterialManager();

	GLuint m_Skybox;

	std::shared_ptr<Material> getMaterialByName(const std::string& materialName);
	bool createMaterialByShader(const std::string& filePath, const std::string& materialName, const Material::Type materialType);
};
