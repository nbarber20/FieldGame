#include "pch.h"
#include "Entity_Mechanisim.h"
#include "Entity_Player.h"
#include "ObservationManager.h"



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
	output->write((char*)&playerActivatable, sizeof(bool));

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
	input->read((char*)&playerActivatable, sizeof(bool));
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

bool Entity_Mechanisim::AttemptBehavior(std::string input, Entity* source, Entity* target)
{
	for (int i = 0; i < behaviors.size(); i++) {
		if (behaviors[i]->activationKey == input) {
			if (std::find(activeBehaviors.begin(), activeBehaviors.end(), behaviors[i]) == activeBehaviors.end()) {
				Entity_Player* playerTest = dynamic_cast<Entity_Player*>(source);
				if (playerTest) {
					ObservationManager::Observation o = ObservationManager::Observation();
					o.sense = ObservationManager::SENSE_Look;
					o.type = ObservationManager::TYPE_Direct;
					o.information = source->names[0]+" "+ behaviors[i]->firstPersonActivation+" the "+ names[0];
					ObservationManager::Instance().MakeObservation(o);
				}
				else {
					ObservationManager::Observation o = ObservationManager::Observation();
					o.sense = ObservationManager::SENSE_Look;
					o.type = ObservationManager::TYPE_Direct;
					o.information = source->names[0] + " " + behaviors[i]->thirdPersonActivation + " the " + names[0];
					ObservationManager::Instance().MakeObservation(o);
				}
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

