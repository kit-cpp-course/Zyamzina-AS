#include "FileTypesRepo.h"
#include "TypeMappings.h"

using namespace std;

namespace catalogize {
	
	FileTypesRepo::FileTypesRepo()
	{
		for (int i = (sizeof(TYPE_NAMES) / sizeof(TYPE_NAMES[0])) - 1; i >= 0; i--) {
			for (int j = TYPE_LENGTHS[i] - 1; j >= 0; j--) {
				extensionToType[TYPES[i][j]] = TYPE_NAMES[i];
			}
		}
	}
	
	/* возвращает расширения(типа) файла */
	std::wstring* FileTypesRepo::getCategory(std::wstring* extension)
	{
		return &extensionToType[*extension];
	}
	
	FileTypesRepo::~FileTypesRepo(){}
}