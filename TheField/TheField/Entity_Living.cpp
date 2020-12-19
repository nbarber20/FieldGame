#include "pch.h"
#include "Entity_Living.h"
#include "ObservationManager.h"
#include "World.h"
void Entity_Living::Tick()
{
	Entity::Tick();

	if (homeID == -2) { //No home, set it here
		SetHome(this->parent.first, this->parent.second);
	}

	if (dead == false) {
		nourishment -= 0.5f;
		hydration--;
		damageThreshold -= resistance;
		if (damageThreshold <= 0) damageThreshold = 0;

		if (nourishment <= 0) {
			TakeDamage(Hunger, 1.0f, 1);
		}
		else if (nourishment <= 20) {
			TakeDamage(Hunger, 0.35f, 1);
		}
		if (hydration <= 0) {
			TakeDamage(Thirst, 1.0f, 1);
		}
		else if (hydration <= 20) {
			TakeDamage(Thirst, 0.35f, 1);
		}


		if (bleedSpeed == 0) {
			bloodLevel += 0.1f;
			if (bloodLevel > maxBloodLevel) {
				bloodLevel = maxBloodLevel;
			}
		}
		else {
			bloodLevel -= bleedSpeed;
			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Physical;
			o.type = ObservationManager::TYPE_Direct;
			o.information = names[0] + " bleeds";
			ObservationManager::Instance().MakeObservation(o);


			if (bloodLevel < 0) {
				bloodLevel = 0;
			}
		}

		if (bloodLevel < 0.45*maxBloodLevel) {
			if (unconscious == false) {
				unconscious = true; 
				unconsciousCounter = 4;
				ObservationManager::Observation o = ObservationManager::Observation();
				o.sense = ObservationManager::SENSE_Physical;
				o.type = ObservationManager::TYPE_Direct;
				o.information = names[0] + " falls unconscious";
				ObservationManager::Instance().MakeObservation(o);
			}
			TakeDamage(Bleed, 1, 1);
		}
		if (unconscious == true) {
			unconsciousCounter--;
			if (unconsciousCounter <= 0) {
				ObservationManager::Observation o = ObservationManager::Observation();
				o.sense = ObservationManager::SENSE_Physical;
				o.type = ObservationManager::TYPE_Direct;
				o.information = names[0] + " wakes";
				ObservationManager::Instance().MakeObservation(o);
				unconsciousCounter = 0;
				unconscious = false;
			}
		}
	}	
}

void Entity_Living::SetHome(Position p, Entity* home)
{
	if (home == nullptr) {
		homeID = -1;
		homeWorldID = this->worldID;
		homePosition = p;
	}
	else {
		homeID = home->uniqueEntityID;
		homeWorldID = home->worldID;
		homePosition = p;
	}
}

void Entity_Living::ReturnHome()
{
	Entity* homeEntity = World::Instance().GetEntityByID(homeID, homeWorldID);
	SetParent(homePosition,homeEntity);
}

void Entity_Living::TakeDamage(DamageType type, float multiplier, int lethalityLevel) {
	//TODO resistances
	damageThreshold += multiplier;
	if (damageThreshold > healthThresholds[(int)healthStatus]) {


		int offset = (int)healthStatus + lethalityLevel;
		if (offset > 6)offset = 6;
		if (offset > 3 && type == Blunt) {
			bleedSpeed = 10.0f;
			if (unconscious == false) {
				unconscious = true;
				unconsciousCounter = 3;
				ObservationManager::Observation o = ObservationManager::Observation();
				o.sense = ObservationManager::SENSE_Physical;
				o.type = ObservationManager::TYPE_Direct;
				o.information = names[0] + " falls unconscious";
				ObservationManager::Instance().MakeObservation(o);
			}
		}


		if (type == Piercing) bleedSpeed = 30.0f;

		healthStatus = (HealthStatus)(offset);
		if (healthStatus == Dead)dead = true;

		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Physical;
		o.type = ObservationManager::TYPE_Direct;
		o.information = names[0] + " is " + GetHealthStatusString(healthStatus);
		ObservationManager::Instance().MakeObservation(o);
	}
}

std::string Entity_Living::GetHealthStatusString(HealthStatus s)
{
	switch (s)
	{
	case Entity_Living::Healthy:
		return "Healthy";
	case Entity_Living::Pained:
		return "Pained";
	case Entity_Living::Woudned:
		return "Woudned";
	case Entity_Living::Gashed:
		return "Brutally wounded";
	case Entity_Living::Critical:
		return "Critical";
	case Entity_Living::Dying:
		return "Dying";
	case Entity_Living::Dead:
		return "Dead";
	default:
		return "Healthy";
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
