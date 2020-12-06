#pragma once
#include "Entity_Constructed.h"

class Entity_Human;
class ObservationManager;
class Entity_Readable : public Entity_Constructed
{
public:
	Entity_Readable(bool visibleInsides, float internalVolume, float size) : Entity_Constructed(visibleInsides, internalVolume, size)
	{};
	void Read(Entity_Human* humanRef);
	Languages requiredLanguage;
	std::string text;
};

