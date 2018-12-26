#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "Configuration.h"

namespace catalogize {
	/*
		Репозиторий типов файлов. Отвечает за определение категории файла по расширению.
	*/
	class FileTypesRepo
	{
	private:
		std::map<std::wstring, std::wstring> extensionToType;
	public:
		FileTypesRepo() {};
		FileTypesRepo(FileTypesRepo const&) = delete;
		void operator=(FileTypesRepo const&) = delete;
		static FileTypesRepo& instance() {
			static FileTypesRepo m_instance;
			return m_instance;
		};
		// Инициализирует репозиторий типов файлов из файла, указанного в конфигурации.
		int load();
		// Возвращает имя категории по расширению файла
		std::wstring & getCategory(std::wstring & extension);
	};

}
