#include "pch.h"
#include "Entity_Event.h"
#include "ObservationManager.h"
#include "World.h"

#pragma region Serialization
void Entity_Event::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);
	writer->Key("EventImageFile");
	writer->String(eventImageFile.c_str(), static_cast<SizeType>(eventImageFile.length()));
	writer->Key("EventText");
	writer->String(eventText.c_str(), static_cast<SizeType>(eventText.length()));
}

void Entity_Event::ReadFromJson(Value& v)
{
	Entity::ReadFromJson(v);
	eventImageFile = v["EventImageFile"].GetString();
	eventText = v["EventText"].GetString();
}

void Entity_Event::WriteData(std::fstream* output)
{
	Entity::WriteData(output);
	WriteStringData(eventImageFile, output);
	WriteStringData(eventText, output);
	int listSize = observationTypeSenseConsumptionList.size();
	output->write((char*)&listSize, sizeof(int));
	for (int i = 0; i < listSize; i++) {

		output->write((char*)&(observationTypeSenseConsumptionList[i].first), sizeof(int));
		output->write((char*)&(observationTypeSenseConsumptionList[i].second), sizeof(int));
	}
}

void Entity_Event::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	eventImageFile = ReadStringData(input);
	eventText = ReadStringData(input);
	int listSize;
	input->read((char*)&listSize, sizeof(int));
	for (int i = 0; i < listSize; i++) {
		int f, s;
		input->read((char*)&(f), sizeof(int));
		input->read((char*)&(s), sizeof(int));
		observationTypeSenseConsumptionList.push_back(std::make_pair(f, s));
	}
}
#pragma endregion

void Entity_Event::SetObservationConsumptionList(std::vector<std::pair<int, int>> newList)
{
	observationTypeSenseConsumptionList = newList;
}

bool Entity_Event::AttemptTrigger()
{
	ObservationManager::Instance().ConsumeObservations(observationTypeSenseConsumptionList);
	if (eventImageFile != "") {
		ObservationManager::Observation img = ObservationManager::Observation();
		img.sense = ObservationManager::SENSE_All;
		img.type = ObservationManager::TYPE_Image;
		img.imageFile = eventImageFile;
		ObservationManager::Instance().MakeObservation(img);
	}
	ObservationManager::Observation o = ObservationManager::Observation();
	o.sense = ObservationManager::SENSE_All;
	o.type = ObservationManager::TYPE_Direct;
	o.information = eventText;
	ObservationManager::Instance().MakeObservation(o);
	World::Instance().RemoveEntity(this);
	return true;
}
