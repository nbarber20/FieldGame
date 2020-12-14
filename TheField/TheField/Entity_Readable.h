#pragma once
#include "Entity_Constructed.h"

class Entity_Living;
class ObservationManager;
class Entity_Readable : public Entity_Constructed
{
public:
	Entity_Readable(bool visibleInsides, float internalVolume, float size) : Entity_Constructed(visibleInsides, internalVolume, size){};
	virtual ~Entity_Readable() {};
	void Read(Entity_Living* humanRef);
	Languages requiredLanguage;
	std::string text;
};

