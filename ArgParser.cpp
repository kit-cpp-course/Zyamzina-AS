#include "ArgParser.h"

using namespace std;

namespace catalogize {
	bool ArgParser::parse(int argc, wchar_t ** argv) {
		Configuration &conf = Configuration::instance();
		Argument arg = ARG_NONE;

		if (argc < 2) return false;

		for (int i = 1; i < argc; i++) {
			wchar_t* currentArg = argv[i];
			// Позиционные аргументы
			if (i == 1) {
				conf.m_from = new wstring(currentArg);
			}
			else if (i == 2) {
				conf.m_to = new wstring(currentArg);
			}
			// Опциональные аргументы
			else if (arg == ARG_NONE) {
				while ((currentArg[0] == '/' || currentArg[0] == '-')) currentArg++;

				if (wcscmp(currentArg, L"repo") == 0) {
					arg = ARG_REPO;
				}
				else {
					return false;
				}
			}
			else {
				switch (arg) {
				case ARG_REPO:
					conf.m_repo_path = new wstring(currentArg);
					break;
				}
				arg = ARG_NONE;
			}
		}
		return true;
	}
}
