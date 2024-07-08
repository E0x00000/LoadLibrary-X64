#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <vector>
#include <string>
namespace Inject
{
	bool injectInFiveM();
	std::vector<DWORD> GetProcessIdsByNames(const std::vector<std::string>& processNames);
	std::wstring AnsiToWide(const std::string& str);
	BOOLEAN dllFileAlreadyExists(char dllLocation[]);
	BOOL saveBytesInFile(const std::vector<unsigned char>& rawData, const char dllLocation[]);
	BOOL inject(DWORD procId);
}