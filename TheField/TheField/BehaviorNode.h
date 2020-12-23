#pragma once
#include <vector>
#include "BehaviorTree.h"
#include "ObservationManager.h"
#include "GameLoader.h"
#include "Entity_Living.h"
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
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
	virtual BehaviorStatus Execute() {
		subNodes[0]->Execute();
		return SUCCEEDED;
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
	BehaviorNode_Sequence() {
		maxSubNodes = -1;
		index = 0;
	};
	virtual int GetClassHash() {
		return typeid(this).hash_code();
	}
	virtual BehaviorStatus Execute() {
		for (int i = index; i < subNodes.size(); i++) {
			BehaviorStatus s = subNodes[i]->Execute();
			if (s == WAITING) {
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

		size_t len = observationString.size();
		output->write((char*)&(len), sizeof(size_t));
		output->write(observationString.c_str(), len);
	};

	virtual void ReadData(std::fstream* input) {
		size_t namelen;
		input->read((char*)&namelen, sizeof(size_t));
		char* temp = new char[namelen + 1];
		input->read(temp, namelen);
		temp[namelen] = '\0';
		observationString = temp;
		delete temp;
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