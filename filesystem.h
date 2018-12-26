#pragma once
#include <string>
#include <vector>
#include "windows.h"
using namespace std;
namespace catalogize {
	/* Указатель на метод, вызываемый методом walk
	   Первый аргумент — путь к файлу
	   Второй аргумент — имя файла */
	typedef void(*WalkFuncPtr)(wstring, wstring);

	/*
		Класс для работы с файловой системой.
		Работает с помощью WinAPI, поддержка Linux-систем была заявлена менеджерами,
		но не была сделана программистом — вместо этого программу рекомендуется запускать под Wine.
	*/
	class FileSystem {
		/* Метод, подготавливающий строку к использованию
		   в качестве пути для вызова функций */
		static const wchar_t* prepare(wstring path);
	public:
		// Проверка на существование файла
		static bool exists(wstring& path);
		/* Проверка на то, является ли объект по указанному пути папкой.
		   Для некорректных/несуществующих путей правильность ответа не гарантируется
		   (сначала используйте метод exists). */
		static bool isFolder(wstring& path);
		/* Создание каталога с путем path.
		   Создает промежуточные каталоги в случае, если их не существует. */
		static bool createFolder(wstring& path);
		/* Копирование файла name из папки from в папку to.
		   В случае, если файл с таким именем уже существует в папке to,
		   добавляет суффикс \([0-9]+\) к имени файла */
		static bool copyFile(wstring& from, wstring& to, wstring& name);
		/* Рекурсивный обход каталога path и всех его подкаталогов
		   с вызовом функции func для всех имен файлов, встреченных в каталоге */
		static void walk(wstring& path, WalkFuncPtr func);
		// Получение расширения из имени файла
		static const wchar_t* getExtension(wstring& name);
	};
}



