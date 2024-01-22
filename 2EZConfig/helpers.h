#pragma once
#include <iostream>
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <TlHelp32.h>
#include <comdef.h> 
//externals
#include <openssl/md5.h>




//Various simple fucntions that might be shared
inline int isProcessOpen(const char* procName)
{
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
                    return 1;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }

    CloseHandle(hSnap);
    return 0;
}

inline int fileExists(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (f != NULL)
    {
        fclose(f);
        return 1;
        
    }

    return 0;
}


// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

inline void RedirectIOToConsole()
{
    int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE* fp;

    // allocate a console for this app
    AllocConsole();

    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

    fp = _fdopen(hConHandle, "w");

    *stdout = *fp;

    setvbuf(stdout, NULL, _IONBF, 0);

    // redirect unbuffered STDIN to the console

    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

    fp = _fdopen(hConHandle, "r");
    *stdin = *fp;
    setvbuf(stdin, NULL, _IONBF, 0);

    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

    fp = _fdopen(hConHandle, "w");

    *stderr = *fp;

    setvbuf(stderr, NULL, _IONBF, 0);


    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
}


inline std::string GetKeyName(unsigned int virtualKey)
{
    unsigned int scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

    // because MapVirtualKey strips the extended bit for some keys
    switch (virtualKey)
    {
    case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: // arrow keys
    case VK_PRIOR: case VK_NEXT: // page up and page down
    case VK_END: case VK_HOME:
    case VK_INSERT: case VK_DELETE:
    case VK_DIVIDE: // numpad slash
    case VK_NUMLOCK:
    {
        scanCode |= 0x100; // set extended bit
        break;
    }
    }

    //fix for some keys not distinguishing
    if (virtualKey == VK_LSHIFT) {
        return "Left Shift";
    }
    if (virtualKey == VK_LCONTROL) {
        return "Left Control";
    }
    if (virtualKey == VK_RCONTROL) {
        return "Right Control";
    }

    char keyName[50];
    if (GetKeyNameText(scanCode << 16, keyName, sizeof(keyName)) != 0)
    {
        return keyName;
    }
    else
    {
        return "[Error]";
    }
}

inline void WritePrivateProfileInt(LPCSTR settingName, LPCSTR settingKey, int settingValue, LPCSTR fileName) {
    char buff[10];
    WritePrivateProfileString(settingName, settingKey, _itoa(settingValue, buff, sizeof(buff)), fileName);
}

inline char* toLower(char* s) {
    for (char* p = s; *p; p++) *p = tolower(*p);
    return s;
}

inline bool hasEnding(std::string const& fullString, std::string const& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}







