#include "Configuration.h"


namespace catalogize {
	ConfigurationState Configuration::verify()
	{
		if (m_from == nullptr || from().empty() || !FileSystem::exists(from()) || !FileSystem::isFolder(from())) {
			return CONFIG_FROM_MISSING;
		}
		if (m_to == nullptr || to().empty() || !FileSystem::createFolder(to())) {
			return CONFIG_TO_MISSING;
		}
		ifstream repofile(repo());
		if (!repofile.good()) {
			return CONFIG_REPO_MISSING;
		}
		return CONFIG_OK;
	}

	Configuration::~Configuration()
	{
		delete m_from;
		delete m_to;
		delete m_repo_path;
	}
}
