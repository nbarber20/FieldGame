#pragma once
#include "Entity.h"
class Entity_Human : public Entity
{
public:
	Entity_Human(bool visibleInsides, float internalVolume, float size) :Entity(visibleInsides, internalVolume, size) {};

	std::vector<Languages> spokenLanguage;
	std::vector<Languages> readingLanguage;

};

