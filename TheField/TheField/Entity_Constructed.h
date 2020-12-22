#pragma once
#include "Entity.h"

class ObservationManager;
class Entity_Constructed : public Entity
{
public:
	Entity_Constructed()
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

	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer) {
		Entity::WriteToJson(writer);
		writer->Key("broken");
		writer->Bool(broken);
		writer->Key("resisitance");
		writer->Int(resisitance);
	}
	virtual void ReadFromJson(Value& v) {
		Entity::ReadFromJson(v);
		broken = v["broken"].GetBool();
		resisitance = v["resisitance"].GetInt();
	}

	bool BreakConstructed(int inputStrength);
	bool broken = false;
	int resisitance = 0;
};

