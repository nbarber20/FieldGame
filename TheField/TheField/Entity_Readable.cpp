#include "pch.h"
#include "Entity_Readable.h"
#include "ObservationManager.h"
#include "Entity_Human.h"
void Entity_Readable::Read(Entity_Human* humanRef)
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
