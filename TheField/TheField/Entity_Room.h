#pragma once
#include "Entity.h"
class Entity_Room : public Entity
{
public:
	Entity_Room() {
		typeID = "Entity_Room";
	};
	Entity_Room(int id, bool visibleInsides, float internalVolume, float size) :Entity(id, visibleInsides, internalVolume, size)
	{
		typeID = "Entity_Room";
		this->countable = false;
	};
	virtual ~Entity_Room() {};
};

