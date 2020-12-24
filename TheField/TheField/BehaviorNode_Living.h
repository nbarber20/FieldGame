#pragma once
#include "BehaviorNode.h"
class BehaviorNode_Living_SetTargetByName : public BehaviorNode
{
public:
	BehaviorNode_Living_SetTargetByName(std::string name) {
		this->name = name;
	};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
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

class BehaviorNode_Living_SetSubTargetByName : public BehaviorNode
{
public:
	BehaviorNode_Living_SetSubTargetByName(std::string name) {
		this->name = name;
	};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
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
	BehaviorNode_Living_SetBehaviorState() {};
	BehaviorNode_Living_SetBehaviorState(Entity_Living::BehaviorState s) {
		this->s = s;
	};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
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
	BehaviorNode_Living_AttackTarget() {};
	BehaviorNode_Living_AttackTarget(bool sourceWeapon) {
		this->sourceWeapon = sourceWeapon;
	};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
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

class BehaviorNode_Living_DrinkTarget : public BehaviorNode
{
public:
	BehaviorNode_Living_DrinkTarget() {};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
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
		this->hash = hash;
	};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
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
				if (entities[i]->GetClassHash() == hash) {
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