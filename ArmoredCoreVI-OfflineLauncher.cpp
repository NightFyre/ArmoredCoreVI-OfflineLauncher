#include <Windows.h>
#include <iostream>

bool bInject = false;                               //  SHOULD INJECT DLL ?
const wchar_t* czPath = L"armoredcore6.exe";        //  EXECUTABLE PATH
const wchar_t* czParams = L"-eac-nop-loaded";       //  LAUNCH PARAMETERS
const char* PATH = "";                              //  DLL PATH
DWORD APIENTRY main(void*)
{
    //  CREATE PROCESS
    PROCESS_INFORMATION     pInfo;
    STARTUPINFO             sInfo;
    ZeroMemory(&sInfo, sizeof(sInfo));
    sInfo.cb = sizeof(sInfo);
    ZeroMemory(&pInfo, sizeof(pInfo));
    if (!CreateProcessW(czPath, (LPWSTR)czParams, NULL, NULL, FALSE, NULL, NULL, NULL, &sInfo, &pInfo))
        return EXIT_FAILURE;

    //  INJECT DLL
    if (bInject)
    {
        pInfo.hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, pInfo.dwProcessId);
        void* addr = VirtualAllocEx(pInfo.hProcess, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!WriteProcessMemory(pInfo.hProcess, addr, PATH, strlen(PATH) + 1, 0))
        {
            TerminateProcess(pInfo.hProcess, EXIT_FAILURE);
            return EXIT_FAILURE;
        }

        HANDLE hThread = CreateRemoteThread(pInfo.hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, addr, 0, 0);
        if (!hThread)
        {
            VirtualFreeEx(pInfo.hProcess, addr, 0, MEM_RELEASE);
            TerminateProcess(pInfo.hProcess, EXIT_FAILURE);
            return EXIT_FAILURE;
        }
        CloseHandle(hThread);
    }

    return EXIT_SUCCESS;
}