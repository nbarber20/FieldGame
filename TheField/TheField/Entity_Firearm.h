#pragma once
#include "Entity_Weapon.h"
#include "Entity_Clip.h"
class Entity_Firearm : public Entity_Weapon
{
public:
	Entity_Firearm(Entity_Clip::ClipType type, Entity_Living::DamageType damageType, float damageMultiplier, int damageLethalityLevel) : Entity_Weapon(damageType,damageMultiplier,damageLethalityLevel)
	{
		this->serializationID = 5;
		this->clipType = type;
	};
	virtual ~Entity_Firearm() {};
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v);
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);

	virtual bool Attack(Entity* source, Entity* target) override;
	void Reload(Entity* clip);
protected:
	Entity_Clip::ClipType clipType;
};

