#include "pch.h"
#include "Entity_Mechanisim.h"
#include "GameLoader.h"
#include "BehaviorTree.h"


struct Entity_Mechanisim::MechanismBehavior {
	std::string activationKey;
	std::string firstPersonActivation;
	std::string thirdPersonActivation;
	BehaviorTree* activationTree;
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

Entity_Mechanisim::~Entity_Mechanisim()
{
}

void Entity_Mechanisim::WriteData(std::fstream* output)
{
	Entity::WriteData(output);

	int behaviorLen = behaviors.size();
	output->write((char*)&behaviorLen, sizeof(int));
	for (int i = 0; i < behaviorLen; i++) {
		behaviors[i]->WriteData(output);
	}
	int activeBehaviorLen = activeBehaviors.size();
	output->write((char*)&activeBehaviorLen, sizeof(int));
	for (int i = 0; i < activeBehaviorLen; i++) {
		auto it = std::find(behaviors.begin(), behaviors.end(), activeBehaviors[i]);
		if (it != behaviors.end())
		{
			int index = it - behaviors.begin();
			output->write((char*)&index, sizeof(int)); //writing behavior ref
		}
		else {
			int badIndex = 0;
			output->write((char*)&badIndex, sizeof(int)); //writing null behavior ref
		}
	}

};

void Entity_Mechanisim::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	int behaviorLen;
	input->read((char*)&behaviorLen, sizeof(int));
	for (int i = 0; i < behaviorLen; i++) {
		MechanismBehavior* b = new MechanismBehavior(); 
		b->ReadData(input);
		behaviors.push_back(b);
	}
	int activeBehaviorLen;
	input->read((char*)&activeBehaviorLen, sizeof(int));
	for (int i = 0; i < activeBehaviorLen; i++) {
		int index;
		input->read((char*)&index, sizeof(int));
		activeBehaviors.push_back(behaviors[index]);
	}
};

void Entity_Mechanisim::Tick()
{
	for (int i = 0; i < activeBehaviors.size(); i++) {
		//Tick trees
		if (activeBehaviors[i]->activationTree->Tick() == true) {
			//tree complete
			activeBehaviors.erase(std::remove(activeBehaviors.begin(), activeBehaviors.end(), activeBehaviors[i]), activeBehaviors.end());
		}
	}
}

bool Entity_Mechanisim::AttemptBehavior(std::string input, Entity* target)
{
	for (int i = 0; i < behaviors.size(); i++) {
		if (behaviors[i]->activationKey == input) {
			if (std::find(activeBehaviors.begin(), activeBehaviors.end(), behaviors[i]) == activeBehaviors.end()) {
				activeBehaviors.push_back(behaviors[i]);
			}
		}
	}
	return false;
}

