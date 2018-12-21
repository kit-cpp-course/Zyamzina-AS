#include "FileSystem.h"
#include <iostream>
#define NO_ERROR -1

namespace catalogize {
	/* подготовка пути для работы программы */
	// path передается по значению для того, чтобы можно было вызывать prepare от суммы нескольких строк
	const wchar_t* FileSystem::prepare(wstring path)
	{
		wstring result = (L"\\\\?\\" + path);
		// Поскольку result уничтожается после выхода из этой функции, скопируем строку в *новый* массив
		wchar_t* resstr = new wchar_t[result.length() + 1];
		const wchar_t *cstr = result.c_str();
		memcpy(resstr, cstr, (result.length() + 1) * sizeof(wchar_t));
		return resstr;
	}

	/* проврека на существование файла или папки */
	bool FileSystem::exists(wstring* const path)
	{
		DWORD attrs = GetFileAttributesW(prepare(*path));
		DWORD error = GetLastError();
		if (attrs == INVALID_FILE_ATTRIBUTES && (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)) {
			return false;
		}
		return true;
	}

	/* папка или файл */
	bool FileSystem::isFolder(wstring* const path)
	{
		DWORD attrs = GetFileAttributesW(prepare(*path));
		DWORD error = GetLastError();
		// ERROR_INVALID_NAME выбрасывается в случае, если мы пытаемся обратиться к файлу как к каталогу
		if (attrs == INVALID_FILE_ATTRIBUTES && (error == ERROR_FILE_NOT_FOUND || error == ERROR_INVALID_NAME)) return false;
		return attrs & FILE_ATTRIBUTE_DIRECTORY;
	}

	/* создание папки */
	bool FileSystem::createFolder(wstring* const path)
	{
		const wchar_t* cpath = prepare(*path);
		DWORD result = CreateDirectoryW(cpath, NULL);
		if (result == 0) {
			DWORD error = GetLastError();
			if (error == ERROR_ALREADY_EXISTS) {
				if (isFolder(path)) {
					// Папка уже существует => притворимся, что её создали мы
					return true;
				}
				else {
					// А это вовсе и не папка, нас пытаются заставить создать что-то в файле!
					return false;
				}
			}
			else if (error == ERROR_PATH_NOT_FOUND) {
				//Несуществующий путь => Пытаемся по нему создать нужную папку

				unsigned int steps = 0;
				size_t index = -1;
				wstring newPath;
				do {
					// Разделим путь на шаги, разбив строку на слэши
					index = path->find_first_of('\\', index + 1);
					// Шаг = подстрока от 0 до следующего слэша
					newPath = path->substr(0, index + 1);
					bool ex = exists(&newPath);
					if (ex && !isFolder(&newPath)) {
						wcout << L"В пути найден файл";
						return false;
					}
					else if (!ex && steps) {
						// Не существует и это не диск => создать.
						createFolder(&newPath);
					}
					else if (!ex && !steps) {
						// TO DO бросить исключение (не существует и это диск)
						wcout << L"Путь не существует";
						return false;
					}
					steps++;
				} while (index != wstring::npos);
			}
		}
		return true;
	}

	/* копирование файла в новую категорию */
	bool FileSystem::copyFile(wstring* const from, wstring* const to, wstring* const name)
	{
		// Если путь ведет не к папке -- что вообще нам нужно делать?
		if (!isFolder(to)) return false;

		const wchar_t* cfrom;

		// Заранее подготовим from -- оно нам пригодится
		// Добавим бэкслэш к исходному пути, если его нет, туда же добавляем имя файла
		cfrom = prepare(*from + ((from->back() != L'\\') ? L"\\" : L"") + *name);
		// Добавляем бэкслэш если его нет в итоговом пути
		const wchar_t* bs = (name != 0 && to->back() != L'\\') ? L"\\" : L"";

		DWORD result = CopyFileW(cfrom, prepare(*to + bs + *name), true);
		if (result == 0) {
			DWORD error = GetLastError();
			if (error == ERROR_FILE_EXISTS) {
				/* Если файл уже существует, добавляем к файлу постфиксы
				   В нашем случае это будут цифры
				*/
				// Для начала, извлечем имя файла без расширения. Для этого используем функцию получения расширения
				const wchar_t* ext = getExtension(name);
				wstring filename;
				wstring extension;
				if (ext == nullptr) {
					// Добавляем постфикс(цифру) в конце имени копируемого файла
					filename = *name;
					extension = L"";
				}
				else {
					// Нужно вытащить имя файла до расширения
					filename = name->substr(0, ext - name->c_str());
					extension = ext;
				}
				// копирование файла в нужную директорю
				int index = 1;
				error = NO_ERROR;
				do {
					// Пытаемся скопировать файл
					result = CopyFileW(cfrom,
						prepare(*to + bs + filename + L" (" + to_wstring(index++) + L")" + extension),
						true);
					// Если снова фейл, просто ставим error
					if (result == 0) {
						error = GetLastError();
					}
					else {
						error = NO_ERROR;
					}
				} while (error == ERROR_FILE_EXISTS);
				if (error == NO_ERROR) return true;
			}
			else {
				return false;
			}
		}
		return true;
	}
	/* рекурсивный поиск файлов и подпапок */
	void FileSystem::walk(wstring path, void(*func)(wstring, wstring))
	{
		WIN32_FIND_DATAW fd;
		const wchar_t* suffix = L"*.*";
		// Добавляем бэкслэш если его нет в пути, добавляем маску поиска
		if (path.back() != '\\') path += L"\\";
		const wchar_t* cpath = prepare(path + suffix);

		HANDLE h_fd = FindFirstFileW(cpath, &fd);

		if (h_fd == INVALID_HANDLE_VALUE) {
			DWORD err = GetLastError();
			wcout << cpath << endl;
			return;
		}

		vector<wstring> folders;

		// Первый результат в папках всегда будет '.', поэтому его можно опустить, вызывая функцию только для второго и более
		FindNextFileW(h_fd, &fd);
		while (FindNextFileW(h_fd, &fd)) {
			wstring newPath = path + fd.cFileName;
			if (isFolder(&newPath)) {
				// Рекурсивно обойдем, пожалуй
				walk(newPath, func);
			}
			else {
				(*func)(path, wstring(fd.cFileName));
			}
		}

		if (GetLastError() != ERROR_NO_MORE_FILES) {
			wcout << GetLastError();
		}
		FindClose(h_fd);
	}

	/* получение расширения(типа) файла */
	const wchar_t * FileSystem::getExtension(wstring* name)
	{
		size_t index = name->find_last_of('.');
		if (index == wstring::npos) {
			return nullptr;
		}
		else {
			return name->c_str() + index; /* возвращение указателя на "." */
		}
	}
}
