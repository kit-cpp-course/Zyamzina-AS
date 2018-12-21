#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace catalogize {
	class FileTypesRepo
	{
	private:
		std::map<std::wstring, std::wstring> extensionToType;
	public:
		FileTypesRepo();
		std::wstring* getCategory(std::wstring* extension);
		~FileTypesRepo();
	};

}