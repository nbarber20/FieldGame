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

void Entity_Mechanisim::WriteData(std::fstream* output)
{
	Entity::WriteData(output);

	int behaviorsLen = behaviors.size();
	output->write((char*)&behaviorsLen, sizeof(int));
	for (int i = 0; i < behaviorsLen; i++) {
		WriteStringData(behaviors[i].first, output);
		int behaviorsSecLen = behaviors[i].second.size();
		output->write((char*)&behaviorsSecLen, sizeof(int));
		for (int j = 0; j < behaviorsSecLen; j++) {
			behaviors[i].second[j]->WriteData(output);
		}
	}

};

void Entity_Mechanisim::ReadData(std::fstream* input)
{
	Entity::ReadData(input);

	int behaviorsLen;
	input->read((char*)&behaviorsLen, sizeof(int));
	for (int i = 0; i < behaviorsLen; i++) {
		std::string bname = ReadStringData(input);

		std::vector<Task> taskList;
		int behaviorsSecLen;
		input->read((char*)&behaviorsSecLen, sizeof(int));

		std::vector<Task*> tasks;
		for (int j = 0; j < behaviorsSecLen; j++) {
			std::string taskType = ReadStringData(input);
			//TODO needs cleanup
			Task* t;
			if (taskType == "Task_DestroySelf") {
				t = new Task_DestroySelf(this);
			}
			else if (taskType == "Task_LogText") {
				t = new Task_LogText(this);
			}
			else if (taskType == "Task_AttackEntity") {
				t = new Task_AttackEntity(this);
			}
			else {
				t = new Task(this);
			}
			t->ReadData(input);
			tasks.push_back(t);
		}
		AddBehavior(std::make_pair(bname, tasks));
	}
};

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
