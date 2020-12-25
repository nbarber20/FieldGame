#pragma once
#include "Entity_Living.h"
#include <iostream>
#include <istream>
#include <string>
#include "Constants.h"
class Entity_Player : public Entity_Living
{
public:
	Entity_Player() {
		SerializationID = 13;
		individualName = "You";
		spokenLanguage.push_back(English);
		readingLanguage.push_back(English);
	};
	virtual ~Entity_Player() {};
	virtual void Tick() override;

	virtual Entity_Player* Clone() {
		return new Entity_Player(*this);
	}
	virtual void CheckForEvents();
	virtual void Look();
	virtual void LookSelf();
	virtual void Look(Entity* subject);
	virtual Entity* FindEntityByName(std::string entityName);
	virtual Entity* FindEntityByName(std::string entityName, std::string adjective);
	virtual Entity* FindEntityByName(std::string entityName, Position realitivePosition, std::string realitiveEntityName);
	virtual Entity* FindEntityByName(std::string entityName, std::string adjective, std::vector<Position> positionBlacklist);
	virtual bool TryMove(Entity* e, Position toPos, Entity* toEntity);
	virtual bool Enter(Entity* e);
	virtual bool Exit(Entity* e);
	Constants constants;
	int strength = 10;
};

