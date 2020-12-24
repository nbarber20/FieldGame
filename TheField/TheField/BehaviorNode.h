#pragma once
#include <vector>
#include "BehaviorTree.h"
#include "ObservationManager.h"
#include "GameLoader.h"
#include "Entity_Living.h"
#include "Entity_Mechanisim.h"
class BehaviorNode
{
public:
	enum BehaviorStatus {
		IDLE,
		FAILED,
		SUCCEEDED,
		WAITING,
	};
	BehaviorNode() {
		maxSubNodes =1;
	};
	virtual ~BehaviorNode() {

	}

	virtual void WriteData(std::fstream* output) {
	};

	virtual void ReadData(std::fstream* input) {
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
	 
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
	virtual BehaviorNode::BehaviorStatus Execute() {
		return subNodes[0]->Execute();
	};
	virtual bool AddSubNode(BehaviorNode* node) {
		if (maxSubNodes < 0) {
			subNodes.push_back(node);
			return true;
		}
		else if (subNodes.size() < maxSubNodes) {
			subNodes.push_back(node);
			return true;
		}
		return false;
	}
	BehaviorTree* treeParent;
	BehaviorNode* nodeParent;
	std::vector<BehaviorNode*> subNodes;
protected:
	int maxSubNodes = 0;
};

class BehaviorNode_Sequence: public BehaviorNode
{
public:
	BehaviorNode_Sequence(int timeout = -1) {
		maxSubNodes = -1;
		index = 0;
		waitTime = 0;
		waitTimeout = timeout;
	};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}	
	virtual void WriteData(std::fstream* output) {
		output->write((char*)&waitTimeout, sizeof(int));
	};
	virtual void ReadData(std::fstream* input) {
		input->read((char*)&waitTimeout, sizeof(int));
	};
	virtual BehaviorStatus Execute() {
		for (int i = index; i < subNodes.size(); i++) {
			BehaviorStatus s = subNodes[i]->Execute();
			if (s == WAITING) {
				waitTime++;
				//Node timeout
				if (waitTimeout != -1&& waitTime>=waitTimeout) {
					return FAILED;
				}
				treeParent->waiting = true;
				treeParent->SetWaitReturn(this);				
				index = i;
				return WAITING;
			}
			if (s == FAILED) {
				index = 0;
				return FAILED;
			}
		}
		index = 0;
		return SUCCEEDED;
	};
private:
	int index = 0;
	int waitTime = 0;
	int waitTimeout = 0;
};

class BehaviorNode_WaitTicks : public BehaviorNode
{
public:
	BehaviorNode_WaitTicks() {};
	BehaviorNode_WaitTicks(int time) {
		this->time = time;
		this->current = time;
	};
	virtual void WriteData(std::fstream* output) {
		output->write((char*)&time, sizeof(int));
	};

	virtual void ReadData(std::fstream* input) {
		input->read((char*)&time, sizeof(int));
		this->current = time;
	};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
	virtual BehaviorStatus Execute() {
		if (current > 0) {
			current--;
			return WAITING;
		}
		else {
			current = time;
			return SUCCEEDED;
		}
	};
private:
	int current = 0;
	int time = 0;
};


class BehaviorNode_AddObservation : public BehaviorNode
{
public:
	BehaviorNode_AddObservation() {};
	BehaviorNode_AddObservation(std::string observationString) {
		this->observationString = observationString;
	};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
	virtual void WriteData(std::fstream* output) {
		WriteStringData(observationString, output);
	};

	virtual void ReadData(std::fstream* input) {
		observationString = ReadStringData(input);
	};
	virtual BehaviorStatus Execute() {

		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.information = observationString;
		ObservationManager::Instance().MakeObservation(o);
		return SUCCEEDED;
	};
private:
	std::string observationString;
};


class BehaviorNode_WaitForObservation : public BehaviorNode
{
public:
	BehaviorNode_WaitForObservation() {};
	BehaviorNode_WaitForObservation(std::string observationString) {
		this->observationString = observationString;
	};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
	virtual void WriteData(std::fstream* output) {
		WriteStringData(observationString, output);
	};

	virtual void ReadData(std::fstream* input) {
		observationString = ReadStringData(input);
	};
	virtual BehaviorStatus Execute() {
		std::vector< ObservationManager::Observation> observations = ObservationManager::Instance().GetObservations();
		for (int i = 0; i < observations.size(); i++) {
			if (observations[i].information == observationString) {
				return SUCCEEDED;
			}
		}
		return WAITING;
	};
private:
	std::string observationString;
};

class BehaviorNode_TargetEntityTypeInTarget : public BehaviorNode
{
public:
	BehaviorNode_TargetEntityTypeInTarget(int hash) {
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
		std::vector<Entity*> entities = treeParent->parentEntity->getVisibleEntities(true, true, true);
		for (int i = 0; i < entities.size(); i++) {
			if (entities[i]->GetClassHash() == hash) {
				treeParent->parentEntity->target = entities[i];
				return SUCCEEDED;
			}
		}
		return FAILED;
	};
private:
	int hash;
};

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
		WriteStringData(name,output);
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


class BehaviorNode_ActivateMechanism : public BehaviorNode
{
public:
	BehaviorNode_ActivateMechanism(std::string key) {
		this->key = key;
	};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
	virtual void WriteData(std::fstream* output) {
		WriteStringData(key, output);
	};
	virtual void ReadData(std::fstream* input) {
		key = ReadStringData(input);
	};
	virtual BehaviorStatus Execute() {

		Entity* target = treeParent->parentEntity->target;
		Entity_Mechanisim* mechanismTarget = dynamic_cast<Entity_Mechanisim*>(target);
		if (mechanismTarget) {
			if (mechanismTarget->AttemptBehavior(key, treeParent->parentEntity, treeParent->parentEntity->subTarget)) {
				return SUCCEEDED;
			}
		}
		return FAILED;
	};
private:
	std::string key;
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

class BehaviorNode_MoveToTarget : public BehaviorNode
{
public:
	BehaviorNode_MoveToTarget() {};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
	virtual void WriteData(std::fstream* output) {
	};
	virtual void ReadData(std::fstream* input) {
	};
	virtual BehaviorStatus Execute() {
		Entity* target = treeParent->parentEntity->target;
		if (target != nullptr) {
			treeParent->parentEntity->SetParent(target->parent.first, target->parent.second);
			return SUCCEEDED;
		}
		return FAILED;
	};
};

class BehaviorNode_RunSubTree: public BehaviorNode
{
public:
	BehaviorNode_RunSubTree() {};
	BehaviorNode_RunSubTree(BehaviorTree* tree) {
		this->tree = tree;
	};
	virtual ~BehaviorNode_RunSubTree() {
		delete tree;
	}
	virtual void WriteData(std::fstream* output) {

		size_t len = tree->treeName.size();
		output->write((char*)&(len), sizeof(size_t));
		output->write(tree->treeName.c_str(), len);
	};

	virtual void ReadData(std::fstream* input) {
		size_t namelen;
		input->read((char*)&namelen, sizeof(size_t));
		char* temp = new char[namelen + 1];
		input->read(temp, namelen);
		temp[namelen] = '\0';
		tree = GameLoader::Instance().LoadBehaviorTree(temp);
	};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
	virtual BehaviorStatus Execute() {
		tree->Tick();
		if (tree->waiting)return WAITING;
		return SUCCEEDED;
	};
private:
	BehaviorTree* tree;
};