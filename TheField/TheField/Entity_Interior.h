#pragma once
#include "Entity_Constructed.h"
class Entity_Interior : public Entity_Constructed
{
public:
	struct Room {
		Entity* roomEntity;
		bool hasExteriorDoor; 
		//Room* NorthRoom;
		//Room* EastRoom;
		//Room* SouthRoom;
		//Room* WestRoom;
	};

	Entity_Interior(bool visibleInsides, float internalVolume, float size) : Entity_Constructed(visibleInsides, internalVolume, size)
	{

	}
	void AddRoom();
	virtual void AddChild(Position pos, Entity* toAdd) override;


	std::vector<Room*> rooms;
};

