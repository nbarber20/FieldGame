#pragma once
#include "Entity.h"
#include "BehaviorTree.h"
#include "GameLoader.h"

class Entity_Living : public Entity
{
public:
	struct SavedTargetVariables {
		std::string Name;
		int EntityID;
		int WorldID;
	};
	enum BehaviorState
	{
		Idle, //Do nothing of note
		Defensive, //Attack target once
		Enraged, //Attack target until death
	};
	enum HealthStatus
	{
		Healthy,
		Pained,
		Woudned,
		Gashed,
		Critical,
		Dying,
		Dead,
	};
	enum DamageType
	{
		Bleed,
		Blunt,
		Slash,
		Piercing,
		Disease,
		Hunger,
		Thirst,
		Radiation,
		Explosive,
		Burn,
		Cold,
	};
	Entity_Living() {
		serializationID = 10;
		worldActive = false;
	};
	virtual ~Entity_Living() {
		for (int i = 0; i < behaviorTrees.size(); i++) {
			delete behaviorTrees[i];
		}
		behaviorTrees.clear();
	};
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	void Tick() override;
	void AddBehavior(BehaviorTree* tree);
	void SetHome(Position p, Entity* home);
	void ReturnHome();
	void AddNourishment(float delta);
	void AddHydration(float delta);
	virtual void TakeDamage(Entity* source, DamageType type, float multiplier, int lethalityLevel);
	void AttackTarget(bool sourceWeapon);
	std::string GetHealthStatusString(HealthStatus s);
	virtual bool Drink(Entity* e, bool drinkAll);
	virtual bool Eat(Entity* e);
	bool Graze();
	virtual bool TrySwallow(Entity* e);
	void SetSavedTarget(SavedTargetVariables newTarget);
	void SetSavedTarget(std::string newTargetName, Entity* newTarget);
	bool GetSavedTarget(std::string newTargetName);

	//TODO organize

	int homeID = -2;
	Position homePosition = Inside;
	int homeWorldID = -2;

	std::vector<Languages> spokenLanguage;
	std::vector<Languages> readingLanguage;

	float strength = 65.f;

	HealthStatus healthStatus = Healthy;
	float nourishment = 150;
	float hydration = 150;
	float maxNourishment = 150;
	float maxHydration = 150;
	float bleedSpeed = 0.0f;
	float bloodLevel = 192.0f;
	float maxBloodLevel = 192.0f;
	int unconsciousCounter = 0;

	float damageThreshold = 0.0f;
	float resistance = 0.1f;

	bool unconscious = false;
	bool dead = false;
	std::vector<BehaviorTree*> behaviorTrees;

	std::vector<float> healthThresholds = { 0.35f,0.5f,0.7f,0.5f,0.5f,0.3f, 0.0f };

	DamageType unarmedDamageType = Blunt;
	float unarmedDamageMultiplier = .8;
	int unarmedDamageLethalityLevel = 1;
	BehaviorState behaviorState = Idle;
	std::vector<SavedTargetVariables> savedTargets;
};

