#include "LoadLirabry.h"
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
char dllLocation[] = "C:\\cheat.dll";
unsigned int dllLen = sizeof(dllLocation) + 1;

std::vector<DWORD> Inject::GetProcessIdsByNames(const std::vector<std::string>& processNames)
{
    std::vector<DWORD> pids;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(processEntry);
        if (Process32First(snapshot, &processEntry)) {
            do {
                wchar_t szExeFile[MAX_PATH];
                mbstowcs(szExeFile, processEntry.szExeFile, MAX_PATH);
                for (const auto& processName : processNames) {
                    if (_wcsicmp(szExeFile, AnsiToWide(processName).c_str()) == 0)
                    {
                        pids.push_back(processEntry.th32ProcessID);
                        break;
                    }
                }
            } while (Process32Next(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }
    return pids;
}

std::wstring Inject::AnsiToWide(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring strWide(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &strWide[0], size_needed);
    return strWide;
}

BOOLEAN Inject::dllFileAlreadyExists(char dllLocation[])
{
    DWORD dwAttrib = GetFileAttributesA(dllLocation);
    if (dwAttrib == INVALID_FILE_ATTRIBUTES)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL Inject::saveBytesInFile(const std::vector<unsigned char>& rawData, const char dllLocation[])
{
    std::ofstream dllFile(dllLocation, std::ios::out | std::ios::binary);
    if (!dllFile.is_open()) {
       
        return FALSE;
    }
    dllFile.write(reinterpret_cast<const char*>(rawData.data()), rawData.size());
    dllFile.close();
  
    return TRUE;
}

BOOL Inject::inject(DWORD procId)
{
    if (!procId)
    {
        
        return FALSE;
    }
    HMODULE hKernel32 = GetModuleHandle("Kernel32");
    void* llFn = GetProcAddress(hKernel32, "LoadLibraryA");
    if (llFn == NULL)
    {
        return FALSE;
    }
    HANDLE hFiveM = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
    if (hFiveM == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    LPVOID allocatedMemory = VirtualAllocEx(hFiveM, NULL, dllLen, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
    if (allocatedMemory == NULL) {
        return FALSE;
    }
    if (!WriteProcessMemory(hFiveM, allocatedMemory, dllLocation, dllLen, NULL)) {
        return FALSE;
    }
    HANDLE remoteThread = CreateRemoteThread(hFiveM, NULL, 0, (LPTHREAD_START_ROUTINE)llFn, allocatedMemory, 0, NULL);
    CloseHandle(hFiveM);
    return TRUE;
}


bool Inject::injectInFiveM() {
    const std::vector<std::string> fiveMArrayBuilds = {
        "FiveM_b2060_GTAProcess.exe",
        "FiveM_b2545_GTAProcess.exe",
        "FiveM_b2189_GTAProcess.exe",
        "FiveM_b2372_GTAProcess.exe",
        "FiveM_b2372_GameProcess.exe",
        "FiveM_b1604_GTAProcess.exe",
        "FiveM_b2612_GTAProcess.exe",
        "FiveM_b2699_GTAProcess.exe",
        "FiveM_b2802_GTAProcess.exe",
        "FiveM_b3095_GTAProcess.exe"
    };
    std::vector<DWORD> procIds = GetProcessIdsByNames(fiveMArrayBuilds);

    for (DWORD procId : procIds) {
        if (dllFileAlreadyExists(dllLocation))
        {
            if (DeleteFileA(dllLocation) == NULL)
            {
                return false;
            }
        }

        BOOL result = saveBytesInFile(Login::rawData, dllLocation);//
        if (result)
        {
            if (inject(procId) == TRUE)
            {
                return true;
            }
            else {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return false;
}