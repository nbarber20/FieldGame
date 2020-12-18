#pragma once
#include "Entity.h"
class Entity_Food : public Entity
{
public:
	Entity_Food() {
		typeID = "Entity_Food";
	};
	Entity_Food(int id, bool visibleInsides, float internalVolume, float size) : Entity(id, visibleInsides, internalVolume, size){
		typeID = "Entity_Food";
	};
	virtual ~Entity_Food() {};
	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
		output->write((char*)&nutritionalValue, sizeof(float));
		output->write((char*)&spoiled, sizeof(bool));

	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		input->read((char*)&nutritionalValue, sizeof(float));
		input->read((char*)&spoiled, sizeof(bool));
	};
	float nutritionalValue;
	bool spoiled = false;
};

