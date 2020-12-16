#include "pch.h"
#include "Entity_Mechanisim.h"
#include "Task.h"

Entity_Mechanisim::~Entity_Mechanisim()
{
	tasksToExecute.clear();
	for (int i = 0; i < behaviors.size(); i++) {
		for (int j = 0; j < behaviors[i].second.size(); j++) {
			delete behaviors[i].second[j];
		}
		behaviors[i].second.clear();
	}
	behaviors.clear();
}

void Entity_Mechanisim::Tick()
{
	for (int i = taskIndex; i < tasksToExecute.size(); i++) {
		Task* t = tasksToExecute[i];
		t->Execute();
		taskIndex++;
		if (t->instant == false)return;
	}
	tasksToExecute.clear();
	taskIndex = 0;
}

bool Entity_Mechanisim::AttemptBehavior(std::string input, Entity* target)
{
	for (int i = 0; i < behaviors.size(); i++) {
		if (behaviors[i].first == input) {
			for (int j = 0; j < behaviors[i].second.size(); j++) {
				tasksToExecute.push_back(behaviors[i].second[j]);
			}
			this->target = target;
			return true;
		}
	}
	return false;
}

void Entity_Mechanisim::AddBehavior(std::pair < std::string, std::vector<Task*>> behaviorToAdd)
{
	behaviors.push_back(behaviorToAdd);
};
