#include "pch.h"
#include "LevelEditor.h"
#include "World.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "Entity.h"
#include <vector>
#include <cstdio>

#pragma warning(disable : 4996)
using namespace rapidjson;

void LevelEditor::LoadFromJSON()
{
	FILE* fp = fopen("Data/LevelEditor/test.json", "rb"); // non-Windows use "r"

	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	Document document;
	document.ParseStream(is);
	fclose(fp);

	const Value& entities = document["entities"];
	assert(entities.IsArray());
	for (int i = 0; i < entities.Size();i++) {
		std::string b = document["entities"][i]["typeID"].GetString();
		Entity* e = World::Instance().genEntity(b);
		e->ReadFromJson(document["entities"][i]);
		World::Instance().AddEntity(e);
	}
	World::Instance().setupParents();
}

void LevelEditor::LoadPlayerFromJSON()
{
	FILE* fp = fopen("Data/LevelEditor/Player.json", "rb"); // non-Windows use "r"

	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	Document document;
	document.ParseStream(is);
	fclose(fp);
	const Value& entities = document["entities"];
	assert(entities.IsArray());
	for (int i = 0; i < entities.Size(); i++) {
		std::string b = document["entities"][i]["typeID"].GetString();
		Entity* e = World::Instance().genEntity(b);
		e->ReadFromJson(document["entities"][i]);
		World::Instance().AddEntity(e);
	}
	World::Instance().setupParents();
}

void LevelEditor::SaveToJSON()
{
	std::vector<Entity*> entities = World::Instance().GetEntities();

	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("entities");
	writer.StartArray();
	for (int i = 0; i < entities.size(); i++) {
		writer.StartObject();
		entities[i]->WriteToJson(&writer);
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();

	std::fstream file("Data/LevelEditor/test.json", std::ios::out);
	file << sb.GetString();
	file.close();
}

void LevelEditor::SavePlayerToJSON()
{
	std::vector<Entity*> entities = World::Instance().GetEntities();

	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("entities");
	writer.StartArray();
	

	for (int i = 0; i < entities.size(); i++) {
		Entity_Player* p = dynamic_cast<Entity_Player*>(entities[i]);
		if (p) {
			entities[i]->worldID = World::Instance().currentPlayerTile;
			writer.StartObject();
			entities[i]->WriteToJson(&writer);
			writer.EndObject();
		}
		else if (entities[i]->IsChildOf(World::Instance().playerEntity) == true) {
			entities[i]->worldID = -1;
			writer.StartObject();
			entities[i]->WriteToJson(&writer);
			writer.EndObject();
		}
	}
	writer.EndArray();
	writer.EndObject();

	std::fstream file("Data/LevelEditor/Player.json", std::ios::out);
	file << sb.GetString();
	file.close();
}

void LevelEditor::SaveToFile(bool savePlayer)
{
	if (savePlayer) {
		World::Instance().SavePlayer();
	}
	World::Instance().Tick();
	for (int i = 0; i < World::Instance().loadedTiles[i]; i++) {
		World::Instance().SaveTile(World::Instance().loadedTiles[i]);
	}
}
