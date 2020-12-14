#pragma once
#include "Entity.h"
class Entity_Room : public Entity
{
public:
	Entity_Room(bool visibleInsides, float internalVolume, float size) :Entity(visibleInsides, internalVolume, size)
	{
		this->coutable = false;
	};
	virtual ~Entity_Room() {};
};

