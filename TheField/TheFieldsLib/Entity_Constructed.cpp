#include "pch.h"
#include "Entity_Constructed.h"
#include "ObservationManager.h"

#pragma region Serialization
void Entity_Constructed::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);
	writer->Key("broken");
	writer->Bool(broken);
	writer->Key("resistance");
	writer->Int(resistance);
}

void Entity_Constructed::ReadFromJson(Value& v)
{
	Entity::ReadFromJson(v);
	broken = v["broken"].GetBool();
	resistance = v["resistance"].GetInt();
}
void Entity_Constructed::WriteData(std::fstream* output)
{
	Entity::WriteData(output);
	output->write((char*)&broken, sizeof(bool));
	output->write((char*)&resistance, sizeof(int));
}

void Entity_Constructed::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	input->read((char*)&broken, sizeof(bool));
	input->read((char*)&resistance, sizeof(int));
}
#pragma endregion

bool Entity_Constructed::BreakConstructed(int inputStrength)
{
	if (inputStrength > resistance) {
		ObservationManager::Instance().MakeObservation(new Observation_Direct("the " + this->names[0] + " breaks",this));

		visibleInsides = true;
		broken = true;
		std::vector<std::string> toRemove = GetAdjectives(Visual);
		for (auto i : toRemove) {
			RemoveAdjective(i);
		}

		AddAdjective(Position::Visual, "broken");

		std::vector<Entity*> inside = GetInventory(Inside);
		for (auto i : inside) {
			i->SetParent(parent.first, parent.second);
		}
		return true;
	}
	return false;
}

