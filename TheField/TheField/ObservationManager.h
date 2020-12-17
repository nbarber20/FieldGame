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
		SENSE_All,
		SENSE_Look,
		SENSE_Feel,
		SENSE_Taste,
		SENSE_Smell,
		SENSE_Hearing,
		SENSE_Physical,
		SENSE_Mental,
	};
	enum ObservationType {
		TYPE_All,
		TYPE_Direct,
		TYPE_Image,
		TYPE_Notice,
		TYPE_Movement,
		TYPE_Rotation,
		TYPE_FacingDirection,
	};

	struct Observation {
		friend bool operator==(const Observation& lhs, const ObservationManager::Observation& rhs) {
			return lhs.sense == rhs.sense &&
				lhs.sense == rhs.sense &&
				lhs.type == rhs.type &&
				lhs.referenceEntity == rhs.referenceEntity &&
				lhs.lastState == rhs.lastState &&
				lhs.information == rhs.information &&
				lhs.displayed == rhs.displayed &&	
				lhs.imageFile == rhs.imageFile;
			 }
		ObservationSense sense;
		ObservationType type;
		Entity* referenceEntity;
		std::string lastState;
		std::string information;
		std::string imageFile;
		bool displayed = false;
	};
	ObservationManager() {};

	void MakeObservation(Observation observation) {
		//Todo check within players sense distance...
		observations.push_back(observation);
	}
	void CompileObservations(Entity* playerEntity, TextDisplay* textDisplay);
	std::vector<Observation> GetObservations() {
		return observations;
	}
	void ClearObservations() {
		observations.clear();
	}
	void ConsumeObservations(std::vector<std::pair<int, int>> toConsume);
	void RemoveObservationForEntity(Entity* entity);

	std::string RotationToString(Rotation r);
	std::string FacingDirectionToString(FacingDirection r);
	std::string PositionToString(Position r, std::string individualName);
	std::string GetComponentName(Position pos);
	std::string getPreposition(Position pos, bool* containsNoun);

private:
	void RemoveObservation(int index);
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

