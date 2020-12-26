#include "pch.h"
#include "Entity_Weapon.h"
#include "ObservationManager.h"

#pragma region Serialization

void Entity_Weapon::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);
}

void Entity_Weapon::ReadFromJson(Value& v)
{
	Entity::ReadFromJson(v);
}

void Entity_Weapon::WriteData(std::fstream* output)
{
	Entity::WriteData(output);
	output->write((char*)&damageType, sizeof(int));
	output->write((char*)&damageMultiplier, sizeof(float));
	output->write((char*)&damageLethalityLevel, sizeof(int));
}

void Entity_Weapon::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	input->read((char*)&damageType, sizeof(int));
	input->read((char*)&damageMultiplier, sizeof(float));
	input->read((char*)&damageLethalityLevel, sizeof(int));
}
#pragma endregion

bool Entity_Weapon::Attack(Entity* source, Entity* target)
{
	Entity_Living* liv = dynamic_cast<Entity_Living*>(target);
	if (liv) {

		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.information = source->names[0] + " attacks with the" + names[0];
		ObservationManager::Instance().MakeObservation(o);
		liv->TakeDamage(this, Entity_Living::Piercing, 1, 3);
		return true;
	}
	return false;
}
