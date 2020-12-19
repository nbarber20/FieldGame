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
		typeID = "Entity_Living";
		worldActive = true;
	};
	Entity_Living(int id, bool visibleInsides, float internalVolume, float size) :Entity(id, visibleInsides, internalVolume, size) {
		typeID = "Entity_Living";
		worldActive = true;
	};
	virtual ~Entity_Living() {};

	virtual void WriteData(std::fstream* output) {
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
	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		int spokenLanguageLen;
		input->read((char*)&spokenLanguageLen, sizeof(int));
		for (int i = 0; i < spokenLanguageLen; i++) {
			int spokenLang;
			input->read((char*)&(spokenLang), sizeof(int));
			spokenLanguage.push_back((Languages)spokenLang);
		}

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
	};


	virtual void Tick() override;

	void SetHome(Position p, Entity* home);
	void ReturnHome();
	
	void AddNourishment(float delta);
	void AddHydration(float delta);
	void TakeDamage(DamageType type, float multiplier, int lethalityLevel);
	std::string GetHealthStatusString(HealthStatus s);

	int homeID = -2;
	Position homePosition = Inside;
	int homeWorldID = -2;

	std::vector<Languages> spokenLanguage;
	std::vector<Languages> readingLanguage;
	HealthStatus healthStatus = Healthy;
	float nourishment = 50;
	float hydration = 50;
	float maxNourishment = 50;
	float maxHydration = 50;
	float bleedSpeed = 0.0f;
	float bloodLevel = 192.0f;
	float maxBloodLevel = 192.0f;
	int unconsciousCounter = 0;

	float damageThreshold = 0.0f;
	float resistance = 0.1;

	bool unconscious = false;
	bool dead = false;

	std::vector<float> healthThresholds = { 0.35f,0.5f,1.0f,0.5f,0.5f,0.3f, 0.0f };
};

