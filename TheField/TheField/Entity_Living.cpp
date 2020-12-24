#include "pch.h"
#include "Entity_Living.h"
#include "Entity_Weapon.h"
#include "Entity_Npc.h"
#include "Entity_Player.h"
#include "Entity_Food.h"
#include "Entity_Fluid.h"
#include "Entity_Container.h"
#include "ObservationManager.h"
#include "World.h"
void Entity_Living::Tick()
{
	Entity::Tick();

	for(int i =0;i<behaviorTrees.size();i++){
		if (behaviorTrees[i]->Tick() == true)
		{
			//tree complete
			behaviorTrees.erase(std::remove(behaviorTrees.begin(), behaviorTrees.end(), behaviorTrees[i]), behaviorTrees.end());
		}
	}
	if (homeID == -2) { //No home, set it here
		SetHome(this->parent.first, this->parent.second);
	}

	std::vector<Entity*> inMouth = GetInventory(Mouth);
	for (auto object : inMouth)
	{
		TrySwallow(object);
	}

	if (dead == false) {
		nourishment -= 0.5f;
		hydration--;
		damageThreshold -= resistance;
		if (damageThreshold <= 0) damageThreshold = 0;

		if (nourishment <= 0) {
			TakeDamage(this,Hunger, 1.0f, 1);
		}
		else if (nourishment <= 20) {
			TakeDamage(this, Hunger, 0.35f, 1);
		}
		if (hydration <= 0) {
			TakeDamage(this, Thirst, 1.0f, 1);
		}
		else if (hydration <= 20) {
			TakeDamage(this, Thirst, 0.35f, 1);
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
			TakeDamage(this, Bleed, 1, 1);
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

void Entity_Living::AddBehavior(BehaviorTree* tree)
{
	tree->parentEntity = this;
	behaviorTrees.push_back(tree);
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

void Entity_Living::TakeDamage(Entity* source, DamageType type, float multiplier, int lethalityLevel) {
	//TODO resistances
	damageThreshold += multiplier;
	behaviorState = Defensive;
	if (damageThreshold > healthThresholds[(int)healthStatus]) {

		behaviorState = Enraged;

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
	if (source != this) {
		Entity_Living* livingTest = dynamic_cast<Entity_Living*>(source);
		if (livingTest) {
			target = livingTest;
			return;
		}
		else {
			Entity* check = nullptr;
			source->IsChildOf(typeid(Entity_Npc*).hash_code(), &check);
			source->IsChildOf(typeid(Entity_Living*).hash_code(), &check);
			source->IsChildOf(typeid(Entity_Player*).hash_code(), &check);
			if (check != nullptr) {
				target = check;
				return;
			}
		}
	}
}

void Entity_Living::AttackTarget(bool sourceWeapon)
{
	if (target == nullptr)return;
	Entity_Living* livingTarget = dynamic_cast<Entity_Living*>(target);
	if (livingTarget) {
		if (sourceWeapon) {
			std::vector<Entity*> items = GetInventory();
			for (int i = 0; i < items.size(); i++) {
				Entity_Weapon* weapon = dynamic_cast<Entity_Weapon*>(items[i]);
				if (weapon) {
					if (weapon->Attack(this,livingTarget)) {

						if (livingTarget->dead) {
							behaviorState = Idle;
							target = nullptr;
						}
						return;
					}
				}
			}

			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Look;
			o.type = ObservationManager::TYPE_Direct;
			o.information = names[0] + " punches" + target->names[0];
			ObservationManager::Instance().MakeObservation(o);
			livingTarget->TakeDamage(this,unarmedDamageType, unarmedDamageMultiplier, unarmedDamageLethalityLevel); // punch
		}
		else {

			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Look;
			o.type = ObservationManager::TYPE_Direct;
			o.information = names[0] + " attacks" + target->names[0];
			ObservationManager::Instance().MakeObservation(o);
			livingTarget->TakeDamage(this, unarmedDamageType, unarmedDamageMultiplier, unarmedDamageLethalityLevel);// punch
		}

		if (livingTarget->dead) {
			behaviorState = Idle;
			target = nullptr;
		}
	}
	if (behaviorState == Defensive) {
		target = nullptr;
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
bool Entity_Living::Drink(Entity* e, bool drinkAll)
{
	Entity_Container* container = dynamic_cast<Entity_Container*>(e);
	if (container) {
		std::vector<Entity*> fluidCheck = container->GetInventory(Position::Inside);
		for (int i = 0; i < fluidCheck.size(); i++) {
			Entity_Fluid* fluid = dynamic_cast<Entity_Fluid*>(fluidCheck[i]);
			if (fluid) {
				if (drinkAll) {
					if (fluid->size > constants.drinkableWaterThreshold) {
						ObservationManager::Observation o = ObservationManager::Observation();
						o.sense = ObservationManager::SENSE_Look;
						o.type = ObservationManager::TYPE_Direct;
						o.referenceEntity = e;
						o.information = "There is too much to drink";
						ObservationManager::Instance().MakeObservation(o);
						return true;
					}
					fluid->SetParent(Mouth, this);
				}
				else {
					Entity* e = fluid->SplitFluid(constants.mouthSize);
					e->SetParent(Mouth, this);
				}
			}
		}
		if (fluidCheck.size() == 0) {
			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Look;
			o.type = ObservationManager::TYPE_Direct;
			o.referenceEntity = e;
			o.information = "The " + e->names[0] + " is empty";
			ObservationManager::Instance().MakeObservation(o);
		}
		return true;
	}
	Entity_Fluid* fluid = dynamic_cast<Entity_Fluid*>(e);
	if (fluid) {

		if (drinkAll) {
			if (fluid->size > constants.drinkableWaterThreshold) {
				ObservationManager::Observation o = ObservationManager::Observation();
				o.sense = ObservationManager::SENSE_Look;
				o.type = ObservationManager::TYPE_Direct;
				o.referenceEntity = e;
				o.information = "There is too much to drink";
				ObservationManager::Instance().MakeObservation(o);
				return true;
			}
			fluid->SetParent(Mouth, this);
		}
		else {
			Entity* e = fluid->SplitFluid(constants.mouthSize);
			e->SetParent(Mouth, this);
		}
		return true;
	}
	return false;
}

bool Entity_Living::Eat(Entity* e)
{
	Entity_Food* food = dynamic_cast<Entity_Food*>(e);
	if (food) {
		e->SetParent(Mouth, this);
		return true;
	}
	return false;
}

bool Entity_Living::TrySwallow(Entity* e)
{

	Entity_Fluid* fluid = dynamic_cast<Entity_Fluid*>(e);
	if (fluid) {
		if (fluid->swallowable) {
			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Taste;
			o.type = ObservationManager::TYPE_Direct;
			o.information = "You drink the " + e->GetRandomAdjective(Taste) + " " + e->names[0];
			ObservationManager::Instance().MakeObservation(o);
			this->AddHydration(fluid->hydration);
			World::Instance().RemoveEntity(fluid);
			return true;
		}
	}
	Entity_Food* food = dynamic_cast<Entity_Food*>(e);
	if (food) {
		if (food->spoiled == false) {
			ObservationManager::Observation o = ObservationManager::Observation();
			o.sense = ObservationManager::SENSE_Taste;
			o.type = ObservationManager::TYPE_Direct;
			o.information = "You eat the " + e->GetRandomAdjective(Taste) + " " + e->names[0];
			ObservationManager::Instance().MakeObservation(o);
			this->AddNourishment(food->nutritionalValue);
			World::Instance().RemoveEntity(food);
			return true;
		}
	}



	ObservationManager::Observation o = ObservationManager::Observation();
	o.sense = ObservationManager::SENSE_Look;
	o.type = ObservationManager::TYPE_Direct;
	o.referenceEntity = e;
	o.information = "You spit out the" + e->names[0];
	ObservationManager::Instance().MakeObservation(o);

	e->SetParent(parent.first, parent.second);
	return false;
}
