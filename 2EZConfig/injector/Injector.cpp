#define _WIN32_WINNT_WINXP
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <Windows.h>
#include <Tlhelp32.h>
#include <iostream>
#include <comdef.h> 
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include "Injector.h"
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

BOOL SetPrivilege(HANDLE hToken, LPCTSTR Privilege, BOOL bEnablePrivilege);

// Our shellcode 
unsigned char shellcode[] = {
    0x9C,							// Push all flags
    0x60,							// Push all register
    0x68, 0x42, 0x42, 0x42, 0x42,	// Push dllPathAddr
    0xB8, 0x42, 0x42, 0x42, 0x42,	// Mov eax, loadLibAddr
    0xFF, 0xD0,						// Call eax
    0x61,							// Pop all register
    0x9D,							// Pop all flags
    0xC3							// Ret
};
int version();
FARPROC loadLibraryAddress();
LPVOID virtualAlloc(HANDLE hProcess, char* dll);
BOOL writeProcessMemory(HANDLE hProcess, LPVOID virtualAlloc, char* dll);
VOID createRemoteThreadMethod(HANDLE hProcess, FARPROC loadLibraryAddress, LPVOID virtualAlloc);

DWORD GetProcId(const char* procName)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    wchar_t wtext[20];
    mbstowcs(wtext, procName, strlen(procName) + 1);//Plus null
    LPWSTR ptr = wtext;

    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry)) {
            do {
                _bstr_t b(procEntry.szExeFile);
                const char* c = b;
                if (!_stricmp(b, procName)) {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }

    CloseHandle(hSnap);
    return procId;

}

using namespace Injector;
int Injector::Inject(char* exename) {
    char* dll = (char*)"2EZ.dll";
    const char* procName = exename;

    SHELLEXECUTEINFOW ShExecInfo = { 0 };
    ShExecInfo.cbSize = sizeof(ShExecInfo);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

    wchar_t procName2[255];
    mbstowcs(procName2, procName, strlen(procName) + 1);//Plus null
    LPWSTR ptr = procName2;
    ShExecInfo.lpFile = ptr;


    if (ShellExecuteExW(&ShExecInfo)) {
        int pid = GetProcessId(ShExecInfo.hProcess);

        if (!pid) {
            while (!pid) {
                pid = GetProcId(procName);
            }
        }

        // Enable debug privilege for xp (inject in system process)
        if (version() < 6) {
            HANDLE hProcess = GetCurrentProcess();
            HANDLE hToken;
            if (OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken)) {
                BOOL bPriv = SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
                CloseHandle(hToken);
                printf("[+] Debug privilege\n");
            }
        }

        // Attach to the process through his PID
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (hProcess == NULL) {
            printf("[-] OpenProcess failed\n");
            exit(0);
        }
        else
            printf("[+] OpenProcess success\n");

        FARPROC llAddr = loadLibraryAddress();
        LPVOID dllAddr = virtualAlloc(hProcess, dll);
        writeProcessMemory(hProcess, dllAddr, dll);

        createRemoteThreadMethod(hProcess, llAddr, dllAddr);
        CloseHandle(hProcess);
        return 1;
    }

    return 0;
}



int Injector::InjectWithName(const char* exename ) {
    //this sleep stops sixth from crashing when switching to 1st
    Sleep(10);
    char* dll = (char*)"2EZ.dll";
    int pid = 0;

    if (!pid) {
        pid = GetProcId(exename);
        if (!pid) {
            return 0;
        }
    }

    // Enable debug privilege for xp (inject in system process)
    if (version() < 6) {
        HANDLE hProcess = GetCurrentProcess();
        HANDLE hToken;
        if (OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken)) {
            BOOL bPriv = SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
            CloseHandle(hToken);
            printf("[+] Debug privilege\n");
        }
    }

    // Attach to the process through his PID
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        printf("[-] OpenProcess failed\n");
        exit(0);
    }
    else 
        printf("[+] OpenProcess success\n");

    FARPROC llAddr = loadLibraryAddress();
    LPVOID dllAddr = virtualAlloc(hProcess, dll);
    writeProcessMemory(hProcess, dllAddr, dll);

    createRemoteThreadMethod(hProcess, llAddr, dllAddr);
    CloseHandle(hProcess);
    return 1;

}

// Find the version of the os
int version() {
    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
    return osvi.dwMajorVersion;
}

// SetPrivilege enables/disables process token privilege.
BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) {
    LUID luid;
    BOOL bRet = FALSE;

    if (LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) {
        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = (bEnablePrivilege) ? SE_PRIVILEGE_ENABLED : 0;
        //
        //  Enable the privilege or disable all privileges.
        //
        if (AdjustTokenPrivileges(hToken, FALSE, &tp, (DWORD)NULL, (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
            //
            //  Check to see if you have proper access.
            //  You may get "ERROR_NOT_ALL_ASSIGNED".
            //
            bRet = (GetLastError() == ERROR_SUCCESS);
    }
    return bRet;
}

// Determine the address of LoadLibraryA
FARPROC loadLibraryAddress() {
    FARPROC LLA = GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
    if (LLA == NULL) {
        printf("[-] LoadLibraryA address not found");
        exit(0);
    }
    else
        printf("[+] LoadLibraryA address found 0x%08x\n", LLA);
    return LLA;
}

// Allocate Memory for the DLL
LPVOID virtualAlloc(HANDLE hProcess, char* dll) {
    LPVOID VAE = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(dll), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (VAE == NULL) {
        printf("[-] VirtualAllocEx failed");
        exit(0);
    }
    else
        printf("[+] VirtualAllocEx  0x%08x\n", VAE);
    return VAE;
}

// Copy the DLL into the targeted process memory allocation
BOOL writeProcessMemory(HANDLE hProcess, LPVOID dllAddr, char* dll) {
    BOOL WPM = WriteProcessMemory(hProcess, dllAddr, dll, strlen(dll), NULL);
    if (!WPM) {
        printf("[-] WriteProcessMemory failed");
        exit(0);
    }
    else
        printf("[+] WriteProcessMemory success\n");
    return WPM;
}

// Execute the DLL into the targeted process with CreateRemoteThread method
VOID createRemoteThreadMethod(HANDLE hProcess, FARPROC loadLibraryAddress, LPVOID virtualAlloc) {
    HANDLE CRT = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddress, virtualAlloc, 0, NULL);
    if (CRT == NULL) {
        printf("[-] CreateRemoteThread failed\n");
        exit(0);
    }
    else
        printf("Injection successful process pointer: %x \n", CRT);
        printf("[+] CreateRemoteThread success\n");
}
