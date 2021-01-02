#include "pch.h"
#include "Entity_Interior.h"
#include "World.h"
#include "Entity_Room.h"
#include "ObservationManager.h"
#include "GameLoader.h"

#pragma region Serialization
void Entity_Interior::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	//TODO
}

void Entity_Interior::ReadFromJson(Value& v)
{
	//TODO
}

void Entity_Interior::WriteData(std::fstream* output)
{
	Entity::WriteData(output);
	int roomCount = rooms.size();
	output->write((char*)&roomCount, sizeof(int));
	for (int i = 0; i < roomCount; i++) {
		output->write((char*)&(rooms[i]->roomEntityID), sizeof(int));
		output->write((char*)&(rooms[i]->position.x), sizeof(int));
		output->write((char*)&(rooms[i]->position.y), sizeof(int));
		output->write((char*)&(rooms[i]->hasExteriorDoor), sizeof(bool));
	}
}

void Entity_Interior::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	int roomCount;
	input->read((char*)&roomCount, sizeof(int));
	for (int i = 0; i < roomCount; i++) {
		Room* r = new Room();
		input->read((char*)&(r->roomEntityID), sizeof(int));
		int x, y;
		input->read((char*)&(x), sizeof(int));
		input->read((char*)&(y), sizeof(int));
		r->position = sf::Vector2i(x, y);
		input->read((char*)&(r->hasExteriorDoor), sizeof(bool));
		rooms.push_back(r);
	}
}
#pragma endregion

void Entity_Interior::AddRoom(std::string roomName, sf::Vector2i position, bool hasExterior)
{
	Room* newRoom = new Room;
	Entity_Room* RoomEntity = new Entity_Room();
	RoomEntity->SetEntityData(GameLoader::Instance().GetUniqueID(), false, 47520.0f, 47520.0f, 80000.0f);
	RoomEntity->names = { roomName,"room","floor" };
	RoomEntity->parent = std::make_pair(OnFloor, this);
	RoomEntity->parentEntityDir = (int)OnFloor;
	RoomEntity->parentEntityID = this->uniqueEntityID;
	RoomEntity->attachedToParent = true;
	RoomEntity->worldID = this->worldID;
	children.push_back(std::pair<Position, std::vector<Entity*>>(OnFloor, { RoomEntity }));
	World::Instance().AddEntity(RoomEntity);
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
			switch (dir){
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

