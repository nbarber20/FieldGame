#pragma once
#include "Constants.h"
#include "Observation.h"
#include <vector>

class ObservationManager
{
public:
	static ObservationManager& Instance()
	{
		static ObservationManager INSTANCE;
		return INSTANCE;
	}
	ObservationManager() {};

	void MakeObservation(Observation* observation) {
		//Todo check within players sense distance...
		observations.push_back(observation);
	}
	void CompileObservations(Entity* playerEntity, TextDisplay* textDisplay);
	std::vector<Observation*> GetObservations() {
		return observations;
	}
	void ClearObservations() {
		observations.clear();
	}
	void RemoveObservationForEntity(Entity* entity);
	bool StartsWithVowel(std::string input);

	int RandomRange(int start, int end);
	std::string GetNameAndParticle(Entity* e);
	std::string GetIndefNameAndParticle(Entity* e);
	std::string RotationToString(Rotation r);
	std::string FacingDirectionToString(FacingDirection r);
	std::string getPreposition(Position pos, bool* containsNoun, bool isPlayer);

private:
	void RemoveObservation(int index);
	std::vector<Observation*> observations;
	std::string numberStrings[10] = {
		"one",
		"two",
		"thee",
		"four",
		"five",
		"six",
		"seven",
		"eight",
		"nine",
		"ten",
	};
};

