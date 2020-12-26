#include "pch.h"
#include "Entity_Readable.h"
#include "ObservationManager.h"
#include "Entity_Living.h"

#pragma region Serialization
void Entity_Readable::WriteData(std::fstream* output)
{
	Entity::WriteData(output);
	output->write((char*)&requiredLanguage, sizeof(int));
	WriteStringData(text, output);
}

void Entity_Readable::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	input->read((char*)&requiredLanguage, sizeof(int));
	text = ReadStringData(input);
}
#pragma endregion

void Entity_Readable::Read(Entity_Living* humanRef)
{
	if (std::find(humanRef->readingLanguage.begin(), humanRef->readingLanguage.end(), this->requiredLanguage) != humanRef->readingLanguage.end()) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.referenceEntity = this;
		o.information = text;
		ObservationManager::Instance().MakeObservation(o);
		return;
	}
	ObservationManager::Observation o = ObservationManager::Observation();
	o.sense = ObservationManager::SENSE_Look;
	o.type = ObservationManager::TYPE_Direct;
	o.referenceEntity = this;
	o.information = "You are not familiar with these symbols";
	ObservationManager::Instance().MakeObservation(o);
}
