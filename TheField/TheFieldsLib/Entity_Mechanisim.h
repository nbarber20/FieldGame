#pragma once
#include "Entity.h"
struct MechanismBehavior;
class Entity_Mechanisim : public Entity
{
public:
	
	Entity_Mechanisim() {
		serializationID = 11;
	}
	virtual ~Entity_Mechanisim() {}
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	void Tick() override;
	bool AttemptBehavior(std::string input,Entity* source, Entity* target);
	void AddBehavior(MechanismBehavior* m);
	std::vector<MechanismBehavior*> behaviors;
	std::vector<MechanismBehavior*> activeBehaviors;
	bool playerActivatable = true;
};

