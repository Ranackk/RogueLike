#include "stdafx.h"
#include "MaterialManager.h"
#include "Game.h"


MaterialManager::MaterialManager()
{
	m_s_Materials = std::map<std::string, std::shared_ptr<Material>>();

	this->m_Skybox = Game::getInstance()->getTextureManager()->createColorCubeMapFromTexture("graphics/cube", 256);
}


MaterialManager::~MaterialManager()
{
}

std::shared_ptr<Material> MaterialManager::getMaterialByName(const std::string& materialName) {
	const auto it = m_s_Materials.find(materialName);
	if (it != m_s_Materials.end()) return it->second;
	return std::unique_ptr<Material>(nullptr);
}

bool MaterialManager::createMaterialByShader(const std::string& filePath, const std::string& materialName,
	const Material::Type materialType) {


	Material::compileShader(filePath);
	Material m = Material(filePath, materialType);

	/* Put texture id into map */
	m_s_Materials[materialName] = std::make_shared<Material>(m);

	std::cout << "... Material \"" << materialName.c_str() << "\" created from shader \"" << filePath.c_str() << "\"" <<  std::endl;
	return true;
}

