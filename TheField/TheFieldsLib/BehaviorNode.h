#pragma once
#include <vector>
#include "BehaviorTree.h"
#include "ObservationManager.h"
#include "GameLoader.h"
#include "Entity_Living.h"
#include "Entity_Mechanisim.h"
#include "Entity_Dispenser.h"
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
		SerializationID = 0;
		maxSubNodes =1;
	}
	virtual ~BehaviorNode() {

	}

	virtual void WriteData(std::fstream* output) {
	}

	virtual void ReadData(std::fstream* input) {
	}

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
	virtual BehaviorNode::BehaviorStatus Execute() {
		return subNodes[0]->Execute();
	}
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
	int SerializationID = 0;
protected:
	int maxSubNodes = 0;
};

class BehaviorNode_Sequence: public BehaviorNode
{
public:
	BehaviorNode_Sequence(int timeout = -1) {
		SerializationID = 1;
		maxSubNodes = -1;
		index = 0;
		waitTime = 0;
		waitTimeout = timeout;
	}
	virtual void WriteData(std::fstream* output) override {
		output->write((char*)&waitTimeout, sizeof(int));
	}
	virtual void ReadData(std::fstream* input) override {
		input->read((char*)&waitTimeout, sizeof(int));
	}
	virtual BehaviorStatus Execute() override {
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
	}
private:
	int index = 0;
	int waitTime = 0;
	int waitTimeout = 0;
};

class BehaviorNode_Selector : public BehaviorNode
{
public:
	BehaviorNode_Selector() {
		SerializationID = 2;
		maxSubNodes = 2;
	}
	virtual BehaviorStatus Execute() override {
		BehaviorStatus first = subNodes[0]->Execute();
		if (first == FAILED) {
			return subNodes[1]->Execute();
		}
		return first;
	}
};

class BehaviorNode_ParallelSequence : public BehaviorNode
{
public:
	BehaviorNode_ParallelSequence() {
		SerializationID = 3;
		maxSubNodes = -1;
	}
	virtual void WriteData(std::fstream* output) override {
	}
	virtual void ReadData(std::fstream* input) override {
	}
	virtual BehaviorStatus Execute() override {
		for (auto & subNode : subNodes) {
			BehaviorStatus s = subNode->Execute();
		}
		return SUCCEEDED;
	}
};



class BehaviorNode_WaitTicks : public BehaviorNode
{
public:
	BehaviorNode_WaitTicks(int time) {
		SerializationID = 4;
		this->time = time;
		this->current = time;
	}
	virtual void WriteData(std::fstream* output) override {
		output->write((char*)&time, sizeof(int));
	}

	virtual void ReadData(std::fstream* input) override {
		input->read((char*)&time, sizeof(int));
		this->current = time;
	}
	virtual BehaviorStatus Execute() override {
		if (current > 0) {
			current--;
			return WAITING;
		}
		else {
			current = time;
			return SUCCEEDED;
		}
	}
private:
	int current = 0;
	int time = 0;
};


class BehaviorNode_AddObservation : public BehaviorNode
{
public:
	BehaviorNode_AddObservation(std::string observationString) {
		SerializationID = 6;
		this->observationString = observationString;
	}
	virtual void WriteData(std::fstream* output) override{
		WriteStringData(observationString, output);
	}

	virtual void ReadData(std::fstream* input) override {
		observationString = ReadStringData(input);
	}
	virtual BehaviorStatus Execute() override {
		ObservationManager::Instance().MakeObservation(new Observation_Direct(observationString, nullptr));
		return SUCCEEDED;
	}
private:
	std::string observationString;
};


class BehaviorNode_WaitForObservation : public BehaviorNode
{
public:
	BehaviorNode_WaitForObservation(std::string observationString) {
		SerializationID = 6; 
		this->observationString = observationString;
	}
	virtual void WriteData(std::fstream* output) override {
		WriteStringData(observationString, output);
	}

	virtual void ReadData(std::fstream* input) override {
		observationString = ReadStringData(input);
	}
	virtual BehaviorStatus Execute() override {
		//TODO Make work
		return WAITING;
	}
private:
	std::string observationString;
};

class BehaviorNode_TargetEntityTypeInTarget : public BehaviorNode
{
public:
	BehaviorNode_TargetEntityTypeInTarget(int hash) {
		SerializationID = 7;
		this->hash = hash;
	}
	virtual void WriteData(std::fstream* output) override {
		output->write((char*)&hash, sizeof(int));
	}
	virtual void ReadData(std::fstream* input) override {
		input->read((char*)&hash, sizeof(int));
	}
	virtual BehaviorStatus Execute() override {
		std::vector<Entity*> entities = treeParent->parentEntity->target->GetInventory();
		for (auto & entity: entities) {
			if (entity->serializationID == hash) {
				treeParent->parentEntity->target = entity;
				return SUCCEEDED;
			}
		}
		return FAILED;
	}
private:
	int hash;
};





