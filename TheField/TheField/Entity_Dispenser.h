#pragma once
#include "Entity.h"
class Entity_Dispenser : public Entity
{
public:
	Entity_Dispenser() {
		this->SerializationID = 17;
	};
	virtual ~Entity_Dispenser() {}; 
	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
		WriteStringData(prefabName, output);
		output->write((char*)&dispenserType, sizeof(int));

	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		prefabName = ReadStringData(input);
		input->read((char*)&dispenserType, sizeof(int));
	};
	Entity* DispenseEntity();
	std::string prefabName;
	int dispenserType;
};

