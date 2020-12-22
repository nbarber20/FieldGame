#pragma once
#include "Entity_Constructed.h"

class Entity_Living;
class ObservationManager;
class Entity_Readable : public Entity_Constructed
{
public:
	Entity_Readable() {};
	virtual ~Entity_Readable() {};
	virtual int GetClassHash() override {
		return typeid(this).hash_code();
	}
	void Read(Entity_Living* humanRef);
	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
		output->write((char*)&requiredLanguage, sizeof(int));
		WriteStringData(text, output);
	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		input->read((char*)&requiredLanguage, sizeof(int));
		text = ReadStringData(input);
	};
	Languages requiredLanguage;
	std::string text;
};

