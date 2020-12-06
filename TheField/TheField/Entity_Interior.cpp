#include "pch.h"
#include "Entity_Interior.h"
#include "World.h"
#include "Entity_Room.h"
#include "ObservationManager.h"

void Entity_Interior::AddRoom()
{
	Room* newRoom = new Room;



	Entity* RoomEntity = new Entity_Room(false, 47520.0f, 47520.0f);
	RoomEntity->names = { "room","floor" };
	RoomEntity->parent = std::make_pair(OnFloor, this);
	children.push_back(std::pair<Position, std::vector<Entity*>>(OnFloor, { RoomEntity }));
	World::Instance().AddEntity(RoomEntity);
	RoomEntity->attachedToParent = true;
	newRoom->hasExteriorDoor = true;
	newRoom->roomEntity = RoomEntity;
	rooms.push_back(newRoom);
}

void Entity_Interior::AddChild(Position pos, Entity* toAdd)
{
	toAdd->parent = std::make_pair(pos, rooms[0]->roomEntity);
	for (int x = 0; x < rooms[0]->roomEntity->children.size(); x++) {
		if (rooms[0]->roomEntity->children[x].first == pos) {
			rooms[0]->roomEntity->children[x].second.push_back(toAdd);
			return;
		}
	}
	//We do not have anything in that position currently
	std::pair<Position, std::vector<Entity*>> pair = std::pair<Position, std::vector<Entity*>>(pos, { toAdd });
	rooms[0]->roomEntity->children.push_back(pair);
}