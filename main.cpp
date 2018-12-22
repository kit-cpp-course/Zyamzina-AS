#include <iostream>
#include <io.h>
#include <fcntl.h>
#include "FileSystem.h"
#include "FileTypesRepo.h"

using namespace catalogize;

FileTypesRepo ftr;
wstring copyRoot = L"C:\\Users\\���������\\Documents\\3 �������\\Collected\\";


void processFile(std::wstring path, std::wstring name) {
	// ���� �����, ������� ������ walk, �������� ��������� copyRoot, ������ �� ������ �� ��������� �������� �����
	if (!path.find(copyRoot)) { return; }
	
	const wchar_t * ext = FileSystem::getExtension(&name);
	if (ext == nullptr) {
		return;
	}
	// ����� �� �����, ����� ����� ����������
	wstring extension(++ext);
	// �������� ���������
	wstring* category = ftr.getCategory(&extension);
	// ���� ��������� �����, ���������� ����
	if (category->empty()) return;

	wstring categoryFolder = copyRoot + (copyRoot.back() != L'\\' ? L"\\" : L"") + *category + L'\\' + extension;

	// ���������, ���������� �� ����� ��� ���� ���������...
	bool exists = FileSystem::exists(&categoryFolder);
	bool folder = FileSystem::isFolder(&categoryFolder);
	if (exists && !folder) { 
		wcout << L" � ���� ������ ���� ";
		return;
	}
	// ...� ���� �� ����������, �������
	else if (!exists) {
		FileSystem::createFolder(&categoryFolder);
	}

	// �������� ����!
	FileSystem::copyFile(&path, &categoryFolder, &name);
	wcout << name << ": " << *category << endl;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	wstring fromRoot;
	//��������� ���������� createFolder
	do {
		wcout << L" ������� ����������, ���� ���������������� ����� ";
		getline(wcin, copyRoot);
	} while (!FileSystem::createFolder(&copyRoot));

	do {
		wcout << L" ������� ����������, ������� ����� ���������������� ";
		getline(wcin, fromRoot);
	} while (!FileSystem::exists(&fromRoot) || !FileSystem::isFolder(&fromRoot));
	FileSystem::walk(fromRoot, &processFile);
	wcout << L" ������������� ����������� " << endl;
}