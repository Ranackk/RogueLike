#pragma once
#include "RoomBlueprint.h"
class Scene;

class WorldGenerator
{
public:
	static void generateWorld(const glm::vec2 roomGridSize, Scene &mapToGenerateIn);

private:
	/* Disallow creation of a world generator instance */
	WorldGenerator() {}

	static RoomBlueprint* readRoomBlueprintsFromFile(const glm::vec2 roomFieldSize, const std::string filePath, Scene& mapToGenerateIn, int* roomCount);
};

