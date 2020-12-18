#pragma once
#include "World.h"
#include "ObservationManager.h"
class Task
{
public:
	Task(Entity* self) {
		this->self = self;
	};
	~Task() {};
	virtual void WriteData(std::fstream* output) 
	{
		WriteStringData(typeID, output);
		output->write((char*)&instant, sizeof(bool));
	};
	virtual void ReadData(std::fstream* input) {
		input->read((char*)&instant, sizeof(bool));
	};
	void WriteStringData(std::string s, std::fstream* output) {
		size_t len = s.size();
		output->write((char*)&(len), sizeof(size_t));
		output->write(s.c_str(), len);
	}
	std::string ReadStringData(std::fstream* input) {

		size_t namelen;
		input->read((char*)&namelen, sizeof(size_t));
		char* temp = new char[namelen + 1];
		input->read(temp, namelen);
		temp[namelen] = '\0';
		return temp;
	}

	virtual void Execute() {};
	bool instant = true;
protected:
	Entity* self;
	std::string typeID = "Task";
};

class Task_DestroySelf : public Task{
public:
	Task_DestroySelf(Entity* self) :Task(self) {
		instant = false;
		typeID = "Task_DestroySelf";
	};
	virtual void WriteData(std::fstream* output) {
		Task::WriteData(output);
	};
	virtual void ReadData(std::fstream* input) {
		Task::ReadData(input);
	};
	virtual void Execute() {
		World::Instance().RemoveEntity(self);
	};
};

class Task_LogText: public Task {
public:
	Task_LogText(Entity* self) :Task(self) {
		typeID = "Task_LogText";
	};
	Task_LogText(Entity* self, std::string text) :Task(self) {
		typeID = "Task_LogText";
		this->text = text;
	};
	virtual void WriteData(std::fstream* output) {
		Task::WriteData(output);
		WriteStringData(text, output);
	};
	virtual void ReadData(std::fstream* input) {
		Task::ReadData(input);
		text = ReadStringData(input);
	};
	virtual void Execute() {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.information = text;
		ObservationManager::Instance().MakeObservation(o);
	};
protected:
	std::string text;
};

class Task_AttackEntity : public Task {
public:
	Task_AttackEntity(Entity* self) :Task(self) {
		typeID = "Task_AttackEntity";
	};
	Task_AttackEntity(Entity* self, Entity_Living::DamageType damageType, float damageMultiplier, int lethality) : Task(self){
		typeID = "Task_AttackEntity";
		this->damageMultiplier = damageMultiplier;
		this->lethality = lethality;
		this->damageType = damageType;
	};
	virtual void WriteData(std::fstream* output) {
		Task::WriteData(output);
		output->write((char*)&damageType, sizeof(int));
		output->write((char*)&damageMultiplier, sizeof(float));
		output->write((char*)&lethality, sizeof(int));
	};
	virtual void ReadData(std::fstream* input) {
		Task::ReadData(input);
		input->read((char*)&damageType, sizeof(int));
		input->read((char*)&damageMultiplier, sizeof(float));
		input->read((char*)&lethality, sizeof(int));
	};
	virtual void Execute() {
		Entity_Mechanisim* mechanism = dynamic_cast<Entity_Mechanisim*>(self);
		if (mechanism) {
			if (mechanism->target) {
				Entity_Living* living = dynamic_cast<Entity_Living*>(mechanism->target);
				if (living) {
					living->TakeDamage(damageType, damageMultiplier, lethality);
				}
			}
		}
	};
protected:
	Entity_Living::DamageType damageType;
	float damageMultiplier;
	int lethality;
};