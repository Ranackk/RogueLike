#include "stdafx.h"
#include "WorldGenerator.h"
#include "Scene.h"
#include "RoomBlueprint.h"
#include "Game.h"
#include "lodepng.h"
#include <filesystem>

void WorldGenerator::generateWorld(const glm::vec2 roomGridSize, Scene &mapToGenerateIn)
{
	/* Const vars for easier calculation */
	const glm::vec2 individualRoomSize = glm::vec2(Game::m_s_cRoomWidthInFields, Game::m_s_cRoomHeightInFields);
	const glm::vec2 worldFieldSize = glm::vec2(roomGridSize.x * individualRoomSize.x,roomGridSize.y * individualRoomSize.y);
	/* Initialize arrays */
	RoomBlueprint* _rooms = new RoomBlueprint[static_cast<unsigned int>(roomGridSize.x * roomGridSize.y)];
	mapToGenerateIn.m_Fields = new Field [static_cast<unsigned int>(worldFieldSize.x * worldFieldSize.y)];

	/* Read all room blueprints from files */
	RoomBlueprint* _roomBlueprints = readRoomBlueprintsFromFile(individualRoomSize, "graphics/rooms.png");

	/* Fill rooms with the just read blueprints */
	for (int rX = 0; rX < roomGridSize.x; rX++) {
		for (int rY = 0; rY < roomGridSize.y; rY++) {
			RoomBlueprint currentRoom = _rooms[(int)roomGridSize.x * rY + rX];
			// TODO: Get a random room blueprint
			currentRoom.fillWithTypes(individualRoomSize, _roomBlueprints[0].getFieldData());
			_rooms[(int)roomGridSize.x * rY + rX] = currentRoom;
		}
	}
	
	/* Apply every room blueprint */
	for (int rX = 0; rX < roomGridSize.x; rX ++) {
		for (int rY = 0; rY < roomGridSize.y; rY++) {
			/* Fetch the correct room blueprint */
			const RoomBlueprint currentRoom = _rooms[(int)roomGridSize.x * rY + rX];
			const glm::vec2 topLeftWorldPosition = glm::vec2(rX * individualRoomSize.x, rY * individualRoomSize.y);

			std::cout << "Creating Room " << rX << ", " << rY << " at topLeftPos " << topLeftWorldPosition.x << ", " << topLeftWorldPosition.y << std::endl;
			/* For every field in the room */
			for (int fX = 0; fX < individualRoomSize.x; fX++) {
				for (int fY = 0; fY < individualRoomSize.y; fY++) {
					/* The array spot we want to access is always current world Y * world width + current world X */
					const glm::vec2 worldPosition = topLeftWorldPosition + glm::vec2(fX, fY);
					/* Create the field from the field type specified in the room blueprint */
					//Field newField =
					//newField.initialize(&mapToGenerateIn, worldPosition, currentRoom.getFieldTypeAt(glm::vec2(fX, fY)));
					mapToGenerateIn.m_Fields[(int)(worldPosition.x + worldPosition.y * worldFieldSize.x)] = Field();
					mapToGenerateIn.m_Fields[(int)(worldPosition.x + worldPosition.y * worldFieldSize.x)].initialize(&mapToGenerateIn, worldPosition, currentRoom.getFieldTypeAt(glm::vec2(fX, fY)));
				}
			}
		}
	}

	/* Generate batches by field type */

	// Sort Fields by their type
	std::map<FieldType, std::vector<GameObject>> fieldsByFieldType = std::map<FieldType, std::vector <GameObject>> ();
	for (int i = 0; i < worldFieldSize.x * worldFieldSize.y; i++) {
		const FieldType currentFieldType = mapToGenerateIn.m_Fields[i].m_FieldType;
		if (fieldsByFieldType.find(currentFieldType) != fieldsByFieldType.end()) {
			fieldsByFieldType[currentFieldType].push_back(mapToGenerateIn.m_Fields[i]);
		}
		else {
			fieldsByFieldType[currentFieldType] = std::vector<GameObject>();
			fieldsByFieldType[currentFieldType].push_back(mapToGenerateIn.m_Fields[i]);
		}
	}
	// Create a batch for each type
	mapToGenerateIn.m_FieldBatches = std::vector<RenderBatch>();
	int amount = 0;
	for (auto it = fieldsByFieldType.begin(); it != fieldsByFieldType.end(); ++it)
	{
		FieldType currentFieldType = it->first;
		RenderComponent* currentRenderComponent = it->second[0].getComponent<RenderComponent>();
		RenderBatch batch = RenderBatch();
		batch.initialize(currentRenderComponent->getModelData(), currentRenderComponent->getMaterial(), it->second);
		mapToGenerateIn.m_FieldBatches.push_back(batch);
		amount++;
	}
	std::cout << "... Created " << amount << " batches for the map geometry" << std::endl;

	/* Cleanup */
	delete[] _rooms;
}

RoomBlueprint* WorldGenerator::readRoomBlueprintsFromFile(const glm::vec2 roomFieldSize, const std::string filePath)
{
	/* Load room blueprints from png file */
	// Load file and decode image.
	std::vector<unsigned char> image;
	unsigned imgWidth, imgHeight;
	const unsigned error = lodepng::decode(image, imgWidth, imgHeight, filePath, LodePNGColorType::LCT_RGBA, 8);
	// If there's an error, display it.
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return nullptr;
	}

	
	const unsigned roomsX = static_cast<unsigned int>(imgWidth / roomFieldSize.x);
	const unsigned roomsY = static_cast<unsigned int>(imgHeight / roomFieldSize.y);

	RoomBlueprint*_bluePrints = new RoomBlueprint[roomsX * roomsY];
	for (unsigned int iX = 0; iX < roomsX; iX++) {
		for (unsigned int iY = 0; iY < roomsY; iY++) {
			const glm::vec2 roomTopLeftImagePosition = glm::vec2(iX * roomFieldSize.x, iY * roomFieldSize.y);
			RoomBlueprint currentBlueprint = _bluePrints[iY * roomsX + iX];
			currentBlueprint = RoomBlueprint();
			FieldType* types = new FieldType[static_cast<unsigned int>(roomFieldSize.x * roomFieldSize.y)];

			for (int fX = 0; fX < roomFieldSize.x; fX++) {
				for (int fY = 0; fY < roomFieldSize.y; fY++) {
					const glm::vec2 samplePosition = roomTopLeftImagePosition + glm::vec2(fX, fY);
					const unsigned char r = image[static_cast<unsigned int>(4 * (samplePosition.x + samplePosition.y * imgWidth) + 0)];
					const unsigned char g = image[static_cast<unsigned int>(4 * (samplePosition.x + samplePosition.y * imgWidth) + 1)];
					const unsigned char b = image[static_cast<unsigned int>(4 * (samplePosition.x + samplePosition.y * imgWidth) + 2)];
					const unsigned char a = image[static_cast<unsigned int>(4 * (samplePosition.x + samplePosition.y * imgWidth) + 3)];

					types[fX + fY * static_cast<int>(roomFieldSize.x)] = FieldType::byColor(r, g, b, a);
				}
			}
			currentBlueprint.fillWithTypes(roomFieldSize, types);
			_bluePrints[iY * roomsX + iX] = currentBlueprint;
		}
	}
	return _bluePrints;

}
