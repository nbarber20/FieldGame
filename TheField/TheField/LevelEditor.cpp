#include "pch.h"
#include "LevelEditor.h"
#include "World.h"
#include "GameLoader.h"
#include "Entity.h"
#include "Entity_GroundTile.h"
#include "LevelEditor_Widget.h"
#include <vector>
#include <cstdio>
#include <windows.h>
#include <string.h>
#include <iostream>
#include <SFML/Graphics.hpp>


#pragma warning(disable : 4996)

void LevelEditor::SetupNew()
{

	widgets.clear();
	World::Instance().ClearEntities();
	Entity_GroundTile* Ground = new Entity_GroundTile();
	Ground->SetEntityData(GameLoader::Instance().GetUniqueID(), false, 0.0f, 60000.0f, 0.0f);
	Ground->attachedToParent = true;
	Ground->names = { "dirt","ground","grass" };
	Ground->worldPos = 0;
	World::Instance().AddEntity(Ground);
	SetupSceneView();
}

void LevelEditor::SetupSceneView()
{
	scrollOffset = 0;
	widgets.clear();
	widgets.push_back(new WidgetNewBtn("New", sf::Vector2i(650, 0), widget150));
	widgets.push_back(new WidgetLoadBIN("Load", sf::Vector2i(650, 0), widget150));
	widgets.push_back(new WidgetSaveBIN("Save", sf::Vector2i(650, 0), widget150));
	widgets.push_back(new WidgetLoadJSON("LoadJSON", sf::Vector2i(650, 0), widget150));
	widgets.push_back(new WidgetSaveJSON("SaveJSON", sf::Vector2i(650, 0), widget150));
	widgets.push_back(new WidgetWorldIDTextEntry(std::to_string(GameLoader::Instance().loadedTiles[0]), sf::Vector2i(0, 0), widget150));
	AddEntityChildrenToLevelEditor(World::Instance().GetEntities()[0],nullptr);
	widgets.push_back(new WidgetBackToTop("BackToTop", sf::Vector2i(650, 0), widget150));
	view = SceneView;
}

void LevelEditor::SetupEntityView(Entity* e)
{
	scrollOffset = 0;
	editingEntity = e;
	entitydocument = Document();


	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("hash");
	writer.Int(e->serializationID);
	e->worldID = GameLoader::Instance().loadedTiles[0];
	e->WriteToJson(&writer);
	writer.EndObject();

	std::string s = sb.GetString();
	entitydocument.Parse(s.c_str());


	widgets.clear();
	widgets.push_back(new WidgetSaveEntityToPrefab("SavePrefab", sf::Vector2i(650, 0), widget150));
	widgets.push_back(new WidgetSaveEntitySettings("Save", sf::Vector2i(650, 0), widget150));
	for (auto i = entitydocument.MemberBegin(); i != entitydocument.MemberEnd(); ++i)
	{
		ParseObject(i);
	}
	widgets.push_back(new WidgetBackToTop("BackToTop", sf::Vector2i(650, 0), widget150));

	view = EntityView;
}

