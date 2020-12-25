#pragma once
#include "Entity.h"
#include "GameLoader.h"
#include "BehaviorTree.h"
class Task;

class Entity_Mechanisim : public Entity
{
public:
	struct MechanismBehavior {
	std::string activationKey;
	std::string firstPersonActivation;
	std::string thirdPersonActivation;
	BehaviorTree* activationTree;
	Entity_Mechanisim* parent;
	void WriteData(std::fstream* output)
	{
		size_t activationKeylen = activationKey.size();
		output->write((char*)&(activationKeylen), sizeof(size_t));
		output->write(activationKey.c_str(), activationKeylen);


		size_t firstPersonActivationlen = firstPersonActivation.size();
		output->write((char*)&(firstPersonActivationlen), sizeof(size_t));
		output->write(firstPersonActivation.c_str(), firstPersonActivationlen);


		size_t thirdPersonActivationlen = activationKey.size();
		output->write((char*)&(thirdPersonActivationlen), sizeof(size_t));
		output->write(activationKey.c_str(), thirdPersonActivationlen);


		size_t treeLen = activationTree->treeName.size();
		output->write((char*)&(treeLen), sizeof(size_t));
		output->write(activationTree->treeName.c_str(), treeLen);

		output->write((char*)&activationTree->waitReturnIndex, sizeof(int));
	}
	void ReadData(std::fstream* input) {
		activationKey = ReadStringData(input);
		firstPersonActivation = ReadStringData(input);
		thirdPersonActivation = ReadStringData(input);
		activationTree = GameLoader::Instance().LoadBehaviorTree(ReadStringData(input));
		int waitReturnIndex;
		input->read((char*)&waitReturnIndex, sizeof(int));
		activationTree->waitReturnIndex = waitReturnIndex;
		activationTree->parentEntity = parent;
	}
	std::string ReadStringData(std::fstream* input) {
		size_t namelen;
		input->read((char*)&namelen, sizeof(size_t));
		char* temp = new char[namelen + 1];
		input->read(temp, namelen);
		temp[namelen] = '\0';
		return temp;
	}
};
	Entity_Mechanisim() {
		SerializationID = 11;
	};
	virtual ~Entity_Mechanisim();
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);
	virtual void Tick() override;
	bool AttemptBehavior(std::string input,Entity* source, Entity* target);
	void AddBehavior(MechanismBehavior* m);
	std::vector<MechanismBehavior*> behaviors;
	std::vector<MechanismBehavior*> activeBehaviors;
	bool playerActivatable = true;


};

