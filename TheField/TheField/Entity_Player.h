#pragma once
#include "Entity_Living.h"
#include <iostream>
#include <istream>
#include <string>
class Entity_Player : public Entity_Living
{
public:
	Entity_Player() {
		serializationID = 13;
		individualName = "You";
		spokenLanguage.push_back(English);
		readingLanguage.push_back(English);
	};
	virtual ~Entity_Player() {};
	Entity_Player* Clone() override {
		return new Entity_Player(*this);
	}
	void Tick() override;

	void CheckForEvents();
	void Look();
	void LookSelf();
	void Look(Entity* subject);
	Entity* FindEntityByName(std::string entityName);
	Entity* FindEntityByName(std::string entityName, std::string adjective);
	Entity* FindEntityByName(std::string entityName, Position realitivePosition, std::string realitiveEntityName);
	Entity* FindEntityByName(std::string entityName, std::string adjective, std::vector<Position> positionBlacklist);
	bool TryMove(Entity* e, Position toPos, Entity* toEntity);
	bool Enter(Entity* e);
	bool Exit(Entity* e);
};

