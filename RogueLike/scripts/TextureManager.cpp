#include "stdafx.h"
#include "TextureManager.h"
#include "lodepng.h"
#include "Game.h"

TextureManager::TextureManager() {
	//m_s_Textures = std::map<std::string, sf::Texture*>();
	m_s_Textures = std::map<std::string, GLuint>();
}

TextureManager::~TextureManager() {
	//for (auto& iterator : m_s_Textures)
	//{
	//	delete m_s_Textures[iterator.first];
	//}
}
//
GLuint TextureManager::createColorCubeMapFromTexture(const std::string filePath, const int _resolution = 256) const {
	/* Generate the texture */
	GLuint textureID;
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	/* Set paramteres */
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	/* Craete the 6 cube map textures */
	for (int face = 0; face < 6; face++) {
		const std::string path = filePath + std::to_string(face) + ".png";

		/* Load up the image from the file */
		std::vector<unsigned char> image;
		unsigned width, height;
		const unsigned error = lodepng::decode(image, width, height, path);
		if (error != 0)
		{
			std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
			return false;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA, _resolution, _resolution,
			0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			std::cout << "Cube Map Color for " << err << std::endl;
		}
	}
	std::cout << "... Loaded CubeMapTexture from \"" << filePath.c_str() << "\" as OpenGLID " << textureID << std::endl;
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "Cube Map Color " << err << std::endl;
	}
	return textureID;
}
//
//GLuint TextureManager::createDepthCubeMapTexture(const int _resolution = 256) {
//	/* Generate the texture */
//	GLuint textureID;
//	glGenTextures(1, &textureID);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//
//	/* Set paramteres */
//	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//	//std::vector<unsigned char> testD = std::vector<unsigned char>(256*256);
//	//for (int i = 0; i < 256 * 256; i++) {
//	//	testD[i] = i;
//	//}
//
//	/* Craete the 6 cube map textures */
//	for (int face = 0; face < 6; face++) {
//		
//		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT24, _resolution, _resolution,
//			0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr); // &testD[0]); //GL_FLOAT
//	}
//	/* Unbind after creation */
//	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
//	GLenum err = glGetError();
//	if (err != GL_NO_ERROR) {
//		std::cout << "Cube Map Depth " << err << std::endl;
//	}
//	return textureID;
//}

GLuint TextureManager::getTextureByIdentifier(const std::string& name){
	const auto it = m_s_Textures.find(name);
	if (it != m_s_Textures.end()) return it->second;
	std::cerr << "There is no Texture named \"" << name << "\"! " << std::endl;
	return -1;
}

bool TextureManager::loadTextureAs(const std::string& filePath, const std::string& textureName) {
	// Load file and decode image.
	std::vector<unsigned char> image;
	unsigned width, height;
	const unsigned error = lodepng::decode(image, width, height, filePath);
	// If there's an error, display it.
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return false;
	}

	/* make the texture a power of two if it is not yet*/
	//size_t u2 = 1; while (u2 < width) u2 *= 2;
	//size_t v2 = 1; while (v2 < height) v2 *= 2;

	//std::vector<unsigned char> imagePowerOfTwo(u2 * v2 * 4);
	//for (size_t y = 0; y < height; y++){
	//	for (size_t x = 0; x < width; x++){
	//		for (size_t c = 0; c < 4; c++)
	//		{
	//			imagePowerOfTwo[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];
	//		}
	//	}
	//}

	/* Generate a new texture "slot" */
	GLuint texId;
	glGenTextures(1, &texId);

	/* Bind the slot to being a texture_2D */
	glBindTexture(GL_TEXTURE_2D, texId);

	/* Store the texture in open gl */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	/* Set the filtering to be nearest */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/* Generate Mipmaps */
	glGenerateMipmap(GL_TEXTURE_2D);

	/* Set Base MipMap Level to 0 (none), set max mip map level to the level specified in the games settings */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, Game::m_s_cMipMapMaxLevel);

	/* Check if the storage went right */
	const int errorCode = glGetError();
	if (errorCode != 0) {
		std::cout << "Error loading image " << filePath << " as " << textureName << " to open gl - ErrorCode: " << errorCode << std::endl;
	}

	/* Put texture id into map */
	m_s_Textures[textureName] = texId;

	std::cout << "... Loaded Texture \"" << textureName << "\" from \"" << filePath << "\" as OpenGLID = " << texId << std::endl;

	return true;
/*
	sf::Texture* newTexture = new sf::Texture();
	const bool success = newTexture->loadFromFile(filePath);
	m_s_Textures[textureName] = newTexture;*/

	//if (success) {
	//	std::cout << "Loaded Texture " << filePath << " as " << textureName << "!" << std::endl;
	//}
}

bool TextureManager::unloadTexture(const std::string& textureName)
{
	/* If the map contains the texture, unload it from gl and the game */
	if (m_s_Textures.find(textureName) !=  m_s_Textures.end()) {
		glDeleteTextures(1, &m_s_Textures[textureName]);
		m_s_Textures.erase(textureName);
		return true;
	}
	return false;
}