#include <iostream>
#include <io.h>
#include <fcntl.h>
#include "FileSystem.h"
#include "FileTypesRepo.h"

using namespace catalogize;

FileTypesRepo ftr;
wstring copyRoot = L"C:\\Users\\Анастасия\\Documents\\3 семестр\\Collected\\";


void processFile(std::wstring path, std::wstring name) {
	// Если папка, которую вернул walk, является подпапкой copyRoot, ничего не делаем во избежание поедания диска
	if (!path.find(copyRoot)) { return; }
	
	const wchar_t * ext = FileSystem::getExtension(&name);
	if (ext == nullptr) {
		return;
	}
	// Точка не нужна, можем смело пропускать
	wstring extension(++ext);
	// Получаем категорию
	wstring* category = ftr.getCategory(&extension);
	// Если категория пуста, пропускаем файл
	if (category->empty()) return;

	wstring categoryFolder = copyRoot + (copyRoot.back() != L'\\' ? L"\\" : L"") + *category + L'\\' + extension;

	// Проверяем, существует ли папка для этой категории...
	bool exists = FileSystem::exists(&categoryFolder);
	bool folder = FileSystem::isFolder(&categoryFolder);
	if (exists && !folder) { 
		wcout << L" В пути указан файл ";
		return;
	}
	// ...и если не существует, создаем
	else if (!exists) {
		FileSystem::createFolder(&categoryFolder);
	}

	// Копируем файл!
	FileSystem::copyFile(&path, &categoryFolder, &name);
	wcout << name << ": " << *category << endl;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	wstring fromRoot;
	//обработка исключений createFolder
	do {
		wcout << L" Введите директорию, куда каталогизировать файлы ";
		getline(wcin, copyRoot);
	} while (!FileSystem::createFolder(&copyRoot));

	do {
		wcout << L" Введите директорию, которую нужно каталогизировать ";
		getline(wcin, fromRoot);
	} while (!FileSystem::exists(&fromRoot) || !FileSystem::isFolder(&fromRoot));
	FileSystem::walk(fromRoot, &processFile);
	wcout << L" Каталогизация завершилась " << endl;
}