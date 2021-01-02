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

#pragma region Serialization
void Entity_Living::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);

	writer->Key("homeID");
	writer->Int(homeID);
	writer->Key("homePosition");
	writer->Int((int)homePosition);
	writer->Key("homeWorldID");
	writer->Int((int)homeWorldID);

	writer->Key("spokenLanguage");
	writer->StartArray();
	for (int i = 0; i < spokenLanguage.size(); i++) {
		std::string s = std::to_string((int)spokenLanguage[i]);
		writer->String(s.c_str());
	}
	writer->EndArray();
	writer->Key("readingLanguage");
	writer->StartArray();
	for (int i = 0; i < readingLanguage.size(); i++) {
		std::string s = std::to_string((int)readingLanguage[i]);
		writer->String(s.c_str());
	}
	writer->EndArray();
	writer->Key("strength");
	writer->Double(strength);
	writer->Key("healthStatus");
	writer->Int((int)healthStatus);
	writer->Key("nourishment");
	writer->Double(nourishment);
	writer->Key("hydration");
	writer->Double(hydration);
	writer->Key("maxNourishment");
	writer->Double(maxNourishment);
	writer->Key("maxHydration");
	writer->Double(maxHydration);
	writer->Key("bleedSpeed");
	writer->Double(bleedSpeed);
	writer->Key("bloodLevel");
	writer->Double(bloodLevel);
	writer->Key("maxBloodLevel");
	writer->Double(maxBloodLevel);
	writer->Key("unconsciousCounter");
	writer->Int(unconsciousCounter);
	writer->Key("damageThreshold");
	writer->Double(damageThreshold);
	writer->Key("resistance");
	writer->Double(resistance);
	writer->Key("unconscious");
	writer->Bool(unconscious);
	writer->Key("dead");
	writer->Bool(dead);
	writer->Key("healthThresholds");
	writer->StartArray();
	for (int i = 0; i < healthThresholds.size(); i++) {
		writer->Int(healthThresholds[i]);
	}
	writer->EndArray();


	writer->Key("behaviorTrees");
	writer->StartArray();
	for (int i = 0; i < behaviorTrees.size(); i++) {
		writer->String(behaviorTrees[i]->treeName.c_str());
	}
	writer->EndArray();

	writer->Key("behaviorTreesWaitReturn");
	writer->StartArray();
	for (int i = 0; i < behaviorTrees.size(); i++) {
		writer->Int(behaviorTrees[i]->waitReturnIndex);
	}
	writer->EndArray();

	writer->Key("unarmedDamageType");
	writer->Int(unarmedDamageType);
	writer->Key("unarmedDamageMultiplier");
	writer->Double(unarmedDamageMultiplier);
	writer->Key("unarmedDamageLethalityLevel");
	writer->Int(unarmedDamageLethalityLevel);
	writer->Key("behaviorState");
	writer->Int(behaviorState);

	writer->Key("savedTargets");
	writer->StartArray();
	for (int i = 0; i < savedTargets.size(); i++) {
		writer->String(savedTargets[i].Name.c_str());
	}
	writer->EndArray();


	writer->Key("savedTargetsID");
	writer->StartArray();
	for (int i = 0; i < savedTargets.size(); i++) {
		writer->Int(savedTargets[i].EntityID);
	}
	writer->EndArray();

	writer->Key("savedTargetsWorldID");
	writer->StartArray();
	for (int i = 0; i < savedTargets.size(); i++) {
		writer->Int(savedTargets[i].WorldID);
	}
	writer->EndArray();

}

