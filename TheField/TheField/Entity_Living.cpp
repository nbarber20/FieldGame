#include "pch.h"
#include "Entity_Living.h"
#include "ObservationManager.h"
void Entity_Living::Tick()
{
	Entity::Tick();
	nourishment -= 0.5f;
	hydration--;
	if (nourishment <= 0) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You are starving.";
		ObservationManager::Instance().MakeObservation(o);
		TakeDamage(Hunger, 1.0f, 1);
	}
	else if (nourishment <= 20) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You really need to eat.";
		ObservationManager::Instance().MakeObservation(o);
		TakeDamage(Hunger, 0.35f, 1);
	}
	else if (nourishment <= 40) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You could eat something.";
		ObservationManager::Instance().MakeObservation(o);
	}

	if (hydration <= 0) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You are dying of dehydration.";
		ObservationManager::Instance().MakeObservation(o);
		TakeDamage(Thirst, 1.0f, 1);
	}
	else if (hydration <= 20) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You really need to drink something.";
		ObservationManager::Instance().MakeObservation(o);
		TakeDamage(Thirst, 0.35f, 1);
	}
	else if (hydration <= 40) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = "You are feeling thirsty.";
		ObservationManager::Instance().MakeObservation(o);
	}
}

void Entity_Living::TakeDamage(DamageType type, float multiplier, int lethalityLevel) {
	//TODO resistances

	if (multiplier > healthThresholds[(int)healthStatus]) {
		int offset = (int)healthStatus + lethalityLevel;
		if (offset > 6)offset = 6;
		healthStatus = (HealthStatus)(offset);
	}
}

void Entity_Living::AddNourishment(float delta)
{
	nourishment += delta;
	if (nourishment > maxNourishment)nourishment = maxNourishment;
}

void Entity_Living::AddHydration(float delta)
{
	hydration += delta;
	if (hydration > maxHydration)hydration = maxHydration;
}
