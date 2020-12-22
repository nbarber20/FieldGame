#pragma once
#include "Entity.h"
class Task;

class Entity_Mechanisim : public Entity
{
public:
	Entity_Mechanisim() {
		typeID = "Entity_Mechanisim";
	};
	virtual ~Entity_Mechanisim();
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);
	virtual void Tick() override;
	bool AttemptBehavior(std::string input, Entity* target);
	Entity* target = nullptr;
};