void Entity_Living::ReadFromJson(Value& v)
{
	Entity::ReadFromJson(v);
	homeID = v["homeID"].GetInt();
	homePosition = (Position)v["homePosition"].GetInt();
	homeWorldID = v["homeWorldID"].GetInt();

	spokenLanguage.clear();
	for (int i = 0; i < v["spokenLanguage"].Size(); i++) {
		spokenLanguage.push_back((Languages)std::stoi(v["spokenLanguage"][i].GetString()));
	}
	readingLanguage.clear();
	for (int i = 0; i < v["readingLanguage"].Size(); i++) {
		readingLanguage.push_back((Languages)std::stoi(v["readingLanguage"][i].GetString()));
	}
	strength = v["strength"].GetDouble();
	healthStatus = (HealthStatus)v["healthStatus"].GetInt();
	nourishment = v["nourishment"].GetDouble();
	hydration = v["hydration"].GetDouble();
	maxNourishment = v["maxNourishment"].GetDouble();
	maxHydration = v["maxHydration"].GetDouble();
	bleedSpeed = v["bleedSpeed"].GetDouble();
	bloodLevel = v["bloodLevel"].GetDouble();
	maxBloodLevel = v["maxBloodLevel"].GetDouble();
	unconsciousCounter = v["unconsciousCounter"].GetInt();
	resistance = v["resistance"].GetDouble();
	unconscious = v["unconscious"].GetBool();
	dead = v["dead"].GetBool();

	healthThresholds.clear();
	for (int i = 0; i < v["healthThresholds"].Size(); i++) {
		healthThresholds.push_back(v["healthThresholds"][i].GetInt());
	}

	behaviorTrees.clear();
	for (int i = 0; i < v["behaviorTrees"].Size(); i++) {
		BehaviorTree* tree = GameLoader::Instance().LoadBehaviorTree(v["behaviorTrees"][i].GetString());
		tree->parentEntity = this;
		behaviorTrees.push_back(tree);
	}
	for (int i = 0; i < v["behaviorTreesWaitReturn"].Size(); i++) {
		behaviorTrees[i]->waitReturnIndex = v["behaviorTreesWaitReturn"][i].GetInt();
	}

	unarmedDamageType = (DamageType)v["unarmedDamageType"].GetInt();
	unarmedDamageMultiplier = v["unarmedDamageMultiplier"].GetDouble();
	unarmedDamageLethalityLevel = v["unarmedDamageLethalityLevel"].GetInt();
	behaviorState = (BehaviorState)v["behaviorState"].GetInt();

	savedTargets.clear();
	for (int i = 0; i < v["savedTargets"].Size(); i++) {
		SetSavedTarget(v["savedTargets"][i].GetString(), nullptr);
	}
	for (int i = 0; i < v["savedTargetsID"].Size(); i++) {
		savedTargets[i].EntityID = v["savedTargetsID"][i].GetInt();
	}
	for (int i = 0; i < v["savedTargetsWorldID"].Size(); i++) {
		savedTargets[i].WorldID = v["savedTargetsWorldID"][i].GetInt();
	}
}

void Entity_Living::WriteData(std::fstream* output)
{
	Entity::WriteData(output);

	int spokenLanguageLen = spokenLanguage.size();
	output->write((char*)&spokenLanguageLen, sizeof(int));
	for (int i = 0; i < spokenLanguageLen; i++) {
		output->write((char*)&(spokenLanguage[i]), sizeof(int));
	}

	int readingLanguageLen = readingLanguage.size();
	output->write((char*)&readingLanguageLen, sizeof(int));
	for (int j = 0; j < readingLanguageLen; j++) {
		output->write((char*)&(readingLanguage[j]), sizeof(int));
	}

	output->write((char*)& homeID, sizeof(int));
	output->write((char*)& homePosition, sizeof(int));
	output->write((char*)& homeWorldID, sizeof(int));

	output->write((char*)&strength, sizeof(float));
	output->write((char*)&healthStatus, sizeof(int));
	output->write((char*)&nourishment, sizeof(float));
	output->write((char*)&hydration, sizeof(float));
	output->write((char*)&maxNourishment, sizeof(float));
	output->write((char*)&maxHydration, sizeof(float));
	output->write((char*)&bleedSpeed, sizeof(float));
	output->write((char*)&bloodLevel, sizeof(float));
	output->write((char*)&maxBloodLevel, sizeof(float));
	output->write((char*)&unconsciousCounter, sizeof(int));
	output->write((char*)&damageThreshold, sizeof(float));
	output->write((char*)&resistance, sizeof(float));
	output->write((char*)&unconscious, sizeof(bool));
	output->write((char*)&dead, sizeof(bool));
	int numTrees = behaviorTrees.size();
	output->write((char*)&numTrees, sizeof(int));
	for (int i = 0; i < numTrees; i++) {
		WriteStringData(behaviorTrees[i]->treeName, output);
		output->write((char*)&behaviorTrees[i]->waitReturnIndex, sizeof(int));
	}


	int numSavedTargets = savedTargets.size();
	output->write((char*)&numSavedTargets, sizeof(int));
	for (int i = 0; i < numSavedTargets; i++) {
		WriteStringData(savedTargets[i].Name, output);
		output->write((char*)&savedTargets[i].EntityID, sizeof(int));
		output->write((char*)&savedTargets[i].WorldID, sizeof(int));
	}
}

