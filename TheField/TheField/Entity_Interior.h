#pragma once
#include "Entity_Constructed.h"
class Entity_Room;
class Entity_Interior : public Entity_Constructed
{
public:
	struct Room {
		int roomEntityID;
		sf::Vector2i position;
		bool hasExteriorDoor; 
	};
	Entity_Interior() {
		typeID = "Entity_Interior";
		this->visibleInsides = false;
	};
	virtual ~Entity_Interior();

	virtual void WriteData(std::fstream* output) {
		Entity_Constructed::WriteData(output);
		int roomCount = rooms.size();
		output->write((char*)&roomCount, sizeof(int));
		for (int i = 0; i < roomCount; i++) {
			output->write((char*)&(rooms[i]->roomEntityID), sizeof(int));
			output->write((char*)&(rooms[i]->position.x), sizeof(int));
			output->write((char*)&(rooms[i]->position.y), sizeof(int));
			output->write((char*)&(rooms[i]->hasExteriorDoor), sizeof(bool));
		}
	};

	virtual void ReadData(std::fstream* input) {
		Entity_Constructed::ReadData(input);
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
	};

	void AddRoom(std::string roomName, sf::Vector2i position,bool hasExterior);
	int GetRoom(sf::Vector2i position);
	virtual void AddChild(Position pos, Entity* toAdd, int roomIndex) override;
	Entity_Room* GetAdjacent(FacingDirection dir, Entity* room);
	std::vector<Room*> rooms;
};