class BehaviorNode_ActivateMechanism : public BehaviorNode
{
public:
	BehaviorNode_ActivateMechanism(std::string key) {
		SerializationID = 8;
		this->key = key;
	}
	virtual void WriteData(std::fstream* output) override {
		WriteStringData(key, output);
	}
	virtual void ReadData(std::fstream* input)override {
		key = ReadStringData(input);
	}
	virtual BehaviorStatus Execute() override {

		Entity* target = treeParent->parentEntity->target;
		Entity_Mechanisim* mechanismTarget = dynamic_cast<Entity_Mechanisim*>(target);
		if (mechanismTarget) {
			if (mechanismTarget->AttemptBehavior(key, treeParent->parentEntity, treeParent->parentEntity->subTarget)) {
				return SUCCEEDED;
			}
		}
		return FAILED;
	}
private:
	std::string key;
};




class BehaviorNode_MoveToTarget : public BehaviorNode
{
public:
	BehaviorNode_MoveToTarget() {
		SerializationID = 9;
	}
	virtual void WriteData(std::fstream* output) override {
	}
	virtual void ReadData(std::fstream* input) override {
	}
	virtual BehaviorStatus Execute() override {
		Entity* target = treeParent->parentEntity->target;
		if (target != nullptr) {
			treeParent->parentEntity->SetParent(target->parent.first, target->parent.second);
			return SUCCEEDED;
		}
		return FAILED;
	}
};

class BehaviorNode_RunSubTree: public BehaviorNode
{
public:
	BehaviorNode_RunSubTree(BehaviorTree* tree) {
		SerializationID = 10;
		this->tree = tree;
	}
	virtual ~BehaviorNode_RunSubTree() {
		delete tree;
	}
	virtual void WriteData(std::fstream* output) override {

		size_t len = tree->treeName.size();
		output->write((char*)&(len), sizeof(size_t));
		output->write(tree->treeName.c_str(), len);
	}

	virtual void ReadData(std::fstream* input) override {
		size_t namelen;
		input->read((char*)&namelen, sizeof(size_t));
		char* temp = new char[namelen + 1];
		input->read(temp, namelen);
		temp[namelen] = '\0';
		tree = GameLoader::Instance().LoadBehaviorTree(temp);
	}
	virtual BehaviorStatus Execute() override {
		tree->Tick();
		if (tree->waiting)return WAITING;
		return SUCCEEDED;
	}
private:
	BehaviorTree* tree;
};


class BehaviorNode_SpawnPrefab : public BehaviorNode
{
public:
	BehaviorNode_SpawnPrefab(std::string name, Position p, bool sametile) {
		SerializationID = 11;
		this->name = name;
		this->p = p;
		this->sametile = sametile;
	}
	virtual void WriteData(std::fstream* output) override {
		WriteStringData(name, output);
		output->write((char*)&p, sizeof(int));
		output->write((char*)&sametile, sizeof(bool));
	}
	virtual void ReadData(std::fstream* input) override {
		name = ReadStringData(input);
		input->read((char*)&p, sizeof(int));
		input->read((char*)&sametile, sizeof(bool));
	}
	virtual BehaviorStatus Execute() override {
		if (sametile) {
			GameLoader::Instance().SpawnPrefab(name, p, treeParent->parentEntity->parent.second);
		}
		else {
			GameLoader::Instance().SpawnPrefab(name, p, treeParent->parentEntity);
		}
		return SUCCEEDED;
	}
private:
	bool sametile = false;
	std::string name = "";
	Position p = Nowhere;
};

class BehaviorNode_TargetDispenserType : public BehaviorNode
{
public:
	BehaviorNode_TargetDispenserType(int type) {
		SerializationID = 21;
		this->type = type;
	}
	virtual void WriteData(std::fstream* output) override {
		output->write((char*)&type, sizeof(int));
	}
	virtual void ReadData(std::fstream* input) override {
		input->read((char*)&type, sizeof(int));
	}
	virtual BehaviorStatus Execute() override {
		std::vector<Entity*> entities = treeParent->parentEntity->GetVisibleEntities(true,true,true,true);
		for (auto & entity: entities) {
			Entity_Dispenser* disp = dynamic_cast<Entity_Dispenser*>(entity);
			if (disp) {
				if (disp->GetDispenserType() == type) {
					treeParent->parentEntity->target = entity;
					return SUCCEEDED;
				}
			}
		}
		return FAILED;
	}
private:
	int type;
};

class BehaviorNode_UseDispenserAndTargetDispensed : public BehaviorNode
{
public:
	BehaviorNode_UseDispenserAndTargetDispensed() {
		SerializationID = 22;
	}
	virtual BehaviorStatus Execute() override {
		Entity_Dispenser* disp = dynamic_cast<Entity_Dispenser*>(treeParent->parentEntity->target);
		if (disp) {
			treeParent->parentEntity->target = disp->DispenseEntity();
			return SUCCEEDED;
		}
		return FAILED;
	}
};