void LevelEditor::ParseObject(Value::MemberIterator i, int depth)
{
	if (i->name == "hash" || i->name == "parentEntityID" || i->name == "uniqueEntityID" || i->name == "behaviorTreesWaitReturn") {
		return;
	}
	widgets.push_back(new Widget(i->name.GetString(), sf::Vector2i(depth, 0), widget250));
	if (i->value.IsArray()) {
		for (int j = 0; j < i->value.Size(); j++) {
			if (i->value.GetArray()[j].IsString()) {
				widgets.push_back(new WidgetTextEntry(i, i->name.GetString(), j, WidgetTextEntry::ARRAYSTRING, i->value.GetArray()[j].GetString(), sf::Vector2i(depth+150, 0), widget250));
			}
			else if (i->value.GetArray()[j].IsBool()) {
				std::string boolString = i->value.GetBool() == true ? "true" : "false";
				widgets.push_back(new WidgetTextEntry(i, i->name.GetString(), j, WidgetTextEntry::ARRAYBOOL, boolString, sf::Vector2i(depth+150, 0), widget250));
			}
			else if (i->value.GetArray()[j].IsInt()) {
				widgets.push_back(new WidgetTextEntry(i, i->name.GetString(), j, WidgetTextEntry::ARRAYINT, std::to_string(i->value.GetArray()[j].GetInt()), sf::Vector2i(depth+150, 0), widget250));
			}
			else if (i->value.GetArray()[j].IsDouble()) {
				widgets.push_back(new WidgetTextEntry(i,i->name.GetString(), j, WidgetTextEntry::ARRAYDOUBLE, std::to_string(i->value.GetArray()[j].GetDouble()), sf::Vector2i(depth+150, 0), widget250));
			}
		}
		if (i->name != "savedTargetsID" && i->name != "savedTargetsWorldID" && i->name!= "behaviorsSecondPerson" && i->name != "behaviorssThirdPerson" && i->name != "behaviorssThirdPerson") {
			widgets.push_back(new WidgetAddKeyToArray(i, i->name.GetString(), 0, WidgetTextEntry::ARRAYSTRING, "", sf::Vector2i(depth + 150, 0), widgetPlus));
			if (i->value.GetArray().Size() != 0) {
				widgets.push_back(new WidgetRemoveKeyFromArray(i, i->name.GetString(), 0, WidgetTextEntry::ARRAYSTRING, "", sf::Vector2i(depth + 150, 0), widgetMinus));
			}
		}		
	}
	else {
		if (i->value.IsString()) {
			widgets.push_back(new WidgetTextEntry(i, i->name.GetString(), 0, WidgetTextEntry::STRING, i->value.GetString(), sf::Vector2i(depth + 150, 0), widget250));
		}
		else if (i->value.IsBool()) {
			std::string boolString = i->value.GetBool() == true ? "true" : "false";
			widgets.push_back(new WidgetTextEntry(i,i->name.GetString(),0, WidgetTextEntry::BOOL, boolString, sf::Vector2i(depth + 150, 0), widget250));
		}
		else if (i->value.IsInt()) {
			widgets.push_back(new WidgetTextEntry(i, i->name.GetString(), 0, WidgetTextEntry::INT, std::to_string(i->value.GetInt()), sf::Vector2i(depth + 150, 0), widget250));
		}
		else if (i->value.IsDouble()) {
			widgets.push_back(new WidgetTextEntry(i, i->name.GetString(), 0, WidgetTextEntry::DOUBLE, std::to_string(i->value.GetDouble()), sf::Vector2i(depth + 150, 0), widget250));
		}
	}
}

void LevelEditor::ClickEditor(sf::Vector2f pos)
{
	int ypos = 0;
	for (int i = 0; i < widgets.size(); i++) {
		if (pos.x > widgets[i]->screenOffset.x && pos.x < widgets[i]->screenOffset.x + widgets[i]->buttonWidth && pos.y > ypos + scrollOffset && pos.y < ypos + scrollOffset + 48) {
			widgets[i]->OnClick(this);
			return;
		}
		ypos += 48;
	}
}

void LevelEditor::SaveInputData()
{
	if (currentTextBoxWorldID != nullptr && view == SceneView) {
		GameLoader::Instance().loadedTiles = { std::stoi(currentTextBoxWorldID->name) };
	}
	if (currentTextBox != nullptr && view == EntityView) {
		Value::MemberIterator itr = currentTextBox->member;

		if (currentTextBox->d == WidgetTextEntry::STRING) {
			if (currentTextBox->name.length() <= 0) {
				currentTextBox->name = "";
				currentTextBox->c = "";
			}
			itr->value.SetString(currentTextBox->c, static_cast<SizeType>(currentTextBox->name.length()));
			currentTextBox->name = itr->value.GetString();
		}
		else if (currentTextBox->d == WidgetTextEntry::BOOL) {
			itr->value.SetBool(currentTextBox->GetBool());
			std::string boolString = currentTextBox->GetBool() == true ? "true" : "false";
			currentTextBox->name = boolString;
		}
		else if (currentTextBox->d == WidgetTextEntry::INT) {
			itr->value.SetInt(currentTextBox->GetInt());
			currentTextBox->name = std::to_string(itr->value.GetInt());
		}
		else if (currentTextBox->d == WidgetTextEntry::DOUBLE) {
			itr->value.SetDouble(currentTextBox->GetDouble());
			currentTextBox->name = std::to_string(itr->value.GetDouble());
		}
		else if (currentTextBox->d == WidgetTextEntry::ARRAYSTRING) {
			itr->value.GetArray()[currentTextBox->index].SetString(currentTextBox->c, static_cast<SizeType>(currentTextBox->name.length()));
			currentTextBox->name = itr->value.GetArray()[currentTextBox->index].GetString();
		}
		else if (currentTextBox->d == WidgetTextEntry::ARRAYBOOL) {
			itr->value.GetArray()[currentTextBox->index].SetBool(currentTextBox->GetBool());
			std::string boolString = currentTextBox->GetBool() == true ? "true" : "false";
			currentTextBox->name = boolString;
		}
		else if (currentTextBox->d == WidgetTextEntry::ARRAYDOUBLE) {
			itr->value.GetArray()[currentTextBox->index].SetInt(currentTextBox->GetInt());
			currentTextBox->name = std::to_string(itr->value.GetArray()[currentTextBox->index].GetInt());
		}
		else if (currentTextBox->d == WidgetTextEntry::ARRAYINT) {
			itr->value.GetArray()[currentTextBox->index].SetInt(currentTextBox->GetInt());
			currentTextBox->name = std::to_string(itr->value.GetArray()[currentTextBox->index].GetInt());
		}
		else if (currentTextBox->d == WidgetTextEntry::ARRAYARRAYSTRING) {
			itr->value.GetArray()[currentTextBox->index].SetInt(currentTextBox->GetInt());
			currentTextBox->name = std::to_string(itr->value.GetArray()[currentTextBox->index].GetInt());
		}
	}
	currentTextBox = nullptr;
}

