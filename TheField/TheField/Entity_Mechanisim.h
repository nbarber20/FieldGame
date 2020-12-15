#pragma once
#include "Entity.h"
#include "Task.h"
class Entity_Mechanisim : public Entity
{
public:
	Entity_Mechanisim(bool visibleInsides, float internalVolume, float size) : Entity(visibleInsides, internalVolume, size) {};
	virtual ~Entity_Mechanisim() {
		tasksToExecute.clear();
		for (int i = 0; i < behaviors.size(); i++) {
			for (int j = 0; j < behaviors[i].second.size(); j++) {
				delete behaviors[i].second[j];
			}
			behaviors[i].second.clear();
		}
		behaviors.clear();
	};
	virtual void Tick() override {
		for (int i = taskIndex; i < tasksToExecute.size(); i++) {
			Task* t = tasksToExecute[i];
			t->Execute();
			taskIndex++;
			if (t->instant == false)return;
		}
		taskIndex = 0;
	}

	bool AttemptBehavior(std::string input) {
		for (int i = 0; i < behaviors.size(); i++) {
			if (behaviors[i].first == input) {
				for (int j = 0; j < behaviors[i].second.size(); j++) {
					tasksToExecute.push_back(behaviors[i].second[j]);
				}
				return true;
			}
		}
		return false;
	};

	void AddBehavior(std::pair < std::string, std::vector<Task*>> behaviorToAdd) {
		behaviors.push_back(behaviorToAdd);
	};

private:
	std::vector<std::pair<std::string, std::vector<Task*>>> behaviors;
	std::vector<Task*> tasksToExecute;
	int taskIndex = 0;
};