void Entity_Living::ReadData(std::fstream* input)
{
	Entity::ReadData(input);

	spokenLanguage.clear();
	int spokenLanguageLen;
	input->read((char*)&spokenLanguageLen, sizeof(int));
	for (int i = 0; i < spokenLanguageLen; i++) {
		int spokenLang;
		input->read((char*)&(spokenLang), sizeof(int));
		spokenLanguage.push_back((Languages)spokenLang);
	}

	readingLanguage.clear();
	int readingLanguageLen;
	input->read((char*)&readingLanguageLen, sizeof(int));
	for (int j = 0; j < readingLanguageLen; j++) {
		int readLang;
		input->read((char*)&readLang, sizeof(int));
		readingLanguage.push_back((Languages)readLang);
	}

	input->read((char*)& homeID, sizeof(int));
	input->read((char*)& homePosition, sizeof(int));
	input->read((char*)& homeWorldID, sizeof(int));

	input->read((char*)&strength, sizeof(float));
	input->read((char*)&healthStatus, sizeof(int));
	input->read((char*)&nourishment, sizeof(float));
	input->read((char*)&hydration, sizeof(float));
	input->read((char*)&maxNourishment, sizeof(float));
	input->read((char*)&maxHydration, sizeof(float));
	input->read((char*)&bleedSpeed, sizeof(float));
	input->read((char*)&bloodLevel, sizeof(float));
	input->read((char*)&maxBloodLevel, sizeof(float));
	input->read((char*)&unconsciousCounter, sizeof(int));
	input->read((char*)&damageThreshold, sizeof(float));
	input->read((char*)&resistance, sizeof(float));
	input->read((char*)&unconscious, sizeof(bool));
	input->read((char*)&dead, sizeof(bool));


	behaviorTrees.clear();
	int numTrees;
	input->read((char*)&numTrees, sizeof(int));
	for (int i = 0; i < numTrees; i++) {
		std::string s = ReadStringData(input);
		BehaviorTree* tree = GameLoader::Instance().LoadBehaviorTree(s);
		tree->parentEntity = this;
		behaviorTrees.push_back(tree);
		int waitReturnIndex;
		input->read((char*)&waitReturnIndex, sizeof(int));
		tree->waitReturnIndex = waitReturnIndex;
	}

	savedTargets.clear();
	int numSavedTargets;
	input->read((char*)&numSavedTargets, sizeof(int));
	for (int i = 0; i < numSavedTargets; i++) {
		SavedTargetVariables v;
		v.Name = ReadStringData(input);
		int VEntityID;
		input->read((char*)&VEntityID, sizeof(int));
		int VWorldID;
		input->read((char*)&VWorldID, sizeof(int));
		v.EntityID = VEntityID;
		v.WorldID = VWorldID;
		savedTargets.push_back(v);
	}
}
#pragma endregion

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
		nourishment -= constants.livingHungerLossSpeed;
		hydration -= constants.livingWaterLossSpeed;
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
			ObservationManager::Instance().MakeObservation(new Observation_Action("bleed", "bleeds", nullptr, this));
			if (bloodLevel < 0) {
				bloodLevel = 0;
			}
		}

		if (bloodLevel < 0.45*maxBloodLevel) {
			if (unconscious == false) {
				unconscious = true; 
				unconsciousCounter = 4;
				ObservationManager::Instance().MakeObservation(new Observation_Action("fall unconscious", "falls unconscious", nullptr, this));
			}
			TakeDamage(this, Bleed, 1, 1);
		}
		if (unconscious == true) {
			unconsciousCounter--;
			if (unconsciousCounter <= 0) {
				ObservationManager::Instance().MakeObservation(new Observation_Action("wake", "wakes", nullptr, this));
				unconsciousCounter = 0;
				unconscious = false;
			}
		}
	}	

	//HYDRATION HUNGER NOTIFIERS

	if (nourishment <= 0) {
		ObservationManager::Instance().MakeObservation(new Observation_Status("starving", this));
	}
	else if (nourishment <= 20) {
		ObservationManager::Instance().MakeObservation(new Observation_Status("very hungry", this));
	}
	else if (nourishment <= 40) {
		ObservationManager::Instance().MakeObservation(new Observation_Status("hungry", this));
	}

	if (hydration <= 0) {
		ObservationManager::Instance().MakeObservation(new Observation_Status("dying of dehydration", this));
	}
	else if (hydration <= 20) {
		ObservationManager::Instance().MakeObservation(new Observation_Status("very thirsty", this));
	}
	else if (hydration <= 40) {
		ObservationManager::Instance().MakeObservation(new Observation_Status("thirsty", this));
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
				ObservationManager::Instance().MakeObservation(new Observation_Action("fall unconscious", "falls unconscious", nullptr, this));
			}
		}


		if (type == Piercing) bleedSpeed = 30.0f;

		healthStatus = (HealthStatus)(offset);
		if (healthStatus == Dead)dead = true;
		ObservationManager::Instance().MakeObservation(new Observation_Status(GetHealthStatusString(healthStatus),this));
	}
	if (source != this) {
		Entity_Living* livingTest = dynamic_cast<Entity_Living*>(source);
		if (livingTest) {
			target = livingTest;
			return;
		}
		else {
			Entity* check = nullptr;
			source->IsChildOf(12, &check); //NPC
			source->IsChildOf(10, &check); //living
			source->IsChildOf(13, &check); //player
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

			ObservationManager::Instance().MakeObservation(new Observation_Action("punch", "punches", livingTarget, this));
			livingTarget->TakeDamage(this,unarmedDamageType, unarmedDamageMultiplier, unarmedDamageLethalityLevel); // punch
		}
		else {
			ObservationManager::Instance().MakeObservation(new Observation_Action("attack", "attacks", livingTarget, this));
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
						ObservationManager::Instance().MakeObservation(new Observation_Direct("There is too much to drink" , e));
						return true;
					}
					fluid->SetParent(Mouth, this,0,false,false);
				}
				else {
					Entity* e = fluid->SplitFluid(constants.mouthSize);
					e->SetParent(Mouth, this, 0, false, false);
				}
			}
		}
		if (fluidCheck.size() == 0) {
			ObservationManager::Instance().MakeObservation(new Observation_Direct("The " + e->names[0] + " is empty", e));
		}
		return true;
	}
	Entity_Fluid* fluid = dynamic_cast<Entity_Fluid*>(e);
	if (fluid) {

		if (drinkAll) {
			if (fluid->size > constants.drinkableWaterThreshold) {
				ObservationManager::Instance().MakeObservation(new Observation_Direct("There is too much to drink", e));
				return true;
			}
			fluid->SetParent(Mouth, this, 0, false, false);
		}
		else {
			Entity* e = fluid->SplitFluid(constants.mouthSize);
			e->SetParent(Mouth, this, 0, false, false);
		}
		return true;
	}
	return false;
}