void LevelEditor::InputCharacter(char c)
{
	if (currentTextBox != nullptr && view == EntityView) {
		currentTextBox->InputData(c);
	}
	if (currentTextBoxWorldID != nullptr && view == SceneView) {
		currentTextBoxWorldID->InputData(c);
	}
}

void LevelEditor::RemoveCharacter()
{
	if (currentTextBox != nullptr && view == EntityView) {
		currentTextBox->name = currentTextBox->name.substr(0, currentTextBox->name.size() - 1);
	}
	if (currentTextBoxWorldID != nullptr && view == SceneView) {
		currentTextBoxWorldID->name = currentTextBoxWorldID->name.substr(0, currentTextBoxWorldID->name.size() - 1);
	}
}

void LevelEditor::DrawEditor(sf::Text* displayTex, sf::Sprite* sprite, sf::RenderWindow* window)
{
	int ypos = 0;
	for (int i = 0; i < widgets.size(); i++) {
		sprite->setTexture(*widgets[i]->texture);
		sprite->setPosition(widgets[i]->screenOffset.x, ypos + scrollOffset);
		sprite->setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(widgets[i]->texture->getSize().x, widgets[i]->texture->getSize().y)));
		sprite->setScale(sf::Vector2f(1, 1));
		displayTex->setPosition(widgets[i]->screenOffset.x+6, ypos + scrollOffset +12);
		displayTex->setString(widgets[i]->name);
		window->draw(*sprite);
		window->draw(*displayTex);
		ypos+=48;
	}
}

void LevelEditor::AddEntityChildrenToLevelEditor(Entity* e, WidgetEntity* currentParent)
{

	WidgetEntity* newChild = AddEntityToLevelEditor(currentParent, e);
	std::vector<Entity*> entityChildren = e->GetInventory();
	for (int i = 0; i < entityChildren.size(); i++) {
		AddEntityChildrenToLevelEditor(entityChildren[i], newChild);
	}
}

void LevelEditor::AddPrefabToLevelEditor(WidgetEntity* parent)
{
	std::string fileName = Openfilename();
	std::replace(fileName.begin(), fileName.end(), '\\', '/');
	Entity* toAdd = GameLoader::Instance().SpawnPrefab(fileName, Inside, parent->entity, true);
	if (toAdd != nullptr) {
		AddEntityToLevelEditor(parent,toAdd);
	}
}

void LevelEditor::RemovePrefabFromoLevelEditor(WidgetEntity* parent)
{
	World::Instance().RemoveEntity(parent->entity);
	SetupSceneView();
}

WidgetEntity* LevelEditor::AddEntityToLevelEditor(WidgetEntity* parent, Entity* e)
{

	std::string name = "Unnamed";
	if (e->names.size() > 0)name = e->names[0];
	if (parent == nullptr) {
		WidgetEntity* newEntity = new WidgetEntity(e, name, sf::Vector2i(0, 0), widget250);
		widgets.push_back(newEntity);
		widgets.push_back(new WidgetAddPrefabBtn(newEntity, "", sf::Vector2i(0, 0), widgetPlus));
		return newEntity;
	}
	else
	{
		auto it = std::find(widgets.begin(), widgets.end(), parent);
		WidgetEntity* newEntity = new WidgetEntity(e, name, sf::Vector2i(parent->screenOffset.x + 48, 0), widget250);
		auto it2 = widgets.insert(it + 1, newEntity);
		auto it3 = widgets.insert(it2 + 1, new WidgetRemovePrefabBtn(newEntity, "", sf::Vector2i(parent->screenOffset.x + 48, 0), widgetMinus));
		widgets.insert(it3 + 1, new WidgetAddPrefabBtn(newEntity, "", sf::Vector2i(parent->screenOffset.x + 48, 0), widgetPlus));
		return newEntity;
	}
}

void LevelEditor::LoadBinIntoLevelEditor()
{
	std::string toLoad = Openfilename();
	World::Instance().ClearEntities();
	GameLoader::Instance().LoadTile(toLoad, "");
	if (World::Instance().GetEntities().size() > 0) {
		GameLoader::Instance().loadedTiles = { World::Instance().GetEntities()[0]->worldID };
		World::Instance().SetupParents();
		SetupSceneView();
	}
}

