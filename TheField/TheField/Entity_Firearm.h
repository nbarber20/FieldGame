#pragma once
#include "Entity_Weapon.h"
#include "Entity_Clip.h"
class Entity_Firearm : public Entity_Weapon
{
public:
	Entity_Firearm(Entity_Clip::ClipType type)
	{
		SerializationID = 5;
		this->clipType = type;
	};
	virtual ~Entity_Firearm() {};
	virtual void WriteData(std::fstream* output) {
		Entity_Weapon::WriteData(output);
		output->write((char*)&clipType, sizeof(int));

	};
	virtual void ReadData(std::fstream* input) {
		Entity_Weapon::ReadData(input);
		input->read((char*)&clipType, sizeof(int));
	};

	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer) {
		Entity_Weapon::WriteToJson(writer);
		writer->Key("clipType");
		writer->Int((int)clipType);
	}

	virtual void ReadFromJson(Value& v) {
		Entity_Weapon::ReadFromJson(v);
		clipType = (Entity_Clip::ClipType)v["clipType"].GetInt();
	}

	virtual bool Attack(Entity* source, Entity* target) override;
	void Reload(Entity* clip);
	Entity_Clip::ClipType clipType;
};

