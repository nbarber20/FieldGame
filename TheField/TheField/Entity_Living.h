#pragma once
#include "Entity.h"
class Entity_Living : public Entity
{
public:
	Entity_Living(bool visibleInsides, float internalVolume, float size) :Entity(visibleInsides, internalVolume, size) {};
	virtual ~Entity_Living() {};
	std::vector<Languages> spokenLanguage;
	std::vector<Languages> readingLanguage;
	virtual void Tick() override;
	
	void AddNourishment(float delta);
	void AddHydration(float delta);

	float nourishment = 50;
	float hydration = 50;
	float maxNourishment = 50;
	float maxHydration = 50;
};

