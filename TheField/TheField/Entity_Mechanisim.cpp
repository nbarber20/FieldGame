#include "pch.h"
#include "Entity_Mechanisim.h"
#include "Entity_Player.h"
#include "MechanismBehavior.h"
#include "ObservationManager.h"

#pragma region Serialization
void Entity_Mechanisim::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);
	writer->Key("playerActivatable");
	writer->Bool(playerActivatable);

	writer->Key("behaviors");
	writer->StartArray();
	for (int i = 0; i < behaviors.size(); i++) {
		writer->String(behaviors[i]->activationKey.c_str());
	}
	writer->EndArray();

	writer->Key("behaviorsSecondPerson");
	writer->StartArray();
	for (int i = 0; i < behaviors.size(); i++) {
		writer->String(behaviors[i]->secondPersonActivation.c_str());
	}
	writer->EndArray();

	writer->Key("behaviorssThirdPerson");
	writer->StartArray();
	for (int i = 0; i < behaviors.size(); i++) {
		writer->String(behaviors[i]->thirdPersonActivation.c_str());
	}
	writer->EndArray();

	writer->Key("behaviorsTrees");
	writer->StartArray();
	for (int i = 0; i < behaviors.size(); i++) {
		writer->String(behaviors[i]->activationTree->treeName.c_str());
	}
	writer->EndArray();

	writer->Key("activeBehaviors");
	writer->StartArray();
	for (int i = 0; i < activeBehaviors.size(); i++) {

		auto it = std::find(behaviors.begin(), behaviors.end(), activeBehaviors[i]);
		if (it != behaviors.end())
		{
			int index = it - behaviors.begin();
			writer->Int(index);
		}
	}
	writer->EndArray();

}

void Entity_Mechanisim::ReadFromJson(Value& v)
{
	Entity::ReadFromJson(v);
	playerActivatable = v["playerActivatable"].GetBool();

	behaviors.clear();
	for (int i = 0; i < v["behaviors"].Size(); i++) {
		MechanismBehavior* m = new MechanismBehavior();
		m->activationKey = v["behaviors"][i].GetString();
		m->parent = this;
		behaviors.push_back(m);
	}
	for (int i = 0; i < behaviors.size(); i++) {
		behaviors[i]->secondPersonActivation = v["behaviorsSecondPerson"][i].GetString();
	}
	for (int i = 0; i < behaviors.size(); i++) {
		behaviors[i]->secondPersonActivation = v["behaviorssThirdPerson"][i].GetString();
	}
	for (int i = 0; i < behaviors.size(); i++) {
		std::string treeName = v["behaviorssThirdPerson"][i].GetString();
		BehaviorTree* tree = GameLoader::Instance().LoadBehaviorTree(treeName);
		tree->parentEntity = this;
		behaviors[i]->activationTree = tree;
	}
	activeBehaviors.clear();
	for (int i = 0; i < v["activeBehaviors"].Size(); i++) {
		activeBehaviors.push_back(behaviors[v["activeBehaviors"].GetInt()]);
	}
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
	output->write((char*)&playerActivatable, sizeof(bool));

};

void Entity_Mechanisim::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	int behaviorLen;
	behaviors.clear();
	input->read((char*)&behaviorLen, sizeof(int));
	for (int i = 0; i < behaviorLen; i++) {
		MechanismBehavior* b = new MechanismBehavior(); 
		b->parent = this;
		b->ReadData(input);
		behaviors.push_back(b);
	}
	activeBehaviors.clear();
	int activeBehaviorLen;
	input->read((char*)&activeBehaviorLen, sizeof(int));
	for (int i = 0; i < activeBehaviorLen; i++) {
		int index;
		input->read((char*)&index, sizeof(int));
		activeBehaviors.push_back(behaviors[index]);
	}
	input->read((char*)&playerActivatable, sizeof(bool));
};
#pragma endRegion

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

bool Entity_Mechanisim::AttemptBehavior(std::string input, Entity* source, Entity* target)
{
	//TODO player activateable
	for (int i = 0; i < behaviors.size(); i++) {
		if (behaviors[i]->activationKey == input) {
			if (std::find(activeBehaviors.begin(), activeBehaviors.end(), behaviors[i]) == activeBehaviors.end()) {
				ObservationManager::Instance().MakeObservation(new Observation_Action(behaviors[i]->secondPersonActivation, behaviors[i]->thirdPersonActivation, this, source));
				this->target = target;
				activeBehaviors.push_back(behaviors[i]);
				return true;
			}
		}
	}
	return false;
}

void Entity_Mechanisim::AddBehavior(MechanismBehavior* m)
{
	m->activationTree->parentEntity = this;
	behaviors.push_back(m);
}

