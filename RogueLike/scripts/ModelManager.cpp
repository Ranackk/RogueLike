#include "stdafx.h"

#include "ModelData.h"
#include "ModelManager.h"

ModelManager::ModelManager() {
	m_s_Models = std::map<std::string, std::shared_ptr<ModelData>>();
}

ModelManager::~ModelManager() {
	
}

std::shared_ptr<ModelData> ModelManager::getModelDataByIdentifier(const std::string& modelName) {
	const auto it = m_s_Models.find(modelName);
	if (it != m_s_Models.end()) return it->second;
	return std::unique_ptr<ModelData>(nullptr);
}

bool ModelManager::loadModelAs(const std::string& filePath, const std::string& modelName) {
	// TODO: Load model here
	ModelData m = ModelData();
	m.initialize(filePath);

	/* Put texture id into map */
	m_s_Models[modelName] = std::make_shared<ModelData>(m);

	std::cout << "... Model at path \"" << filePath.c_str() << "\" loaded as \"" << modelName.c_str() << "\"" << std::endl;
	return true;
}
