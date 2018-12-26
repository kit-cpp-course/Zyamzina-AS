#pragma once
#include <string>
#include <codecvt>
#include "Constants.h"
#include "Configuration.h"

namespace catalogize {
	/*
		Парсер параметров командной строки.
		Результат работы сохраняется в объект Configuration.
	*/
	class ArgParser
	{
	public:
		// Загрузить конфигурацию из параметров командной строки
		static bool parse(int argc, wchar_t ** argv);
	};
}

