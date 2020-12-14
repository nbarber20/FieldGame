#include "pch.h"
#include "Entity_Interior.h"
#include "World.h"
#include "Entity_Room.h"
#include "ObservationManager.h"

void Entity_Interior::AddRoom(std::string roomName, sf::Vector2i position, bool hasExterior)
{
	Room* newRoom = new Room;
	Entity_Room* RoomEntity = new Entity_Room(false, 47520.0f, 47520.0f);
	RoomEntity->names = { roomName,"room","floor" };
	RoomEntity->parent = std::make_pair(OnFloor, this);
	children.push_back(std::pair<Position, std::vector<Entity*>>(OnFloor, { RoomEntity }));
	World::Instance().AddEntity(RoomEntity);
	RoomEntity->attachedToParent = true;
	newRoom->hasExteriorDoor = hasExterior;
	newRoom->position = position;
	newRoom->roomEntity = RoomEntity;
	rooms.push_back(newRoom);
}

void Entity_Interior::AddChild(Position pos, Entity* toAdd, int roomIndex)
{
	if (roomIndex < 0 || roomIndex >= rooms.size())return;

	toAdd->parent = std::make_pair(pos, rooms[roomIndex]->roomEntity);

	for (auto object : rooms[roomIndex]->roomEntity->children) {
		if (object.first == pos) {
			object.second.push_back(toAdd);
			return;
		}
	}
	//We do not have anything in that position currently
	std::vector<Entity*> newArray = { toAdd };
	rooms[roomIndex]->roomEntity->children.push_back(std::make_pair(pos, newArray));
}

Entity_Room* Entity_Interior::GetAdjacent(FacingDirection dir, Entity* room)
{
	for (auto i : rooms) {
		if (i->roomEntity == room) {
			sf::Vector2i delta = sf::Vector2i(0,0);
			switch (dir)
			{
			case North:
				delta = sf::Vector2i(0, 1);
				break;
			case East:
				delta = sf::Vector2i(1, 0);
				break;
			case South:
				delta = sf::Vector2i(0, -1);
				break;
			case West:
				delta = sf::Vector2i(-1, 0);
				break;
			}

			int index = GetRoom(i->position + delta);
			if (index != -1) {
				return rooms[index]->roomEntity;
			}
			return nullptr;
		}
	}
	return nullptr;
}

int Entity_Interior::GetRoom(sf::Vector2i position)
{
	for (int i = 0; i < rooms.size(); i++) {
		if (rooms[i]->position == position) {
			return i;
		}
	}
	return -1;
}

