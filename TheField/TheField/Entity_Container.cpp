#include "pch.h"
#include "Entity_Container.h"
#include "ObservationManager.h"

void Entity_Container::Tick()
{
	Entity::Tick();
	if ((this->visibleInsides && this->rotation != Upright) || this->broken == true) {
		std::vector<Entity*> inside = GetInventory(Inside);
		for(auto object : inside){
			Entity_Fluid* fluidCheck = dynamic_cast<Entity_Fluid*>(object);
			if (fluidCheck) {
				fluidCheck->AddAdjective(Visual, "spilled");
			}
			object->SetParent(parent.first, parent.second);
		}
	}
	if (this->permiability > 0) {
		std::vector<Entity*> inside = GetInventory(Inside);
		for (auto object : inside) {
			Entity_Fluid* fluidCheck = dynamic_cast<Entity_Fluid*>(object);
			if (fluidCheck) {
				ObservationManager::Observation o = ObservationManager::Observation();
				o.sense = ObservationManager::SENSE_Look;
				o.type = ObservationManager::TYPE_Direct;
				o.referenceEntity = fluidCheck;
				o.information = "Some " + fluidCheck->names[0] + " drips out of the " + this->names[0];
				ObservationManager::Instance().MakeObservation(o);

				Entity* drippingFluid = fluidCheck->SplitFluid(permiability);
				drippingFluid->AddAdjective(Visual, "spilled");
				drippingFluid->SetParent(parent.first, parent.second);				
				break;
			}
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
	for (auto object : inside) {		
		//If this is inside, ensure there is space or pour as much as possible
		if (targetPositin == Inside) {
			float targetVoidSpace = target->getInternalVoidSPace();
			if (object->size > targetVoidSpace) {
				Entity_Fluid* fluidCheck = dynamic_cast<Entity_Fluid*>(object);
				if (fluidCheck) {
					fluidCheck->RemoveAdjective("spilled");
					fluidCheck->SplitFluid(targetVoidSpace)->SetParent(targetPositin,target);
				}
				continue;
			}
		}	
		Entity_Fluid* fluidCheck = dynamic_cast<Entity_Fluid*>(object);
		if (fluidCheck) {
			fluidCheck->RemoveAdjective("spilled");
		}
		object->SetParent(targetPositin, target);
	}

	if (getInternalVoidSPace() == this->internalVolume) //the pouring entity is now empty
	{
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.referenceEntity = this;
		o.information = "The " + names[0] + " is empty";
		ObservationManager::Instance().MakeObservation(o);
		return true;
	}
	return true;
}
