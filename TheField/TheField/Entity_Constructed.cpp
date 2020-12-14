#include "pch.h"
#include "Entity_Constructed.h"
#include "ObservationManager.h"


bool Entity_Constructed::BreakConstructed(int inputStrength)
{
	if (inputStrength > resisitance) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.referenceEntity = this;
		o.information = "the " + this->names[0] + " breaks";
		ObservationManager::Instance().MakeObservation(o);
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

