#pragma once
#include "Entity.h"
#include "Entity_Living.h"
class Entity_Weapon:public Entity
{
public:
	Entity_Weapon(Entity_Living::DamageType damageType, float damageMultiplier, int damageLethalityLevel)
	{
		this->serializationID = 16;
		this->damageType = damageType;
		this->damageMultiplier = damageMultiplier;
		this->damageLethalityLevel = damageLethalityLevel;
	};
	virtual ~Entity_Weapon() {};
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v);
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);

	virtual bool Attack(Entity* source, Entity* target);
protected:
	Entity_Living::DamageType damageType = Entity_Living::Blunt;
	float damageMultiplier;
	int damageLethalityLevel;
};

