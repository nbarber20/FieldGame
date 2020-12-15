#pragma once
#include "World.h"
#include "ObservationManager.h"
class Task
{
public:
	Task() {};
	~Task() {};
	virtual void Execute() {};
	bool instant = true;
};

class Task_DestroySelf : public Task{
public:
	Task_DestroySelf(Entity* self) {
		this->ref = self;
		instant = false;
	};
	virtual void Execute() {
		World::Instance().RemoveEntity(ref);
	};
private:
	Entity* ref;
};

class Task_LogText: public Task {
public:
	Task_LogText(std::string text) {
		this->text = text;
	};
	virtual void Execute() {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.information = text;
		ObservationManager::Instance().MakeObservation(o);
	};
private:
	std::string text;
};

class Task_AttackEntity : public Task {
public:
	Task_AttackEntity(Entity* self) {
		this->ref = self;
	};
	virtual void Execute() {
	};
private:
	Entity* ref;
};