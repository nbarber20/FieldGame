#pragma once
#include "Entity.h"

class ObservationManager;
class Entity_Constructed : public Entity
{
public:
	Entity_Constructed(bool visibleInsides, float internalVolume, float size) : Entity(visibleInsides, internalVolume, size)
	{

	}
	virtual ~Entity_Constructed() {};

	bool BreakConstructed(int inputStrength);
	bool broken = false;
	int resisitance = 0;
};

