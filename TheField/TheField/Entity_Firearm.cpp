#include "pch.h"
#include "Entity_Firearm.h"
#include "Entity_Living.h"
#include "ObservationManager.h"

void Entity_Firearm::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity_Weapon::WriteToJson(writer);
	writer->Key("clipType");
	writer->Int((int)clipType);
}

void Entity_Firearm::ReadFromJson(Value& v)
{
	Entity_Weapon::ReadFromJson(v);
	clipType = (Entity_Clip::ClipType)v["clipType"].GetInt();
}

void Entity_Firearm::WriteData(std::fstream* output)
{
	Entity_Weapon::WriteData(output);
	output->write((char*)&clipType, sizeof(int));
}

void Entity_Firearm::ReadData(std::fstream* input)
{
	Entity_Weapon::ReadData(input);
	input->read((char*)&clipType, sizeof(int));
}


bool Entity_Firearm::Attack(Entity* source, Entity* target)
{
	std::vector<Entity*> inv = GetInventory(Inside);
	for (int i = 0; i < inv.size(); i++) {
		Entity_Clip* clip = dynamic_cast<Entity_Clip*>(inv[i]);
		if (clip) {
			if (clip->FireBullet(1) > 0) {
				Entity_Living* liv = dynamic_cast<Entity_Living*>(target);
				if (liv) {
					ObservationManager::Instance().MakeObservation(new Observation_Action("fire","fires",this, source));
					liv->TakeDamage(this, Entity_Living::Piercing, 1, 3);
					return true;
				}
			}
		}
	}
	return false;
}

void Entity_Firearm::Reload(Entity* clip)
{
	Entity_Clip* clipTest = dynamic_cast<Entity_Clip*>(clip);
	if (clipTest) {
		if (clipTest->GetClipType() == this->clipType) {
			clipTest->SetParentOverride(Inside, this);
		}
	}
}
