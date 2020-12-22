#pragma once
#include "Entity.h"
class Entity_Food : public Entity
{
public:
	Entity_Food() {};
	virtual ~Entity_Food() {};
	virtual int GetClassHash() override {
		return typeid(this).hash_code();
	}
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
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer) {
		Entity::WriteToJson(writer);
		writer->Key("nutritionalValue");
		writer->Double(nutritionalValue);
		writer->Key("spoiled");
		writer->Bool(spoiled);
	}

	virtual void ReadFromJson(Value& v) {
		Entity::ReadFromJson(v);
		nutritionalValue = v["nutritionalValue"].GetDouble();
		spoiled = v["spoiled"].GetBool();
	}
	float nutritionalValue;
	bool spoiled = false;
};

