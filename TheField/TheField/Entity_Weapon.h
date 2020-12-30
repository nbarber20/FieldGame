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
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	virtual bool Attack(Entity* source, Entity* target);
protected:
	Entity_Living::DamageType damageType = Entity_Living::Blunt;
	float damageMultiplier;
	int damageLethalityLevel;
};

