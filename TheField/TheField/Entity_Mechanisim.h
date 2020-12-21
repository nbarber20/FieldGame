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
	void AddBehavior(std::pair < std::string, std::vector<Task*>> behaviorToAdd);
	Entity* target = nullptr;
private:
	std::vector<std::pair<std::string, std::vector<Task*>>> behaviors;
	std::vector<Task*> tasksToExecute;
	int taskIndex = 0;
};