void LevelEditor::LoadJSONIntoLevelEditor()
{
	World::Instance().ClearEntities();
	std::string s = Openfilename();
	FILE* fp = fopen(s.c_str(), "rb"); // non-Windows use "r"

	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	Document document;
	if (document.ParseStream(is).HasParseError()==false) {
		if (document.HasMember("entities")) {
			const Value& entities = document["entities"];
			if (entities.IsArray()) {
				for (int i = 0; i < entities.Size(); i++) {
					int b = document["entities"][i]["hash"].GetInt();
					Entity* e = GameLoader::Instance().GenEntity(b);
					e->ReadFromJson(document["entities"][i]);
					World::Instance().AddEntity(e);
				}
				GameLoader::Instance().loadedTiles = { World::Instance().GetEntities()[0]->worldID };
				World::Instance().SetupParents();
				SetupSceneView();
			}
		}
	}
	fclose(fp);
}

void LevelEditor::EditWorldIDTextBox(WidgetWorldIDTextEntry* entryBox)
{
	currentTextBoxWorldID = entryBox;
}

void LevelEditor::EditTextBox(WidgetTextEntry* entryBox)
{
	SaveInputData();
	currentTextBox = entryBox;
}

void LevelEditor::AddElementToJSONArray(WidgetAddKeyToArray* btn)
{
	Value::MemberIterator itr = btn->member;
	Document::AllocatorType& a = entitydocument.GetAllocator();
	itr->value.GetArray().PushBack(Value("0_EMPTY"), a); //TODO fix
	SaveEntityView();
	SetupEntityView(editingEntity);
}

void LevelEditor::SubElemenFromJSONArray(WidgetRemoveKeyFromArray* btn)
{
	Value::MemberIterator itr = btn->member;
	itr->value.GetArray().Erase(itr->value.GetArray().End()-1);
	SaveEntityView();
	SetupEntityView(editingEntity);
}


void LevelEditor::SaveEntityViewToPrefab()
{
	SaveInputData();
	GameLoader::Instance().SavePrefabToPath(editingEntity, Openfilename());
	SetupSceneView();
}

void LevelEditor::SaveEntityView()
{
	SaveInputData();
	editingEntity->ReadFromJson(entitydocument);
	World::Instance().SetupParents();
	SetupSceneView();
}

std::string LevelEditor::Openfilename(HWND owner) {
	OPENFILENAMEA ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrFile = fileName;
	std::string fileNameStr;

	if (GetOpenFileNameA(&ofn))
		fileNameStr = fileName;

	return fileNameStr;
}

void LevelEditor::SaveToBin()
{
	GameLoader::Instance().SaveTile(Openfilename(), GameLoader::Instance().loadedTiles[0]);
}
/*

void LevelEditor::LoadFromJSON()
{
	std::string s = Openfilename();
	FILE* fp = fopen(s.c_str(), "rb"); // non-Windows use "r"

	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	Document document;
	document.ParseStream(is);
	fclose(fp);

	const Value& entities = document["entities"];
	assert(entities.IsArray());
	for (int i = 0; i < entities.Size();i++) {
		int b = document["entities"][i]["hash"].GetInt();
		Entity* e = GameLoader::Instance().GenEntity(b);
		e->ReadFromJson(document["entities"][i]);
		World::Instance().AddEntity(e);
	}
	World::Instance().SetupParents();
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
		int b = document["entities"][i]["hash"].GetInt();
		Entity* e = GameLoader::Instance().GenEntity(b);
		e->ReadFromJson(document["entities"][i]);
		World::Instance().AddEntity(e);
	}
	World::Instance().SetupParents();
}*/

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

		writer.Key("hash");
		writer.Int(entities[i]->serializationID);

		entities[i]->WriteToJson(&writer);
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();


	std::string s = Openfilename();
	std::fstream file(s.c_str(), std::ios::out);
	file << sb.GetString();
	file.close();
}
/*
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
			entities[i]->worldID = GameLoader::Instance().currentPlayerTile;

			writer.Key("hash");
			writer.Int(entities[i]->serializationID);
			writer.StartObject();
			entities[i]->WriteToJson(&writer);
			writer.EndObject();
		}
		else if (entities[i]->IsChildOf(World::Instance().playerEntity) == true) {
			entities[i]->worldID = -1;

			writer.Key("hash");
			writer.Int(entities[i]->serializationID);
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
		GameLoader::Instance().SavePlayer();
	}
	World::Instance().Tick();
	GameLoader::Instance().UnloadTiles();
}
*/