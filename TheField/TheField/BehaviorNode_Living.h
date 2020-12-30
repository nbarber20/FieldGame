#pragma once
#include "BehaviorNode.h"
class BehaviorNode_Living_SetTargetByName : public BehaviorNode
{
public:
	BehaviorNode_Living_SetTargetByName(std::string name) {
		this->name = name;
		SerializationID = 12;
	};
	void WriteData(std::fstream* output) override {
		WriteStringData(name, output);
	};
	void ReadData(std::fstream* input) override {
		name = ReadStringData(input);
	};
	BehaviorStatus Execute() override {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			std::vector<Entity*> entities = living->GetVisibleEntities(true, true, true,true);
			for (auto & entity: entities) {
				for (int n = 0; n < entity->names.size(); n++) {
					if (entity->names[n] == name) {
						living->target = entity;
						return SUCCEEDED;
					}
				}
			}
		}
		return FAILED;
	};
private:
	std::string name;
};

class BehaviorNode_Living_GetSavedTarget : public BehaviorNode
{
public:
	BehaviorNode_Living_GetSavedTarget(std::string name) {
		this->name = name;
		SerializationID = 13;
	};
	virtual void WriteData(std::fstream* output) override {
		WriteStringData(name, output);
	};
	virtual void ReadData(std::fstream* input) override {
		name = ReadStringData(input);
	};
	virtual BehaviorStatus Execute() override {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			if (living->GetSavedTarget(name)) {
				return SUCCEEDED;
			}
		}
		return FAILED;
	};
private:
	std::string name;
};

class BehaviorNode_Living_ReturnHome : public BehaviorNode
{
public:
	BehaviorNode_Living_ReturnHome() {
		SerializationID = 20;
	};
	virtual void WriteData(std::fstream* output) override {
	};
	virtual void ReadData(std::fstream* input) override {
	};
	virtual BehaviorStatus Execute() override {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			Entity* e = World::Instance().GetEntityByID(living->homeID, living->homeWorldID);
			living->SetParent(living->homePosition, e);
		}
		return FAILED;
	};
};

class BehaviorNode_Living_SetSubTargetByName : public BehaviorNode
{
public:
	BehaviorNode_Living_SetSubTargetByName(std::string name) {
		this->name = name;
		SerializationID = 14;
	};
	virtual void WriteData(std::fstream* output) override {
		WriteStringData(name, output);
	};
	virtual void ReadData(std::fstream* input) override {
		name = ReadStringData(input);
	};
	virtual BehaviorStatus Execute() override {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			std::vector<Entity*> entities = living->GetVisibleEntities(true, true, true, true);
			for (auto & entity: entities) {
				for (int n = 0; n < entity->names.size(); n++) {
					if (entity->names[n] == name) {
						living->subTarget = entity;
						return SUCCEEDED;
					}
				}
			}
		}
		return FAILED;
	};
private:
	std::string name;
};

class BehaviorNode_Living_SetBehaviorState : public BehaviorNode
{
public:
	BehaviorNode_Living_SetBehaviorState(Entity_Living::BehaviorState s) {
		this->s = s;
		SerializationID = 15;
	};
	void WriteData(std::fstream* output) override {
		output->write((char*)&(s), sizeof(int));
	};
	void ReadData(std::fstream* input) override {
		input->read((char*)&(s), sizeof(int));
	};
	BehaviorStatus Execute() override {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			living->behaviorState = s;
		}
		return SUCCEEDED;
	};
private:
	Entity_Living::BehaviorState s;
};

