#pragma once
#include <string>
class DialogTree;
class TreeLoader
{
public:
	TreeLoader() {};
	void ConvertBehaviorTree(std::string filepathInput);
	DialogTree* ConvertDialogTree(std::string filepathInput);
};

