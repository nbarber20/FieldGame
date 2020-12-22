#pragma once
#include "Entity.h"
class Entity_Event : public Entity
{
public:
	Entity_Event() {};
	virtual ~Entity_Event() {
		ObservationTypeSenseConsumptionList.clear();
	};
	virtual int GetClassHash() override {
		return typeid(this).hash_code();
	}
	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
		WriteStringData(EventImageFile, output);
		WriteStringData(EventText, output);

		int listSize = ObservationTypeSenseConsumptionList.size();
		output->write((char*)&listSize, sizeof(int));
		for (int i = 0; i < listSize; i++) {

			output->write((char*)&(ObservationTypeSenseConsumptionList[i].first), sizeof(int));
			output->write((char*)&(ObservationTypeSenseConsumptionList[i].second), sizeof(int));
		}
	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		EventImageFile = ReadStringData(input);
		EventText = ReadStringData(input);
		int listSize;
		input->read((char*)&listSize, sizeof(int));
		for (int i = 0; i < listSize; i++) {
			int f, s;
			input->read((char*)&(f), sizeof(int));
			input->read((char*)&(s), sizeof(int));
			ObservationTypeSenseConsumptionList.push_back(std::make_pair(f, s));
		}
	};

	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer) {
		Entity::WriteToJson(writer);
		writer->Key("EventImageFile");
		writer->String(EventImageFile.c_str(), static_cast<SizeType>(EventImageFile.length()));
		writer->Key("EventText");
		writer->String(EventText.c_str(), static_cast<SizeType>(EventText.length()));
	}

	virtual void ReadFromJson(Value& v) {
		Entity::ReadFromJson(v);
		EventImageFile = v["EventImageFile"].GetString();
		EventText = v["EventText"].GetString();
	}


	void setObservationConsumptionList(std::vector<std::pair<int, int>> newList);
	bool AttemptTrigger();
	std::string EventImageFile;
	std::string EventText;
private:
	std::vector<std::pair<int, int>> ObservationTypeSenseConsumptionList;
};

