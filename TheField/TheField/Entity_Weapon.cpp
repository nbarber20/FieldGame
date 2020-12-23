#include "pch.h"
#include "Entity_Weapon.h"
#include "ObservationManager.h"

bool Entity_Weapon::Attack(Entity* source, Entity* target)
{
	Entity_Living* liv = dynamic_cast<Entity_Living*>(target);
	if (liv) {

		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Direct;
		o.information = source->names[0] + " attacks with the" + names[0];
		ObservationManager::Instance().MakeObservation(o);
		liv->TakeDamage(this, Entity_Living::Piercing, 1, 3);
		return true;
	}
	return false;
}
