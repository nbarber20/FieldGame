#include "pch.h"
#include "Entity_Container.h"
#include "ObservationManager.h"

void Entity_Container::Tick()
{
	Entity::Tick();
	if (this->rotation != Upright || this->broken == true) {
		std::vector<Entity*> inside = GetInventory(Inside);
		for (int i = 0; i < inside.size(); i++) {

			Entity_Fluid* fluidCheck = dynamic_cast<Entity_Fluid*>(inside[i]);
			if (fluidCheck) {
				fluidCheck->AddAdjective(Visual, "spilled");
			}

			inside[i]->SetParent(parent.first, parent.second);
		}
	}
}

bool Entity_Container::PourInto(Entity* target)
{
	if (target == this) {
		return false;
	}

	Position targetPositin = On;

	if (target->internalVolume > 0) {
		targetPositin = Inside;
	}
	std::vector<Entity*> inside = GetInventory(Inside);
	for (int i = 0; i < inside.size(); i++) {
		Entity_Fluid* fluidCheck = dynamic_cast<Entity_Fluid*>(inside[i]);
		if (fluidCheck) {
			fluidCheck->RemoveAdjective("spilled");
		}
		inside[i]->SetParent(targetPositin, target);
	}

	if (inside.size() == 0)
	{
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.referenceEntity = this;
		o.information = "The " + names[0] + " is empty";
		ObservationManager::Instance().MakeObservation(o);
	}
	return true;
}
