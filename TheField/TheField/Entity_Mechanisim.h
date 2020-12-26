#pragma once
#include "Entity.h"
struct MechanismBehavior;
class Entity_Mechanisim : public Entity
{
public:
	
	Entity_Mechanisim() {
		serializationID = 11;
	};
	virtual ~Entity_Mechanisim();
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);

	virtual void Tick() override;
	bool AttemptBehavior(std::string input,Entity* source, Entity* target);
	void AddBehavior(MechanismBehavior* m);
	std::vector<MechanismBehavior*> behaviors;
	std::vector<MechanismBehavior*> activeBehaviors;
	bool playerActivatable = true;
};

