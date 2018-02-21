#include "stdafx.h"
#include "WorldGenerator.h"
#include "Scene.h"
#include "RoomBlueprint.h"
#include "Game.h"
#include "lodepng.h"
#include <filesystem>
#include <detail/_vectorize.hpp>

void WorldGenerator::generateWorld(const glm::vec2 roomGridSize, Scene &mapToGenerateIn)
{
	/* Const vars for easier calculation */
	const glm::vec2 individualRoomSize = glm::vec2(Game::m_s_cRoomWidthInFields, Game::m_s_cRoomHeightInFields);
	const glm::vec2 worldFieldSize = glm::vec2(roomGridSize.x * individualRoomSize.x,roomGridSize.y * individualRoomSize.y);
	/* Initialize arrays */
	RoomBlueprint* _rooms = new RoomBlueprint[static_cast<unsigned int>(roomGridSize.x * roomGridSize.y)];
	mapToGenerateIn.m_Fields = new FieldComponent [static_cast<unsigned int>(worldFieldSize.x * worldFieldSize.y)];
	
	/* Read all room blueprints from files */
	int roomCount;
	RoomBlueprint* _roomBlueprints = readRoomBlueprintsFromFile(individualRoomSize, "graphics/rooms.png", mapToGenerateIn, &roomCount);

	/* Fill rooms with the just read blueprints */
	for (int rX = 0; rX < roomGridSize.x; rX++) {
		for (int rY = 0; rY < roomGridSize.y; rY++) {
			RoomBlueprint currentRoom = _rooms[(int)roomGridSize.x * rY + rX];
			// TODO: Get a random room blueprint
			const int id = rand() % roomCount;
			currentRoom.fillWithTypes(individualRoomSize, _roomBlueprints[id].getFieldData());
			currentRoom.fillLightAndEnemyVector(_roomBlueprints[id].m_LightPositions, _roomBlueprints[id].m_EnemyInformation);
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
					//FieldComponent newField =
					//newField.initialize(&mapToGenerateIn, worldPosition, currentRoom.getFieldTypeAt(glm::vec2(fX, fY)));
					GameObject* gO = new GameObject("Field [" + std::to_string(fX) + ", " + std::to_string(fY) + "]");
					mapToGenerateIn.m_Fields[(int)(worldPosition.x + worldPosition.y * worldFieldSize.x)] = *gO->addComponent(new FieldComponent());
					mapToGenerateIn.m_Fields[(int)(worldPosition.x + worldPosition.y * worldFieldSize.x)].initialize(&mapToGenerateIn, worldPosition, currentRoom.getFieldTypeAt(glm::vec2(fX, fY)));
				}
			}

			/* Create Lights & Enemies*/
			for (auto it = currentRoom.m_LightPositions.begin(); it != currentRoom.m_LightPositions.end(); ++it) {
				//if (mapToGenerateIn.m_Lights.size() >= 2* 2) continue;
				GameObject* go = new GameObject("Scene Light " + mapToGenerateIn.m_Lights.size());
				LightComponent* l = go->addComponent(new LightComponent());
				l->initialize(glm::vec3(topLeftWorldPosition.x + it->x + 0.5, 1, topLeftWorldPosition.y + it->y + 0.5), 30.0f, glm::vec4(0.8, 0.8, 1.3, 1.0), LightComponent::STATIC, true);
				mapToGenerateIn.m_Lights.push_back(l);
			}
			for (auto it = currentRoom.m_EnemyInformation.begin(); it != currentRoom.m_EnemyInformation.end(); ++it) {
				if (mapToGenerateIn.m_Enemies.size() >= 1) continue;
				GameObject* go = new GameObject("Scene Enemy " + mapToGenerateIn.m_Enemies.size());
				EnemyComponent* e = go->addComponent(new EnemyComponent());
				const EnemyComponent::EnemyType type = it->second;
				e->initialize(&mapToGenerateIn, type, glm::vec3(topLeftWorldPosition.x + it->first.x + 0.5, 0.0f, topLeftWorldPosition.y + it->first.y + 0.5));
				mapToGenerateIn.m_Enemies.push_back(e);
			}
		}
	}

	/* Generate batches by field type */

	// Sort Fields by their type
	std::map<int, std::vector<GameObject*>> fieldsByFieldType = std::map<int, std::vector <GameObject*>> ();
	for (int i = 0; i < worldFieldSize.x * worldFieldSize.y; i++) {
		const FieldType currentFieldType = mapToGenerateIn.m_Fields[i].m_FieldType;

		bool found = false;
		for (auto it = fieldsByFieldType.begin(); it != fieldsByFieldType.end(); ++it) {
			if (it->first == currentFieldType.m_Id) {
				fieldsByFieldType[currentFieldType.m_Id].push_back(mapToGenerateIn.m_Fields[i].getGameObject());
				found = true;
			}
		}
		if (!found) {
			fieldsByFieldType[currentFieldType.m_Id] = std::vector<GameObject*>();
			fieldsByFieldType[currentFieldType.m_Id].push_back(mapToGenerateIn.m_Fields[i].getGameObject());
		}
		//if (fieldsByFieldType.find(currentFieldType) != fieldsByFieldType.end()) {
		//	fieldsByFieldType[currentFieldType].push_back(&mapToGenerateIn.m_Fields[i]);
		//}
		//else {
		//	fieldsByFieldType[currentFieldType] = std::vector<GameObject*>();
		//	fieldsByFieldType[currentFieldType].push_back(&mapToGenerateIn.m_Fields[i]);
		//}
	}
	// Create a batch for each type
	mapToGenerateIn.m_FieldBatches = std::vector<RenderBatch>();
	int amount = 0;
	for (auto it = fieldsByFieldType.begin(); it != fieldsByFieldType.end(); ++it)
	{
		//FieldType currentFieldType = it->first;
		RenderComponent* currentRenderComponent = it->second[0]->getComponent<RenderComponent>();
		RenderBatch batch = RenderBatch();
		batch.initialize(currentRenderComponent->getModelData(), currentRenderComponent->getMaterial(), it->second);
		mapToGenerateIn.m_FieldBatches.push_back(batch);
		amount++;
	}
	std::cout << "... Created " << amount << " batches for the map geometry" << std::endl;

	/* Cleanup */
	delete[] _rooms;
}

RoomBlueprint* WorldGenerator::readRoomBlueprintsFromFile(const glm::vec2 roomFieldSize, const std::string filePath, Scene &mapToGenerateIn, int* roomCount)
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

	*roomCount = roomsX * roomsY;

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

					types[fX + fY * static_cast<int>(roomFieldSize.x)] = FieldType::byColor(r);
					
					if (b != 0) {
						currentBlueprint.m_LightPositions.push_back(glm::vec2(fX, fY));
					}
					if (g != 0) {
						currentBlueprint.m_EnemyInformation.push_back(std::pair<glm::vec2, EnemyComponent::EnemyType>(glm::vec2(fX, fY), EnemyComponent::getTypeByColor(g)));
					}
				}
			}
			currentBlueprint.fillWithTypes(roomFieldSize, types);
			_bluePrints[iY * roomsX + iX] = currentBlueprint;
		}
	}
	return _bluePrints;

}
