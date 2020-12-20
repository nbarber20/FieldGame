#pragma once
#include "Entity.h"
class Entity_Room : public Entity
{
public:
	Entity_Room() {
		typeID = "Entity_Room";
	};
	Entity_Room(int id, bool visibleInsides, float internalVolume, float size) :Entity(id, visibleInsides, internalVolume, size)
	{
		typeID = "Entity_Room";
		this->countable = false;
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

