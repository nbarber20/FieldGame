#include "pch.h"
#include "Entity_Interior.h"
#include "World.h"
#include "Entity_Room.h"
#include "ObservationManager.h"

Entity_Interior::~Entity_Interior()
{
	for (auto room : rooms)
	{
		Entity* e = World::Instance().GetEntityByID(room->roomEntityID, worldID);
		World::Instance().RemoveEntity(e);
	}
	rooms.clear();
}


void Entity_Interior::AddRoom(std::string roomName, sf::Vector2i position, bool hasExterior)
{
	Room* newRoom = new Room;
	Entity_Room* RoomEntity = new Entity_Room(World::Instance().GetUniqueID(),false, 47520.0f, 47520.0f);
	RoomEntity->names = { roomName,"room","floor" };
	RoomEntity->parent = std::make_pair(OnFloor, this);
	RoomEntity->parentEntityDir = (int)OnFloor;
	RoomEntity->parentEntityID = this->uniqueEntityID;
	children.push_back(std::pair<Position, std::vector<Entity*>>(OnFloor, { RoomEntity }));
	World::Instance().AddEntity(RoomEntity);
	RoomEntity->attachedToParent = true;
	newRoom->hasExteriorDoor = hasExterior;
	newRoom->position = position;
	newRoom->roomEntityID = RoomEntity->uniqueEntityID;
	rooms.push_back(newRoom);
}

void Entity_Interior::AddChild(Position pos, Entity* toAdd, int roomIndex)
{
	if (roomIndex < 0 || roomIndex >= rooms.size())return;

	Entity_Room* r = dynamic_cast<Entity_Room*>(toAdd);
	if (r) {
		Entity::AddChild(pos, toAdd, 0);
		return;
	}

	Entity* e = World::Instance().GetEntityByID(rooms[roomIndex]->roomEntityID,worldID);
	toAdd->parent = std::make_pair(pos,e );

	for (int i = 0; i < e->children.size();i++) {
		if (e->children[i].first == pos) {
			e->children[i].second.push_back(toAdd);
			return;
		}
	}
	//We do not have anything in that position currently
	std::vector<Entity*> newArray = { toAdd };
	e->children.push_back(std::make_pair(pos, newArray));
}

Entity_Room* Entity_Interior::GetAdjacent(FacingDirection dir, Entity* room)
{
	for (auto i : rooms) {
		Entity* e = World::Instance().GetEntityByID(i->roomEntityID, worldID);
		if (e == room) {
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
				Entity* e2 = World::Instance().GetEntityByID(rooms[index]->roomEntityID, worldID);
				Entity_Room* roomE2 = dynamic_cast<Entity_Room*>(e2);
				return roomE2;
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

