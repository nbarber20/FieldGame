#pragma once
#include "Entity_Constructed.h"
class Entity_Room;
class Entity_Interior : public Entity_Constructed
{
public:
	struct Room {
		Entity_Room* roomEntity;
		sf::Vector2i position;
		bool hasExteriorDoor; 
	};

	Entity_Interior(bool visibleInsides, float internalVolume, float size) : Entity_Constructed(visibleInsides, internalVolume, size)
	{
		this->coutable = false;
	}
	virtual ~Entity_Interior() {
		for (auto room : rooms)
		{
			delete room;
		}
		rooms.clear();
	}

	void AddRoom(std::string roomName, sf::Vector2i position,bool hasExterior);
	int GetRoom(sf::Vector2i position);
	virtual void AddChild(Position pos, Entity* toAdd, int roomIndex) override;
	Entity_Room* GetAdjacent(FacingDirection dir, Entity* room);
	std::vector<Room*> rooms;
};

