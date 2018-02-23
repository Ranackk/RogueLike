#include "stdafx.h"

#include "ModelData.h"
#include "ModelManager.h"
#include "Utility.h"

ModelManager::ModelManager() {
	m_s_Models = std::map<std::string, std::shared_ptr<ModelData>>();
	m_s_PrimitiveModels = std::map<Primitives, std::shared_ptr<ModelData>>();

	loadPrimitive(QUAD);
}

ModelManager::~ModelManager() {
	
}

std::shared_ptr<ModelData> ModelManager::getModelDataByIdentifier(const std::string& modelName) {
	const auto it = m_s_Models.find(modelName);
	if (it != m_s_Models.end()) return it->second;
	return std::unique_ptr<ModelData>(nullptr);
}

std::shared_ptr<ModelData> ModelManager::getModelDataByPrimitve(const Primitives _primitive) {
	const auto it = m_s_PrimitiveModels.find(_primitive);
	if (it != m_s_PrimitiveModels.end()) return it->second;
	return std::unique_ptr<ModelData>(nullptr);
}

bool ModelManager::loadModelAs(const std::string& filePath, const std::string& modelName) {
	
	/* Load obj data into temporary containers */
	std::vector<glm::vec3> vertices, outVertices;
	std::vector<glm::vec2> uvs, outUVs;
	std::vector<glm::vec3> normals, outNormals;
	std::vector<unsigned short> outIndices;

	const bool result = Utility::loadObj(std::string(filePath).c_str(), vertices, uvs, normals);
	if (!result) {
		std::cout << "Error getting the modelinformation " << std::endl;
		return false;
	}
	/* Index the loaded obj data into the correct buffers */
	Utility::indexVbo(vertices, uvs, normals, outIndices, outVertices, outUVs, outNormals);


	ModelData m = ModelData();
	//m.initialize(filePath);
	m.initialize(outIndices, outVertices, outUVs, outNormals);

	/* Put texture id into map */
	m_s_Models[modelName] = std::make_shared<ModelData>(m);

	std::cout << "... Loaded model \"" << modelName.c_str() << "\"" << " from \"" << filePath.c_str() << "\"" << std::endl;
	return true;
}

bool ModelManager::loadPrimitive(const Primitives _primitives)
{
	std::vector<unsigned short> _indexBufferData;
	std::vector<glm::vec3> _vertexBufferData;
	std::vector<glm::vec2> _uvBufferData;
	std::vector<glm::vec3> _normalsBufferData;

	switch (_primitives) {
	case QUAD:
		Utility::getQuadPrimitiveData(_vertexBufferData, _uvBufferData, _normalsBufferData, _indexBufferData);
		break;
	}

	ModelData m = ModelData();
	m.initialize(_indexBufferData, _vertexBufferData, _uvBufferData, _normalsBufferData);

	m_s_PrimitiveModels[_primitives] = std::make_shared<ModelData>(m);

	std::cout << "... Loaded Primitive: \"" << _primitives << "\"" << std::endl;
	return true;
}
