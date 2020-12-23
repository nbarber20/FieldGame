#pragma once
#include "Entity.h"
class Task;

class Entity_Mechanisim : public Entity
{
public:
	struct MechanismBehavior;
	Entity_Mechanisim() {};
	virtual ~Entity_Mechanisim();
	virtual int GetClassHash() override {
		return typeid(this).hash_code();
	}
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);
	virtual void Tick() override;
	bool AttemptBehavior(std::string input, Entity* target);
	Entity* target = nullptr;
	std::vector<MechanismBehavior*> behaviors;
	std::vector<MechanismBehavior*> activeBehaviors;

};

