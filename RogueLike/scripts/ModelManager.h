#pragma once
#include "stdafx.h"
#include "ModelData.h"
#include <memory>

class ModelManager
{
private:
	std::map<std::string, std::shared_ptr<ModelData>> m_s_Models;
	std::map<Primitives, std::shared_ptr<ModelData>> m_s_PrimitiveModels;
public:
	ModelManager();
	~ModelManager();

	std::shared_ptr<ModelData> getModelDataByIdentifier(const std::string& modelName);
	std::shared_ptr<ModelData> getModelDataByPrimitve(const Primitives _primitive);

	bool ModelManager::loadModelAs(const std::string& filePath, const std::string& modelName);
	bool ModelManager::loadPrimitive(const Primitives _primitives);
};


