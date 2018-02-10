#pragma once
#include "stdafx.h"
#include <map>

class TextureManager
{
private:
	//std::map<std::string, sf::Texture*> m_s_Textures;
	std::map<std::string, GLuint> m_s_Textures;
public:
	TextureManager();
	~TextureManager();
	GLuint createColorCubeMapFromTexture(const std::string filePath, const int _resolution) const;

	static GLuint createDepthCubeMapTexture(const int _resolution);

	GLuint getTextureByIdentifier(const std::string& name);
	bool TextureManager::loadTextureAs(const std::string& filePath, const std::string& textureName);
	bool unloadTexture(const std::string& textureName);
};

