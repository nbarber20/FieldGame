#pragma once
#include "Entity.h"
class Entity_Food : public Entity
{
public:
	Entity_Food(bool visibleInsides, float internalVolume, float size) : Entity(visibleInsides, internalVolume, size) 
	{};
	virtual ~Entity_Food() {};
	float nutritionalValue;
	bool spoiled = false;
};

