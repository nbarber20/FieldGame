#pragma once
#include "Entity.h"
class Entity_GroundTile : public Entity
{
public:
	Entity_GroundTile(bool visibleInsides, float internalVolume, float size) :Entity(visibleInsides,internalVolume,size) {};
	virtual Entity_GroundTile* Clone() {
		return new Entity_GroundTile(*this);
	}
};

