#pragma once
#include "Entity.h"

class ObservationManager;
class Entity_Constructed : public Entity
{
public:
	Entity_Constructed() {
		typeID = "Entity_Constructed";
	};
	Entity_Constructed(int id, bool visibleInsides, float internalVolume, float size) : Entity(id, visibleInsides, internalVolume, size)
	{
		typeID = "Entity_Constructed";
	}
	virtual ~Entity_Constructed() {};

	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
		output->write((char*)&broken, sizeof(bool));
		output->write((char*)&resisitance, sizeof(int));

	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		input->read((char*)&broken, sizeof(bool));
		input->read((char*)&resisitance, sizeof(int));
	};

	bool BreakConstructed(int inputStrength);
	bool broken = false;
	int resisitance = 0;
};

