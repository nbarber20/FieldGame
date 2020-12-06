#pragma once
#include <string>
#include <vector>
#include "Entity.h"
#include "Constants.h"

class ObservationManager
{
public:
	static ObservationManager& Instance()
	{
		static ObservationManager INSTANCE;
		return INSTANCE;
	}
	enum ObservationSense {
		SENSE_Look,
		SENSE_Feel,
		SENSE_Taste,
		SENSE_Smell,
		SENSE_Hearing,
		SENSE_Physical,
		SENSE_Mental,
	};
	enum ObservationType {
		TYPE_Direct,
		TYPE_Notice,
		TYPE_Movement,
		TYPE_Rotation,
		TYPE_FacingDirection,
	};

	struct Observation {
		ObservationSense sense;
		ObservationType type;
		Entity* referenceEntity;
		std::string lastState;
		std::string information;
		bool displayed = false;
	};
	ObservationManager() {};

	void MakeObservation(Observation observation) {
		//Todo check within players sense distance...
		observations.push_back(observation);
	}
	std::vector <TextDisplay::Log> CompileObservations(Entity* playerEntity);
	std::vector<Observation> GetObservations() {
		return observations;
	}
	void ClearObservations() {
		observations.clear();
	}

	std::string RotationToString(Rotation r);
	std::string FacingDirectionToString(FacingDirection r);
	std::string PositionToString(Position r, std::string individualName);
	std::string GetComponentName(Position pos);
	std::string getPreposition(Position pos, bool* containsNoun);

private:
	std::vector<Observation> observations;
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

