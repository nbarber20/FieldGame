#pragma once
#include "Entity.h"
class Entity_Room;
class Entity_Interior : public Entity
{
public:
	struct Room {
		int roomEntityID;
		sf::Vector2i position;
		bool hasExteriorDoor; 
	};
	Entity_Interior() {
		this->serializationID = 9;
		this->visibleInsides = false;
	}
	virtual ~Entity_Interior() {
		for (auto room : rooms)
		{
			delete room;
		}
		rooms.clear();
	}

	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v);
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);

	void AddRoom(std::string roomName, sf::Vector2i position,bool hasExterior);
	int GetRoom(sf::Vector2i position);
	virtual void AddChild(Position pos, Entity* toAdd, int roomIndex) override;
	Entity_Room* GetAdjacent(FacingDirection dir, Entity* room);
	std::vector<Room*> rooms;
};

