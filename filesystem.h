#pragma once
#include <string>
#include <vector>
#include "windows.h"
using namespace std;
namespace catalogize {
	class FileSystem {
		static const wchar_t* prepare(wstring path);
	public: 
		static bool exists(wstring* const path);
		static bool isFolder(wstring* const path);
		static bool createFolder(wstring* const path);
		static bool copyFile(wstring* const from, wstring* const to, wstring* const name);
		static void walk(wstring path, void(*func)(wstring, wstring));
		static const wchar_t* getExtension(wstring* name);
	};
}

