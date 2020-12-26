#pragma once
#include "Entity.h"
class Entity_Room : public Entity
{
public:
	Entity_Room() {
		serializationID = 15;
		this->visibleInsides = false;
	};
	virtual ~Entity_Room() {};
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);
};

