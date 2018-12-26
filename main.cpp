#include <iostream>
#include <io.h>
#include <fcntl.h>
#include "FileSystem.h"
#include "FileTypesRepo.h"
#include "Configuration.h"
#include "ArgParser.h"

using namespace catalogize;

void processFile(std::wstring path, std::wstring name) {
	Configuration &conf = Configuration::instance();
	wstring copyRoot = conf.to();
	// Если папка, которую вернул walk, является подпапкой copyRoot, ничего не делаем во избежание поедания диска
	if (!path.find(copyRoot)) { return; }

	const wchar_t * ext = FileSystem::getExtension(name);
	if (ext == nullptr) {
		return;
	}
	// Точка не нужна, можем смело пропускать
	wstring extension(++ext);
	// Получаем категорию
	wstring category = FileTypesRepo::instance().getCategory(extension);
	// Если категория пуста, пропускаем файл
	if (category.empty()) return;

	wstring categoryFolder = copyRoot + (copyRoot.back() != L'\\' ? L"\\" : L"") + category + L'\\' + extension + L'\\';

	// Проверяем, существует ли папка для этой категории...
	bool exists = FileSystem::exists(categoryFolder);
	bool folder = FileSystem::isFolder(categoryFolder);
	if (exists && !folder) {
		wcout << L" В пути указан файл ";
		return;
	}
	// ...и если не существует, создаем
	else if (!exists) {
		FileSystem::createFolder(categoryFolder);
	}


	// Копируем файл!
	FileSystem::copyFile(path, categoryFolder, name);
	wcout << name << ": " << category << endl;
}

int wmain(int argc, wchar_t** argv)
{
	setlocale(LC_ALL, "Russian");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	/*
	//обработка исключений createFolder
	do {
		wcout << L" Введите директорию, куда каталогизировать файлы ";
		getline(wcin, copyRoot);
	} while (!FileSystem::createFolder(copyRoot));

	do {
		wcout << L" Введите директорию, которую нужно каталогизировать ";
		getline(wcin, fromRoot);
	} while (!FileSystem::exists(fromRoot) || !FileSystem::isFolder(fromRoot));*/

	ArgParser p;
	bool parsed = p.parse(argc, argv);
	if (!parsed) {
		wcout << L"Usage: " << argv[0] << L" <from> <to> [/repo <repo>]" << endl;
		return 1;
	}
	Configuration &conf = Configuration::instance();
	ConfigurationState state = conf.verify();
	switch (state) {
	case CONFIG_FROM_MISSING:
		wcout << L"<from> is missing or does not exists" << endl;
		return 1;
	case CONFIG_TO_MISSING:
		wcout << L"<to> is missing or couldn't be created" << endl;
		return 1;
	case CONFIG_REPO_MISSING:
		wcout << L"Repo file \"" << conf.repo() << L"\" not found" << endl;
		return 1;
	}

	int loaded = FileTypesRepo::instance().load();
	wcout << L"Loaded " << loaded << " extensions from " << conf.repo() << endl;

	FileSystem::walk(conf.from(), &processFile);
	return 0;
}
