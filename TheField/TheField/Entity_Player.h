#pragma once
#include "Entity_Living.h"
#include <iostream>
#include <istream>
#include <string>
#include "Constants.h"
class Entity_Player : public Entity_Living
{
public:
	Entity_Player(bool visibleInsides, float internalVolume, float size) :Entity_Living(visibleInsides, internalVolume, size)
	{
		individualName = "You";
		spokenLanguage.push_back(English);
		readingLanguage.push_back(English);
	}
	virtual ~Entity_Player() {};
	virtual void Tick() override
	{
		Entity_Living::Tick();
		std::vector<Entity*> inMouth = GetInventory(Mouth);
		for (auto object : inMouth)
		{
			TrySwallow(object);
		}
	}

	virtual Entity_Player* Clone() {
		return new Entity_Player(*this);
	}
	virtual std::vector<Entity*>  getVisibleEntities(bool getParent);
	virtual void CheckForEvents();
	virtual void Look();
	virtual Entity* FindEntityByName(std::string entityName);
	virtual Entity* FindEntityByName(std::string entityName, std::string adjective);
	virtual Entity* FindEntityByName(std::string entityName, std::string adjective, std::vector<Position> positionBlacklist);
	virtual bool TryMove(Entity* e, Position toPos, Entity* toEntity);
	virtual bool Enter(Entity* e);
	virtual bool Exit(Entity* e);
	virtual bool Drink(Entity* e, bool drinkAll);
	virtual bool Eat(Entity* e);
	virtual bool TrySwallow(Entity* e);
	Constants constants;
	int strength = 10;
};

