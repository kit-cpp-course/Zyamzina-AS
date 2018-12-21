#include "FileSystem.h"
#include <iostream>
#define NO_ERROR -1

namespace catalogize {
	/* ���������� ���� ��� ������ ��������� */
	// path ���������� �� �������� ��� ����, ����� ����� ���� �������� prepare �� ����� ���������� �����
	const wchar_t* FileSystem::prepare(wstring path)
	{
		wstring result = (L"\\\\?\\" + path);
		// ��������� result ������������ ����� ������ �� ���� �������, ��������� ������ � *�����* ������
		wchar_t* resstr = new wchar_t[result.length() + 1];
		const wchar_t *cstr = result.c_str();
		memcpy(resstr, cstr, (result.length() + 1) * sizeof(wchar_t));
		return resstr;
	}

	/* �������� �� ������������� ����� ��� ����� */
	bool FileSystem::exists(wstring* const path)
	{
		DWORD attrs = GetFileAttributesW(prepare(*path));
		DWORD error = GetLastError();
		if (attrs == INVALID_FILE_ATTRIBUTES && (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)) {
			return false;
		}
		return true;
	}

	/* ����� ��� ���� */
	bool FileSystem::isFolder(wstring* const path)
	{
		DWORD attrs = GetFileAttributesW(prepare(*path));
		DWORD error = GetLastError();
		// ERROR_INVALID_NAME ������������� � ������, ���� �� �������� ���������� � ����� ��� � ��������
		if (attrs == INVALID_FILE_ATTRIBUTES && (error == ERROR_FILE_NOT_FOUND || error == ERROR_INVALID_NAME)) return false;
		return attrs & FILE_ATTRIBUTE_DIRECTORY;
	}

	/* �������� ����� */
	bool FileSystem::createFolder(wstring* const path)
	{
		const wchar_t* cpath = prepare(*path);
		DWORD result = CreateDirectoryW(cpath, NULL);
		if (result == 0) {
			DWORD error = GetLastError();
			if (error == ERROR_ALREADY_EXISTS) {
				if (isFolder(path)) {
					// ����� ��� ���������� => �����������, ��� � ������� ��
					return true;
				}
				else {
					// � ��� ����� � �� �����, ��� �������� ��������� ������� ���-�� � �����!
					return false;
				}
			}
			else if (error == ERROR_PATH_NOT_FOUND) {
				//�������������� ���� => �������� �� ���� ������� ������ �����

				unsigned int steps = 0;
				size_t index = -1;
				wstring newPath;
				do {
					// �������� ���� �� ����, ������ ������ �� �����
					index = path->find_first_of('\\', index + 1);
					// ��� = ��������� �� 0 �� ���������� �����
					newPath = path->substr(0, index + 1);
					bool ex = exists(&newPath);
					if (ex && !isFolder(&newPath)) {
						wcout << L"� ���� ������ ����";
						return false;
					}
					else if (!ex && steps) {
						// �� ���������� � ��� �� ���� => �������.
						createFolder(&newPath);
					}
					else if (!ex && !steps) {
						// TO DO ������� ���������� (�� ���������� � ��� ����)
						wcout << L"���� �� ����������";
						return false;
					}
					steps++;
				} while (index != wstring::npos);
			}
		}
		return true;
	}

	/* ����������� ����� � ����� ��������� */
	bool FileSystem::copyFile(wstring* const from, wstring* const to, wstring* const name)
	{
		// ���� ���� ����� �� � ����� -- ��� ������ ��� ����� ������?
		if (!isFolder(to)) return false;

		const wchar_t* cfrom;

		// ������� ���������� from -- ��� ��� ����������
		// ������� ������� � ��������� ����, ���� ��� ���, ���� �� ��������� ��� �����
		cfrom = prepare(*from + ((from->back() != L'\\') ? L"\\" : L"") + *name);
		// ��������� ������� ���� ��� ��� � �������� ����
		const wchar_t* bs = (name != 0 && to->back() != L'\\') ? L"\\" : L"";

		DWORD result = CopyFileW(cfrom, prepare(*to + bs + *name), true);
		if (result == 0) {
			DWORD error = GetLastError();
			if (error == ERROR_FILE_EXISTS) {
				/* ���� ���� ��� ����������, ��������� � ����� ���������
				   � ����� ������ ��� ����� �����
				*/
				// ��� ������, �������� ��� ����� ��� ����������. ��� ����� ���������� ������� ��������� ����������
				const wchar_t* ext = getExtension(name);
				wstring filename;
				wstring extension;
				if (ext == nullptr) {
					// ��������� ��������(�����) � ����� ����� ����������� �����
					filename = *name;
					extension = L"";
				}
				else {
					// ����� �������� ��� ����� �� ����������
					filename = name->substr(0, ext - name->c_str());
					extension = ext;
				}
				// ����������� ����� � ������ ���������
				int index = 1;
				error = NO_ERROR;
				do {
					// �������� ����������� ����
					result = CopyFileW(cfrom,
						prepare(*to + bs + filename + L" (" + to_wstring(index++) + L")" + extension),
						true);
					// ���� ����� ����, ������ ������ error
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
	/* ����������� ����� ������ � �������� */
	void FileSystem::walk(wstring path, void(*func)(wstring, wstring))
	{
		WIN32_FIND_DATAW fd;
		const wchar_t* suffix = L"*.*";
		// ��������� ������� ���� ��� ��� � ����, ��������� ����� ������
		if (path.back() != '\\') path += L"\\";
		const wchar_t* cpath = prepare(path + suffix);

		HANDLE h_fd = FindFirstFileW(cpath, &fd);

		if (h_fd == INVALID_HANDLE_VALUE) {
			DWORD err = GetLastError();
			wcout << cpath << endl;
			return;
		}

		vector<wstring> folders;

		// ������ ��������� � ������ ������ ����� '.', ������� ��� ����� ��������, ������� ������� ������ ��� ������� � �����
		FindNextFileW(h_fd, &fd);
		while (FindNextFileW(h_fd, &fd)) {
			wstring newPath = path + fd.cFileName;
			if (isFolder(&newPath)) {
				// ���������� �������, �������
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

	/* ��������� ����������(����) ����� */
	const wchar_t * FileSystem::getExtension(wstring* name)
	{
		size_t index = name->find_last_of('.');
		if (index == wstring::npos) {
			return nullptr;
		}
		else {
			return name->c_str() + index; /* ����������� ��������� �� "." */
		}
	}
}
