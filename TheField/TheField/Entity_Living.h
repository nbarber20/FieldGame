#pragma once
#include "Entity.h"
#include "BehaviorTree.h"
#include "GameLoader.h"

class Entity_Living : public Entity
{
public:
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
		worldActive = false;
	};
	virtual ~Entity_Living() {
		for (int i = 0; i < behaviorTrees.size(); i++) {
			delete behaviorTrees[i];
		}
		behaviorTrees.clear();
	};
	virtual int GetClassHash() override {
		return typeid(this).hash_code();
	}

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

		int numTrees;
		input->read((char*)&numTrees, sizeof(int));
		for (int i = 0; i < numTrees; i++) {
			std::string s = ReadStringData(input);
			BehaviorTree* tree = GameLoader::Instance().LoadBehaviorTree(s);
			behaviorTrees.push_back(tree);
			int waitReturnIndex;
			input->read((char*)&waitReturnIndex, sizeof(int));
			tree->waitReturnIndex = waitReturnIndex;
		}
	};

	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer) {
		Entity::WriteToJson(writer);

		writer->Key("homeID");
		writer->Int(homeID);
		writer->Key("homePosition");
		writer->Int((int)homePosition);
		writer->Key("homeWorldID");
		writer->Int((int)homeWorldID);
		//TODO spoken
		//TODO written
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
	}

	virtual void ReadFromJson(Value& v) {
		Entity::ReadFromJson(v);
		homeID = v["homeID"].GetInt();
		homePosition = (Position)v["homePosition"].GetInt();
		homeWorldID = v["homeWorldID"].GetInt();
		//TODO spoken
		//TODO written
		strength = v["strength"].GetDouble();
		healthStatus = (HealthStatus)v["strength"].GetInt();
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
	}

	virtual void Tick() override;
	void AddBehavior(BehaviorTree* tree);
	void SetHome(Position p, Entity* home);
	void ReturnHome();	
	void AddNourishment(float delta);
	void AddHydration(float delta);
	virtual void TakeDamage(Entity* source, DamageType type, float multiplier, int lethalityLevel);
	void AttackTarget(bool sourceWeapon);
	std::string GetHealthStatusString(HealthStatus s);

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
	Entity* target;
	BehaviorState behaviorState = Idle;
};

