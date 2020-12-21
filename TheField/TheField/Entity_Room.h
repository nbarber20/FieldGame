#pragma once
#include "Entity.h"
class Entity_Room : public Entity
{
public:
	Entity_Room() {
		typeID = "Entity_Room";
		this->visibleInsides = false;
	};
	virtual ~Entity_Room() {};
	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
	};
};

