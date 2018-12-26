#pragma once
#include <fstream>
#include <string>
#include "Constants.h"
#include "filesystem.h"

using namespace std;
namespace catalogize {
	class ArgParser;
	/*
		Конфигурация приложения.
	*/
	class Configuration
	{
		friend ArgParser;
		wstring* m_from;
		wstring* m_to;
		wstring* m_repo_path;
	public:
		
		static Configuration& instance() {
			static Configuration m_instance;
			return m_instance;
		};
		Configuration(Configuration const&) = delete;
		void operator=(Configuration const&) = delete;

		wstring& from() const { return *m_from; };
		wstring& to() const { return *m_to; };
		wstring& repo() {
			if (m_repo_path == nullptr) {
				m_repo_path = new wstring(L".\\types.repo");
			}
			return *m_repo_path;
		};
		// Проверяет конфигурацию на наличие ошибок
		ConfigurationState verify();

		~Configuration();
	};
}

