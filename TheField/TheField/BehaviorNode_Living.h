#pragma once
#include "BehaviorNode.h"
class BehaviorNode_Living_SetTargetByName : public BehaviorNode
{
public:
	BehaviorNode_Living_SetTargetByName(std::string name) {
		this->name = name;
		SerializationID = 12;
	};
	virtual void WriteData(std::fstream* output) {
		WriteStringData(name, output);
	};
	virtual void ReadData(std::fstream* input) {
		name = ReadStringData(input);
	};
	virtual BehaviorStatus Execute() {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			std::vector<Entity*> entities = living->getVisibleEntities(true, true, true);
			for (int i = 0; i < entities.size(); i++) {
				for (int n = 0; n < entities[i]->names.size(); n++) {
					if (entities[i]->names[n] == name) {
						living->target = entities[i];
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
	virtual void WriteData(std::fstream* output) {
		WriteStringData(name, output);
	};
	virtual void ReadData(std::fstream* input) {
		name = ReadStringData(input);
	};
	virtual BehaviorStatus Execute() {
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
	virtual void WriteData(std::fstream* output) {
	};
	virtual void ReadData(std::fstream* input) {
	};
	virtual BehaviorStatus Execute() {
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
	virtual void WriteData(std::fstream* output) {
		WriteStringData(name, output);
	};
	virtual void ReadData(std::fstream* input) {
		name = ReadStringData(input);
	};
	virtual BehaviorStatus Execute() {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			std::vector<Entity*> entities = living->getVisibleEntities(true, true, true);
			for (int i = 0; i < entities.size(); i++) {
				for (int n = 0; n < entities[i]->names.size(); n++) {
					if (entities[i]->names[n] == name) {
						living->subTarget = entities[i];
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
	virtual void WriteData(std::fstream* output) {
		output->write((char*)&(s), sizeof(int));
	};
	virtual void ReadData(std::fstream* input) {
		input->read((char*)&(s), sizeof(int));
	};
	virtual BehaviorStatus Execute() {
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
	virtual void WriteData(std::fstream* output) {
		output->write((char*)&(sourceWeapon), sizeof(bool));
	};
	virtual void ReadData(std::fstream* input) {
		input->read((char*)&(sourceWeapon), sizeof(bool));
	};
	virtual BehaviorStatus Execute() {
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
	virtual void WriteData(std::fstream* output) {
		output->write((char*)&threshold,sizeof(int));
	};
	virtual void ReadData(std::fstream* input) {
		input->read((char*)&threshold, sizeof(int));
	};
	virtual BehaviorStatus Execute() {
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

class BehaviorNode_Living_DrinkTarget : public BehaviorNode
{
public:
	BehaviorNode_Living_DrinkTarget() {
		SerializationID = 17;
	};
	virtual void WriteData(std::fstream* output) {
	};
	virtual void ReadData(std::fstream* input) {
	};
	virtual BehaviorStatus Execute() {
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

class BehaviorNode_Living_TargetEntityTypeInSelf : public BehaviorNode
{
public:
	BehaviorNode_Living_TargetEntityTypeInSelf(int hash) {
		SerializationID = 18;
		this->hash = hash;
	};
	virtual void WriteData(std::fstream* output) {
		output->write((char*)&hash, sizeof(int));
	};
	virtual void ReadData(std::fstream* input) {
		input->read((char*)&hash, sizeof(int));
	};
	virtual BehaviorStatus Execute() {
		Entity_Living* living = dynamic_cast<Entity_Living*>(treeParent->parentEntity);
		if (living) {
			std::vector<Entity*> entities = living->getVisibleEntities(false,false,true);
			for (int i = 0; i < entities.size(); i++) {
				if (entities[i]->SerializationID == hash) {
					treeParent->parentEntity->target = entities[i];
					return SUCCEEDED;
				}
			}
		}
		return FAILED;
	};
private:
	int hash;
};