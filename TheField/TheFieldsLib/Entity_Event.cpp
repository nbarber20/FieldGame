#include "pch.h"
#include "Entity_Event.h"
#include "ObservationManager.h"
#include "World.h"

#pragma region Serialization
void Entity_Event::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);
	writer->Key("EventImageFile");
	writer->String(eventImageFile.c_str());
	writer->Key("EventText");
	writer->String(eventText.c_str());
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
}

void Entity_Event::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	eventImageFile = ReadStringData(input);
	eventText = ReadStringData(input);
}
#pragma endregion

bool Entity_Event::AttemptTrigger()
{
	ObservationManager::Instance().ClearObservations();
	if (eventImageFile != "") {
		ObservationManager::Instance().MakeObservation(new Observation_Image(eventImageFile, this));
	}
	ObservationManager::Instance().MakeObservation(new Observation_Direct(eventText, this));
	World::Instance().RemoveEntity(this);
	return true;
}
