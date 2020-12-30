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

	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	void AddRoom(std::string roomName, sf::Vector2i position,bool hasExterior);
	int GetRoom(sf::Vector2i position);
	virtual void AddChild(Position pos, Entity* toAdd, int roomIndex) override;
	Entity_Room* GetAdjacent(FacingDirection dir, Entity* room);
	std::vector<Room*> rooms;
};

