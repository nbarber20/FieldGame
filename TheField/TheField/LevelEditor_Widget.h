#pragma once
#include <string>
#include "Entity.h"
#include "LevelEditor.h"

class LevelEditor;
class Widget {
public:
	Widget(std::string name, sf::Vector2i screenOffset, sf::Texture* texture){
		this->screenOffset = screenOffset;
		this->texture = texture;
		this->name = name;
	}
	virtual ~Widget() {
		delete texture;
	}
	virtual void OnClick(LevelEditor* editor) {

	}
	sf::Vector2i screenOffset;
	sf::Texture* texture;
	int buttonWidth = 250;
	std::string name;
};


class WidgetEntity : public Widget{
public:
	WidgetEntity(Entity* entity, std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset,texture) {
		this->entity = entity;
		buttonWidth = 250;
	}

	virtual void OnClick(LevelEditor* editor) {
		editor->SetupEntityView(entity);
	}
	Entity* entity;
};
class WidgetNewBtn : public Widget {
public:
	WidgetNewBtn( std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		buttonWidth = 150;
	}

	virtual void OnClick(LevelEditor* editor) {
		editor->SetupNew();
	}
};
class WidgetAddPrefabBtn : public Widget {
public:
	WidgetAddPrefabBtn(WidgetEntity* parent, std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		this->parent = parent;
		buttonWidth = 45;
	}

	virtual void OnClick(LevelEditor* editor) {
		editor->AddPrefabToLevelEditor(parent);
	}
	WidgetEntity* parent;
};
class WidgetRemovePrefabBtn : public Widget {
public:
	WidgetRemovePrefabBtn(WidgetEntity* parent, std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		this->parent = parent;
		buttonWidth = 45;
	}

	virtual void OnClick(LevelEditor* editor) {
		editor->RemovePrefabFromoLevelEditor(parent);
	}
	WidgetEntity* parent;
};


class WidgetLoadBIN : public Widget {
public:
	WidgetLoadBIN(std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		buttonWidth = 150;
	}
	virtual void OnClick(LevelEditor* editor) {
		editor->LoadBinIntoLevelEditor();
	}
};

class WidgetLoadJSON : public Widget {
public:
	WidgetLoadJSON(std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		buttonWidth = 150;
	}
	virtual void OnClick(LevelEditor* editor) {
		editor->LoadJSONIntoLevelEditor();
	}
};

class WidgetSaveBIN : public Widget {
public:
	WidgetSaveBIN(std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		buttonWidth = 150;
	}
	virtual void OnClick(LevelEditor* editor) {
		editor->SaveToBin();
	}
};

class WidgetSaveJSON : public Widget {
public:
	WidgetSaveJSON(std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		buttonWidth = 150;
	}
	virtual void OnClick(LevelEditor* editor) {
		editor->SaveToJSON();
	}
};

class WidgetSaveEntitySettings : public Widget {
public:
	WidgetSaveEntitySettings(std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		buttonWidth = 150;
	}
	virtual void OnClick(LevelEditor* editor) {
		editor->SaveEntityView();
	}
};

class WidgetSaveEntityToPrefab : public Widget {
public:
	WidgetSaveEntityToPrefab(std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		buttonWidth = 150;
	}
	virtual void OnClick(LevelEditor* editor) {
		editor->SaveEntityViewToPrefab();
	}
};

class WidgetBackToTop : public Widget {
public:
	WidgetBackToTop(std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		buttonWidth = 150;
	}
	virtual void OnClick(LevelEditor* editor) {
		editor->BackToTop();
	}
};


class WidgetWorldIDTextEntry : public Widget {
public:
	WidgetWorldIDTextEntry(std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		buttonWidth = 250;
	}
	virtual void OnClick(LevelEditor* editor) {
		editor->EditWorldIDTextBox(this);
	}
	virtual void InputData(char i) {
		name += i;
	}
};

class WidgetTextEntry : public Widget {
public:
	enum dataType {
		STRING,
		INT,
		DOUBLE,
		BOOL,
		ARRAYSTRING,
		ARRAYINT,
		ARRAYDOUBLE,
		ARRAYBOOL,
		ARRAYARRAYSTRING
	};
	WidgetTextEntry(Value::MemberIterator member,std::string key, int index, dataType d, std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		this->d = d;
		this->member = member;
		this->key = key;
		this->index = index;
		this->c = name.c_str();
		buttonWidth = 250;
	}
	virtual ~WidgetTextEntry() {
		delete c;
	}
	virtual void OnClick(LevelEditor* editor) {
		editor->EditTextBox(this);
	}
	virtual void InputData(char i) {
		name += i;
		c = name.c_str();
	}
	virtual int GetInt() {
		int v = 0;
		try {
			v = std::stoi(name);
		}
		catch (...) {

		}
		return v;
	}

	virtual double GetDouble() {
		double v = 0;
		try {
			v = std::stod(name);
		}
		catch (...) {

		}
		return v;
	}

	virtual bool GetBool() {
		if (name == "true")return true;
		return false;
	}
	dataType d;
	const char* c;
	std::string key;
	int index;
	Value::MemberIterator member;
};

class WidgetAddKeyToArray : public Widget {
public:
	WidgetAddKeyToArray(Value::MemberIterator member, std::string key, int index, WidgetTextEntry::dataType d, std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		buttonWidth = 45;
		this->d = d;
		this->index = index;
		this->key = key;
		this->member = member;
	}

	virtual void OnClick(LevelEditor* editor) {
		editor->AddElementToJSONArray(this);
	}
	std::string key;
	int index;
	WidgetTextEntry::dataType d;
	Value::MemberIterator member;
};
class WidgetRemoveKeyFromArray : public Widget {
public:
	WidgetRemoveKeyFromArray(Value::MemberIterator member, std::string key, int index, WidgetTextEntry::dataType d, std::string name, sf::Vector2i screenOffset, sf::Texture* texture) :Widget(name, screenOffset, texture) {
		buttonWidth = 45; 
		this->d = d;
		this->key = key;
		this->index = index;
		this->member = member;
	}

	virtual void OnClick(LevelEditor* editor) {
		editor->SubElemenFromJSONArray(this);
	}
	std::string key;
	int index;
	WidgetTextEntry::dataType d;
	Value::MemberIterator member;
};
