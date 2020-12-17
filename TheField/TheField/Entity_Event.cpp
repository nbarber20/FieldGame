#include "pch.h"
#include "Entity_Event.h"
#include "ObservationManager.h"
#include "World.h"

void Entity_Event::setObservationConsumptionList(std::vector<std::pair<int, int>> newList)
{
	ObservationTypeSenseConsumptionList = newList;
}

bool Entity_Event::AttemptTrigger()
{
	ObservationManager::Instance().ConsumeObservations(ObservationTypeSenseConsumptionList);

	if (EventImageFile != "") {
		ObservationManager::Observation img = ObservationManager::Observation();
		img.sense = ObservationManager::SENSE_All;
		img.type = ObservationManager::TYPE_Image;
		img.imageFile = EventImageFile;
		ObservationManager::Instance().MakeObservation(img);
	}


	ObservationManager::Observation o = ObservationManager::Observation();
	o.sense = ObservationManager::SENSE_All;
	o.type = ObservationManager::TYPE_Direct;
	o.information = EventText;
	ObservationManager::Instance().MakeObservation(o);

	World::Instance().RemoveEntity(this);

	return true;
}
