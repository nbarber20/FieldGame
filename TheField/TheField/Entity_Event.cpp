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

	ObservationManager::Observation o = ObservationManager::Observation();
	o.sense = ObservationManager::SENSE_All;
	o.type = ObservationManager::TYPE_Direct;
	o.referenceEntity = nullptr;
	o.information = EventText;
	ObservationManager::Instance().MakeObservation(o);

	World::Instance().RemoveEntity(this);

	return true;
}
