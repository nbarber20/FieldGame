#pragma once
#include "Entity.h"
#include "Entity_Clip.h"
class Entity_Firearm : public Entity
{
public:
	Entity_Firearm() {
		typeID = "Entity_Firearm";
	};
	Entity_Firearm(Entity_Clip::ClipType type)
	{
		typeID = "Entity_Firearm";
		this->clipType = type;
	};
	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
		output->write((char*)&clipType, sizeof(int));

	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		input->read((char*)&clipType, sizeof(int));
	};

	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer) {
		Entity::WriteToJson(writer);
		writer->Key("clipType");
		writer->Int((int)clipType);
	}

	virtual void ReadFromJson(Value& v) {
		Entity::ReadFromJson(v);
		clipType = (Entity_Clip::ClipType)v["clipType"].GetInt();
	}

	virtual ~Entity_Firearm() {};
	void Fire(Entity* target);
	void Reload(Entity* clip);
	Entity_Clip::ClipType clipType;
};

