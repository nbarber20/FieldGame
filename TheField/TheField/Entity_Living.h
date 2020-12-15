#pragma once
#include "Entity.h"
class Entity_Living : public Entity
{
public:
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

	Entity_Living(bool visibleInsides, float internalVolume, float size) :Entity(visibleInsides, internalVolume, size) {};
	virtual ~Entity_Living() {};
	std::vector<Languages> spokenLanguage;
	std::vector<Languages> readingLanguage;
	virtual void Tick() override;
	
	void AddNourishment(float delta);
	void AddHydration(float delta);
	void TakeDamage(DamageType type, float multiplier, int lethalityLevel);

	HealthStatus healthStatus = Healthy;
	float nourishment = 50;
	float hydration = 50;
	float maxNourishment = 50;
	float maxHydration = 50;

	std::vector<float> healthThresholds = { 0.35f,0.5f,1.0f,0.5f,0.5f,0.3f};
};

