#pragma once
#include "Entity.h"
#include "Entity_Living.h"
class Entity_Weapon:public Entity
{
public:
	Entity_Weapon()
	{
	};
	virtual ~Entity_Weapon() {};
	virtual int GetClassHash() override {
		return typeid(this).hash_code();
	}
	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
		output->write((char*)&damageType, sizeof(int));
		output->write((char*)&damageMultiplier, sizeof(float));
		output->write((char*)&damageLethalityLevel, sizeof(int));

	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		input->read((char*)&damageType, sizeof(int));
		input->read((char*)&damageMultiplier, sizeof(float));
		input->read((char*)&damageLethalityLevel, sizeof(int));
	};

	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer) {
		Entity::WriteToJson(writer);
	}

	virtual void ReadFromJson(Value& v) {
		Entity::ReadFromJson(v);
	}

	virtual bool Attack(Entity* source,Entity* target);

	Entity_Living::DamageType damageType = Entity_Living::Blunt;
	float damageMultiplier = .8;
	int damageLethalityLevel = 1;
};