class BehaviorNode_Living_AttackTarget : public BehaviorNode
{
public:
	BehaviorNode_Living_AttackTarget(bool sourceWeapon) {
		SerializationID = 16;
		this->sourceWeapon = sourceWeapon;
	};
	void WriteData(std::fstream* output) override {
		output->write((char*)&(sourceWeapon), sizeof(bool));
	};
	void ReadData(std::fstream* input) override {
		input->read((char*)&(sourceWeapon), sizeof(bool));
	};
	BehaviorStatus Execute() override {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			if (living->behaviorState == Entity_Living::Defensive || living->behaviorState == Entity_Living::Enraged) {
				if (living->target != nullptr) {
					living->AttackTarget(sourceWeapon);
				}
			}
		}
		return SUCCEEDED;
	};
private:
	bool sourceWeapon = false;
};

class BehaviorNode_Living_IfThirsty : public BehaviorNode
{
public:
	BehaviorNode_Living_IfThirsty(float threshold) {
		this->threshold = threshold;
		SerializationID = 19;
	};
	void WriteData(std::fstream* output) override {
		output->write((char*)&threshold,sizeof(int));
	};
	void ReadData(std::fstream* input) override {
		input->read((char*)&threshold, sizeof(int));
	};
	BehaviorStatus Execute() override {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			if (living->hydration < threshold) {
				return SUCCEEDED;
			}
		}
		return FAILED;
	};
private:
	float threshold;
};

class BehaviorNode_Living_IfHungry : public BehaviorNode
{
public:
	BehaviorNode_Living_IfHungry(float threshold) {
		this->threshold = threshold;
		SerializationID = 23;
	};
	void WriteData(std::fstream* output) override {
		output->write((char*)&threshold, sizeof(int));
	};
	void ReadData(std::fstream* input) override {
		input->read((char*)&threshold, sizeof(int));
	};
	BehaviorStatus Execute() override {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			if (living->nourishment < threshold) {
				return SUCCEEDED;
			}
		}
		return FAILED;
	};
private:
	float threshold;
};

class BehaviorNode_Living_DrinkTarget : public BehaviorNode
{
public:
	BehaviorNode_Living_DrinkTarget() {
		SerializationID = 17;
	};
	void WriteData(std::fstream* output) override {
	};
	void ReadData(std::fstream* input) override {
	};
	BehaviorStatus Execute() override {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			Entity_Fluid* fluid = dynamic_cast<Entity_Fluid*>(treeParent->parentEntity->target);
			if (fluid) {
				living->Drink(fluid, false);
			}
		}
		return SUCCEEDED;
	};
};

class BehaviorNode_Living_EatTarget : public BehaviorNode
{
public:
	BehaviorNode_Living_EatTarget() {
		SerializationID = 24;
	};
	void WriteData(std::fstream* output) override {
	};
	void ReadData(std::fstream* input) override {
	};
	BehaviorStatus Execute() override {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			Entity_Food* food = dynamic_cast<Entity_Food*>(treeParent->parentEntity->target);
			if (food) {
				living->Eat(food);
			}
		}
		return SUCCEEDED;
	};
};

class BehaviorNode_Living_TargetEntityTypeInSelf : public BehaviorNode
{
public:
	BehaviorNode_Living_TargetEntityTypeInSelf(int hash) {
		SerializationID = 18;
		this->hash = hash;
	};
	void WriteData(std::fstream* output) override {
		output->write((char*)&hash, sizeof(int));
	};
	void ReadData(std::fstream* input) override {
		input->read((char*)&hash, sizeof(int));
	};
	BehaviorStatus Execute() override {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			std::vector<Entity*> entities = living->GetVisibleEntities(false,false,true, true);
			for (auto & entity: entities) {
				if (entity->serializationID == hash) {
					treeParent->parentEntity->target = entity;
					return SUCCEEDED;
				}
			}
		}
		return FAILED;
	};
private:
	int hash;
};


class BehaviorNode_Graze : public BehaviorNode
{
public:
	BehaviorNode_Graze() {
		SerializationID = 25;
	};
	void WriteData(std::fstream* output) override {
	};
	void ReadData(std::fstream* input) override {
	};
	BehaviorStatus Execute() override {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			living->Graze();
		}
		return FAILED;
	};
};