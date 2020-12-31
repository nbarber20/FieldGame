 #pragma once
#include <SFML/Graphics.hpp>
#include <windows.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "GameLoader.h"
#include "Entity.h"
#include "TreeLoader.h"
 
using namespace rapidjson;
class WidgetEntity;
class Widget;
class WidgetTextEntry;
class WidgetAddKeyToArray;
class WidgetRemoveKeyFromArray;
class WidgetAddMemberToArray;
class WidgetRemoveMemberFromArray;
class WidgetWorldIDTextEntry;
class LevelEditor
{
public:
	enum EditorView {
		SceneView,
		EntityView,
	};
	LevelEditor() {
		widget150 = new sf::Texture();
		widget150->loadFromFile(GameLoader::Instance().GetDirectory() + "Data/LevelEditor/UI/Widget_150.png");
		widgetPlus = new sf::Texture();
		widgetPlus->loadFromFile(GameLoader::Instance().GetDirectory() + "Data/LevelEditor/UI/WidgetPlus.png");
		widgetMinus = new sf::Texture();
		widgetMinus->loadFromFile(GameLoader::Instance().GetDirectory() + "Data/LevelEditor/UI/WidgetMinus.png");
		widget250 = new sf::Texture();
		widget250->loadFromFile(GameLoader::Instance().GetDirectory() + "Data/LevelEditor/UI/Widget_250.png");
	};
	~LevelEditor() {};

	void SetupNew();
	void SetupSceneView();
	void SetupEntityView(Entity* e);
	void ParseObject(Value::MemberIterator i, int depth = 0);
	void ClickEditor(sf::Vector2f pos);
	void SaveInputData();
	void InputCharacter(char c);
	void RemoveCharacter();
	void DrawEditor(sf::Text* displayTex, sf::Sprite* sprite, sf::RenderWindow* window);
	void AddEntityChildrenToLevelEditor(Entity* e, WidgetEntity* currentParent);
	void AddPrefabToLevelEditor(WidgetEntity* parent);
	void RemovePrefabFromoLevelEditor(WidgetEntity* parent);
	WidgetEntity* AddEntityToLevelEditor(WidgetEntity* parent,Entity* e);

	void LoadBinIntoLevelEditor();
	void LoadJSONIntoLevelEditor();
	void EditWorldIDTextBox(WidgetWorldIDTextEntry* entryBox);
	void EditTextBox(WidgetTextEntry* entryBox);
	void AddElementToJSONArray(WidgetAddKeyToArray* btn);
	void SubElemenFromJSONArray(WidgetRemoveKeyFromArray* btn);



	void SaveEntityViewToPrefab();
	void SaveEntityView();

	std::string Openfilename(HWND owner = nullptr);


	void SaveToBin();
	void SaveToJSON();
	void ConvertDialog();
	void ConvertBehavior();

	void BackToTop()
	{
		scrollOffset = 0;
	}

	std::vector<Widget*> widgets;

	sf::Texture* widget150;
	sf::Texture* widget250;
	sf::Texture* widgetPlus;
	sf::Texture* widgetMinus;

	Entity* editingEntity;
	EditorView view;
	WidgetWorldIDTextEntry* currentTextBoxWorldID;
	WidgetTextEntry* currentTextBox;

	Document entitydocument;
	int scrollOffset = 0;
	TreeLoader treeLoader;
};

