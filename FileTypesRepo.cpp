#include "FileTypesRepo.h"

using namespace std;

namespace catalogize {

	int FileTypesRepo::load()
	{
		Configuration &conf = Configuration::instance();
		wifstream input;
		input.open(conf.repo());

		int loaded = 0;
		wstring line, extension, category;
		while (input.good()) {
			getline(input, line);
			int separator = line.find(':');
			extension = line.substr(0, separator);
			category = line.substr(separator + 1, line.length() - separator);
			extensionToType[extension] = category;
			loaded++;
		}
		return loaded;
	}

	/* возвращает расширения(типа) файла */
	std::wstring& FileTypesRepo::getCategory(std::wstring& extension)
	{
		return extensionToType[extension];
	}
}
