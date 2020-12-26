#include "pch.h"
#include "Entity_Clip.h"


#pragma region Serialization
void Entity_Clip::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);
	writer->Key("clipType");
	writer->Int((int)clipType);
	writer->Key("bulletsInClip");
	writer->Int(bulletsInClip);
	writer->Key("clipSize");
	writer->Int(clipSize);
}

void Entity_Clip::ReadFromJson(Value& v)
{
	Entity::ReadFromJson(v);
	clipType = (ClipType)v["clipType"].GetInt();
	bulletsInClip = v["bulletsInClip"].GetInt();
	clipSize = v["clipSize"].GetInt();
}

void Entity_Clip::WriteData(std::fstream* output)
{
	Entity::WriteData(output);
	output->write((char*)&clipType, sizeof(int));
	output->write((char*)&bulletsInClip, sizeof(int));
	output->write((char*)&clipSize, sizeof(int));

}

void Entity_Clip::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	input->read((char*)&clipType, sizeof(int));
	input->read((char*)&bulletsInClip, sizeof(int));
	input->read((char*)&clipSize, sizeof(int));
}
#pragma  endregion

int Entity_Clip::FireBullet(int toRemove)
{
	if (toRemove > bulletsInClip) {
		int in = bulletsInClip;
		bulletsInClip = 0;
		return in;
	}
	else {
		bulletsInClip -= toRemove;
		return toRemove;
	}
}

Entity_Clip::ClipType Entity_Clip::GetClipType()
{
	return clipType;
}
