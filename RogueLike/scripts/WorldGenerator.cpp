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
	/* === INITIALIZATION === */
	/* Const vars for easier calculation */
	const glm::vec2 individualRoomSize = glm::vec2(Game::m_s_cRoomWidthInFields, Game::m_s_cRoomHeightInFields);
	const glm::vec2 worldFieldSize = glm::vec2(roomGridSize.x * individualRoomSize.x, roomGridSize.y * individualRoomSize.y);
	/* Initialize arrays */
	RoomBlueprint* _rooms = new RoomBlueprint[static_cast<unsigned int>(roomGridSize.x * roomGridSize.y)];
	mapToGenerateIn.m_Fields = new FieldComponent[static_cast<unsigned int>(worldFieldSize.x * worldFieldSize.y)];


	/* === READ THE ROOMS FILE AND STORE IT === */
	/* Read all room blueprints from files */
	int roomCount;
	RoomBlueprint* _roomBlueprints = readRoomBlueprintsFromFile(individualRoomSize, "graphics/rooms.png", mapToGenerateIn, &roomCount);

	/* Sort the room blue prints by their availible door directions */
	std::vector<RoomBlueprint> _roomBlueprintsWithDoorToNorth;
	std::vector<RoomBlueprint> _roomBlueprintsWithDoorToEast;
	std::vector<RoomBlueprint> _roomBlueprintsWithDoorToSouth;
	std::vector<RoomBlueprint> _roomBlueprintsWithDoorToWest;

	for (int i = 0; i < roomCount; i++) {
		if (_roomBlueprints[i].m_HasLeftoverDoorFacingNorth) _roomBlueprintsWithDoorToNorth.push_back(_roomBlueprints[i]);
		if (_roomBlueprints[i].m_HasLeftoverDoorFacingEast) _roomBlueprintsWithDoorToEast.push_back(_roomBlueprints[i]);
		if (_roomBlueprints[i].m_HasLeftoverDoorFacingSouth) _roomBlueprintsWithDoorToSouth.push_back(_roomBlueprints[i]);
		if (_roomBlueprints[i].m_HasLeftoverDoorFacingWest) _roomBlueprintsWithDoorToWest.push_back(_roomBlueprints[i]);
	}

	/* === START OF THE WORLD GENERATION === */
	//const long seed = rand();
	//std::cout << "Generating random map with seed " << seed << std::endl;

	std::vector<glm::vec2> roomCoordinatesPlaced = std::vector<glm::vec2>();

	/* Place the starting room (always index 0 in blueprints array) at the mid of the world */
	{	
	const int midX = static_cast<int>(ceil(roomGridSize.x / 2.0f));
	const int midY = static_cast<int>(ceil(roomGridSize.y / 2.0f));
	RoomBlueprint currentRoom = _rooms[static_cast<int>(roomGridSize.x) * midY + midX];
	currentRoom.fillWithBlueprint(&_roomBlueprints[0]);
	_rooms[static_cast<int>(roomGridSize.x) * midY + midX] = currentRoom;

	roomCoordinatesPlaced.push_back(glm::vec2(midX, midY));
	}

	/* Start the room placement of the rest */
	const int countOfRoomsInGrid = static_cast<int>(roomGridSize.x * roomGridSize.y);
	const int maxRooms = 10;
	const int maxTryCount = 20;

	int currentTryCount = 0;
	while (currentTryCount < maxTryCount) {
		currentTryCount++;

		/* Try to find a already placed room */
		const int randomPlacedIndex = rand() % roomCoordinatesPlaced.size();


		const int randomRoomX = static_cast<int>(roomCoordinatesPlaced[randomPlacedIndex].x);
		const int randomRoomY = static_cast<int>(roomCoordinatesPlaced[randomPlacedIndex].y);

		const int randomRoomIndex = static_cast<int>(randomRoomY * roomGridSize.x + randomRoomX);

		if (_rooms[randomRoomIndex].isPlacedInWorld()) {
			RoomBlueprint currentRoom = _rooms[randomRoomIndex];
			std::vector<int> roomDoors = currentRoom.getLeftoverDoorsIndices();

			/* Check if the room has open doors */
			if (roomDoors.size() == 0) continue;

			/* If so, get a random open door*/
			const int randomIndex = rand() % roomDoors.size();
			const int doorDirection = roomDoors[randomIndex];
			
			/* Calculate position of new room */
			int roomToGenerateX = -1;
			int roomToGenerateY = -1;
			switch (doorDirection) {
			case 0:
				roomToGenerateX = randomRoomX;
				roomToGenerateY = randomRoomY - 1;
				break;
			case 1:
				roomToGenerateX = randomRoomX + 1;
				roomToGenerateY = randomRoomY;
				break;
			case 2:
				roomToGenerateX = randomRoomX;
				roomToGenerateY = randomRoomY + 1;
				break;
			case 3:
				roomToGenerateX = randomRoomX - 1;
				roomToGenerateY = randomRoomY;
				break;
			default: break;
			}

			/* Validate (is this field empty & in bounds ?)*/
			if (roomToGenerateX < 0 || roomToGenerateX > roomGridSize.x || roomToGenerateY < 0 || roomToGenerateY > roomGridSize.y) continue;
			const int roomToGenerateIndex = static_cast<int>(roomToGenerateY * roomGridSize.x + roomToGenerateX);

			/* If the room already exists, try another one. */
			if (_rooms[roomToGenerateIndex].isPlacedInWorld()) {
				// TODO: Check if this door has to be seales or not (depending on the state of the door next to it)
				continue;
			}

			/* Find a room to place there */
			std::vector<RoomBlueprint>* roomVectorToUse = nullptr;
			switch (doorDirection) {
			case 0:
				roomVectorToUse = &_roomBlueprintsWithDoorToSouth;
				break;
			case 1:
				roomVectorToUse = &_roomBlueprintsWithDoorToWest;
				break;
			case 2:
				roomVectorToUse = &_roomBlueprintsWithDoorToNorth;
				break;
			case 3:
				roomVectorToUse = &_roomBlueprintsWithDoorToEast;
				break;
			default: break;
			}

			const int roomToPlaceIndexInDoorList = rand() % (*roomVectorToUse).size();
			RoomBlueprint roomToPlaceBlueprint = (*roomVectorToUse)[roomToPlaceIndexInDoorList];

			/* Prepare the room */
			RoomBlueprint roomToPlace = _rooms[roomToGenerateIndex];
			roomToPlace.fillWithBlueprint(&roomToPlaceBlueprint);

			roomCoordinatesPlaced.push_back(glm::vec2(roomToGenerateX, roomToGenerateY));

			/* Unmark the both rooms (original & newly placed) as having doors left over */
			int inverseDoorDirection = -1;
			switch (doorDirection) {
			case 0: inverseDoorDirection = 2; break;
			case 1: inverseDoorDirection = 3; break;
			case 2: inverseDoorDirection = 0; break;
			case 3: inverseDoorDirection = 1; break;
			default: break;
			}

			_rooms[randomRoomIndex].markDoorAsUsed(doorDirection);
			roomToPlace.markDoorAsUsed(inverseDoorDirection);

			/* Place it */
			_rooms[roomToGenerateIndex] = roomToPlace;
		}

		if (roomCoordinatesPlaced.size() == countOfRoomsInGrid || roomCoordinatesPlaced.size() >= maxRooms) {
			break;
		}
	}

	/* Fill all empty rooms with void*/
	for (int rX = 0; rX < roomGridSize.x; rX++) {
		for (int rY = 0; rY < roomGridSize.y; rY++) {
			if (_rooms[static_cast<int>(roomGridSize.x) * rY + rX].getFieldData() == nullptr) {
				_rooms[static_cast<int>(roomGridSize.x) * rY + rX].fillWithVoid(individualRoomSize);
			}
		}
	}


	///* Fill rooms with the just read blueprints */
	//for (int rX = 0; rX < roomGridSize.x; rX++) {
	//	for (int rY = 0; rY < roomGridSize.y; rY++) {
	//		RoomBlueprint currentRoom = _rooms[(int)roomGridSize.x * rY + rX];
	//		// TODO: Get a random room blueprint
	//		const int id = srand(seed) % roomCount;
	//		currentRoom.fillWithBlueprint(&_roomBlueprints[id]);
	//		_rooms[static_cast<int>(roomGridSize.x) * rY + rX] = currentRoom;
	//	}
	//}

	/* Apply every room blueprint */
	for (int rX = 0; rX < roomGridSize.x; rX ++) {
		for (int rY = 0; rY < roomGridSize.y; rY++) {
			/* Fetch the correct room blueprint */
			const RoomBlueprint currentRoom = _rooms[static_cast<int>(roomGridSize.x) * rY + rX];
			const glm::vec2 topLeftWorldPosition = glm::vec2(rX * individualRoomSize.x, rY * individualRoomSize.y);

			std::cout << "Creating Room " << rX << ", " << rY << " at topLeftPos " << topLeftWorldPosition.x << ", " << topLeftWorldPosition.y << std::endl;
			/* For every field in the room */
			for (int fX = 0; fX < individualRoomSize.x; fX++) {
				for (int fY = 0; fY < individualRoomSize.y; fY++) {
					/* The array spot we want to access is always current world Y * world width + current world X */
					const glm::vec2 worldPosition = topLeftWorldPosition + glm::vec2(fX, fY);

					/* Create the field from the field type specified in the room blueprint */
					const FieldType ft = currentRoom.getFieldTypeAt(glm::vec2(fX, fY));
					GameObject* gO = new GameObject("Field [" + std::to_string(fX) + ", " + std::to_string(fY) + "]");
					mapToGenerateIn.m_Fields[static_cast<int>(worldPosition.x + worldPosition.y * worldFieldSize.x)] = *gO->addComponent(new FieldComponent());
					mapToGenerateIn.m_Fields[static_cast<int>(worldPosition.x + worldPosition.y * worldFieldSize.x)].initialize(&mapToGenerateIn, worldPosition, ft);
					mapToGenerateIn.m_Fields[static_cast<int>(worldPosition.x + worldPosition.y * worldFieldSize.x)].setRoomCoord(glm::vec2(rX, rY));
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
				//if (mapToGenerateIn.m_Enemies.size() >= 1) continue;
				GameObject* go = new GameObject("Scene Enemy " + mapToGenerateIn.m_Enemies.size());
				EnemyComponent* e = go->addComponent(new EnemyComponent());
				const EnemyComponent::EnemyType type = it->second;
				e->initialize(&mapToGenerateIn, type, glm::vec3(topLeftWorldPosition.x + it->first.x + 0.5, 0.0f, topLeftWorldPosition.y + it->first.y + 0.5));
				e->setRoomCoord(glm::vec2(rX, rY));
				mapToGenerateIn.m_Enemies.push_back(e);
			}
		}
	}

	/* For every door, check if another door is connected to it. If not, seal it */
	for (int i = 0; i < worldFieldSize.x * worldFieldSize.y; i++) {
		const FieldType currentFieldType = mapToGenerateIn.m_Fields[i].m_FieldType;
		if (currentFieldType == FieldType::DOOR_NORTH || currentFieldType == FieldType::DOOR_EAST || currentFieldType == FieldType::DOOR_SOUTH || currentFieldType == FieldType::DOOR_WEST) {
			const int left = i - 1;
			const int right = i + 1;
			const int top = i - static_cast<int>(worldFieldSize.x);
			const int bot = i + static_cast<int>(worldFieldSize.x);

			bool neighbourIsDoor = false;
			if (left >= 0) {
				const FieldType checkType = mapToGenerateIn.m_Fields[left].m_FieldType;
				if (checkType == FieldType::DOOR_NORTH || checkType == FieldType::DOOR_EAST || checkType == FieldType::DOOR_SOUTH || checkType == FieldType::DOOR_WEST) {
					neighbourIsDoor = true;
				}
			}
			if (top >= 0) {
				const FieldType checkType = mapToGenerateIn.m_Fields[top].m_FieldType;
				if (checkType == FieldType::DOOR_NORTH || checkType == FieldType::DOOR_EAST || checkType == FieldType::DOOR_SOUTH || checkType == FieldType::DOOR_WEST) {
					neighbourIsDoor = true;
				}
			}
			if (right < worldFieldSize.x * worldFieldSize.y) {
				const FieldType checkType = mapToGenerateIn.m_Fields[right].m_FieldType;
				if (checkType == FieldType::DOOR_NORTH || checkType == FieldType::DOOR_EAST || checkType == FieldType::DOOR_SOUTH || checkType == FieldType::DOOR_WEST) {
					neighbourIsDoor = true;
				}
			}
			if (bot < worldFieldSize.x * worldFieldSize.y) {
				const FieldType checkType = mapToGenerateIn.m_Fields[bot].m_FieldType;
				if (checkType == FieldType::DOOR_NORTH || checkType == FieldType::DOOR_EAST || checkType == FieldType::DOOR_SOUTH || checkType == FieldType::DOOR_WEST) {
					neighbourIsDoor = true;
				}
			}

			if (!neighbourIsDoor) {
				if (currentFieldType == FieldType::DOOR_NORTH || currentFieldType == FieldType::DOOR_SOUTH)
					mapToGenerateIn.m_Fields[i].m_FieldType = FieldType::WALL_X;
				else mapToGenerateIn.m_Fields[i].m_FieldType = FieldType::WALL_Z;
			}
		}
	
	}


	/* Generate batches by field type */

	// Sort Fields by their type
	std::map<int, std::vector<GameObject*>> fieldsByFieldType = std::map<int, std::vector <GameObject*>> ();
	for (int i = 0; i < worldFieldSize.x * worldFieldSize.y; i++) {
		const FieldType currentFieldType = mapToGenerateIn.m_Fields[i].m_FieldType;
		/* Dont create render batches for VOID type fields */
		if (currentFieldType.isVoid()) {
			continue;
		}
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