bool Entity_Living::Eat(Entity* e)
{
	Entity_Food* food = dynamic_cast<Entity_Food*>(e);
	if (food) {
		e->SetParent(Mouth, this,0,false,false);
		return true;
	}
	return false;
}

bool Entity_Living::Graze()
{
	//TODO - a bit hacky, grass isnt the most nutritious 
	ObservationManager::Instance().MakeObservation(new Observation_Action("eat some grass.. wait what?", "eats the grass", nullptr, this));
	this->AddHydration(100);
	this->AddNourishment(100);
	return true;
}

bool Entity_Living::TrySwallow(Entity* e)
{

	Entity_Fluid* fluid = dynamic_cast<Entity_Fluid*>(e);
	if (fluid) {
		if (fluid->GetSwallowable()) {
			ObservationManager::Instance().MakeObservation(new Observation_Action("drink", "drinks", e, this));
			this->AddHydration(fluid->GetHydration());
			World::Instance().MarkRemoveEntity(fluid);
			return true;
		}
	}
	Entity_Food* food = dynamic_cast<Entity_Food*>(e);
	if (food) {
		if (food->GetSpoiled() == false) {
			ObservationManager::Instance().MakeObservation(new Observation_Action("eat", "eats", e, this));
			this->AddNourishment(food->GetNutritionalValue());
			World::Instance().MarkRemoveEntity(food);
			return true;
		}
	}


	ObservationManager::Instance().MakeObservation(new Observation_Action("spit out", "spits out", e, this));
	e->SetParent(parent.first, parent.second);
	return false;
}

void Entity_Living::SetSavedTarget(std::string newTargetName, Entity* newTarget)
{
	SavedTargetVariables t;
	t.Name = newTargetName;
	if (newTarget != nullptr) {
		t.EntityID = newTarget->uniqueEntityID;
		t.WorldID = newTarget->worldID;
	}
	savedTargets.push_back(t);
}

void Entity_Living::SetSavedTarget(SavedTargetVariables newTarget)
{
	savedTargets.push_back(newTarget);
}

bool Entity_Living::GetSavedTarget(std::string newTargetName)
{
	for (int i = 0; i < savedTargets.size(); i++) {
		if (savedTargets[i].Name == newTargetName) {
			target = World::Instance().GetEntityByID(savedTargets[i].EntityID, savedTargets[i].WorldID);
			if (target != nullptr) {
				return true;
			}
		}
	}
	return false;
}
