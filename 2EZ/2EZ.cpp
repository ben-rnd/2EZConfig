// Ez2 Io Hook built by kasaski - 2022.
// Very simple solution to Hook EZ2 IO, surprised how simple it is and why nothing has been released earler 
// by much more capable programmers :).
#include "2EZ.h"
#include <ShlObj.h>
#include <Shlwapi.h>
#include <iostream>
#include "input.h"
#include <thread>
using namespace std;

ioBinding buttonBindings[NUM_OF_IO];
ioAnalogs analogBindings[NUM_OF_ANALOG];
Joysticks joysticks[NUM_OF_JOYSTICKS];
virtualTT vTT[2];
uintptr_t baseAddress;
uint8_t apKey;
int GameVer;
djGame currGame;
LPCSTR config = ".\\2EZ.ini";


UINT8 getButtonInput(int ionRangeStart) {
    UINT8 output = 255;
    int count = 0;
    for (int i = ionRangeStart; i < ionRangeStart+8; i++) {
        if (buttonBindings[i].bound) {
            if (buttonBindings[i].method) {
                if (input::isJoyButtonPressed(buttonBindings[i].joyID, buttonBindings[i].binding)) {
                    output = output & byteArray[count];
                }
            } else {
                if(input::isKbButtonPressed(buttonBindings[i].binding)) {
                    output = output & byteArray[count];
                }
            }            
        }
        count ++;
    }
    return output;
}

UINT8 getAnalogInput(int player) {
    if (analogBindings[player].bound){
        UINT8 ttRawVal = input::JoyAxisPos(analogBindings[player].joyID, analogBindings[player].axis);
        return (analogBindings[player].reverse ? ~ttRawVal : ttRawVal) + vTT[player].pos;
    }else{ 
        return vTT[player].pos;
    }   
}


//Debugging anything in here is a bitch as attaching a debugger 
//often crashes the game when using the io shim.
LONG WINAPI IOportHandler(PEXCEPTION_POINTERS pExceptionInfo) {
    //ty batteryshark for the great writeup :)
    
    if (pExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_PRIV_INSTRUCTION) {
        if (pExceptionInfo->ExceptionRecord->ExceptionCode != DBG_PRINTEXCEPTION_C) {
        }
        return EXCEPTION_EXECUTE_HANDLER;
    }
 
    unsigned int eip_val = *(unsigned int*)pExceptionInfo->ContextRecord->Eip;
    //
    // EZ2DJ IO INPUT.
    // 
    
    // Each port has 8 buttons assigned to it. 
    // each port reports a 8bit value, 0 = pressed, 1 = not pressed

    //EZ2DJ Io Input - Missing coin input, no clue what QE are?
    // HANDLE IN AL, DX
    if ((eip_val & 0xFF) == 0xEC) {
        //handle io calls while controller still being setup
        DWORD VAL = pExceptionInfo->ContextRecord->Edx & 0xFFFF;
        switch (pExceptionInfo->ContextRecord->Edx & 0xFFFF) {
        case 0x101: //TEST SVC E4 E3 E2 E1 P2 P1
            pExceptionInfo->ContextRecord->Eax = getButtonInput(0);
            break;
        case 0x102: //P1 Buttons - PEDAL QE QE B5 B4 B3 B2 B1
            pExceptionInfo->ContextRecord->Eax = getButtonInput(8);
            break;
        case 0x103://P1 TT 0-255
            pExceptionInfo->ContextRecord->Eax = getAnalogInput(0);
            break;
        case 0x104://P2 TT 0-255
            pExceptionInfo->ContextRecord->Eax = getAnalogInput(1);
            break;
        case 0x106: //P2 Buttons - PEDAL QE QE B10 B9 B8 B7 B6
            pExceptionInfo->ContextRecord->Eax = getButtonInput(16);
            break;
        default:
            pExceptionInfo->ContextRecord->Eax = 0;
            break;
        }
        
        pExceptionInfo->ContextRecord->Eip++;
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    //IO OUTPUT
    //For Lighting
    //HANDLE IN  DX, AL
    if ((eip_val & 0xFF) == 0xEE) {
        DWORD VAL = pExceptionInfo->ContextRecord->Edx & 0xFF;
        switch (pExceptionInfo->ContextRecord->Edx & 0xFFFF) {
        case 0x100: //NA NA NA NEONS LAMPX LAMPX LAMPX LAMPX
        case 0x101: //NA NA E4 E3 E2 E1 P2 P1
        case 0x102: //NA NA TT1 B5 B4 B3 B2 B1
        case 0x103: //NA NA TT2 B10 B9 B8 B7 B6
        default:
            break;
        }
        pExceptionInfo->ContextRecord->Eip++;
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    //
    // EZ2Dancer IO Ports
    // 
    
    // Game input
    // -- Handle IN AX,DX --

    //testing indicates this is somewhat the right range just need to verify further
    //its a PIA so low prio
    if ((eip_val & 0xFFFF) == 0xED66) {
        DWORD VAL = pExceptionInfo->ContextRecord->Edx & 0xFFFF;
        switch (pExceptionInfo->ContextRecord->Edx & 0xFFFF) {
        case 0x300://No clue what this maps to
            /*pExceptionInfo->ContextRecord->Eax = do something
            break;*/
        case 0x302://No clue what this maps to
            /*pExceptionInfo->ContextRecord->Eax = do something
            break;*/
        case 0x304://No clue what this maps to
            /*pExceptionInfo->ContextRecord->Eax = do something
            break;*/
        case 0x306://No clue what this maps to
            /*pExceptionInfo->ContextRecord->Eax = do something
            break;*/
        case 0x308://No clue what this maps to
            /*pExceptionInfo->ContextRecord->Eax = do something
            break;*/
        case 0x310://No clue what this maps to
            /*pExceptionInfo->ContextRecord->Eax = do something
            break;*/
        case 0x312://No clue what this maps to
            /*pExceptionInfo->ContextRecord->Eax = do something
            break;*/
        case 0x314://No clue what this maps to
            /*pExceptionInfo->ContextRecord->Eax = do something
            break;*/
        default:
            pExceptionInfo->ContextRecord->Eax = 0;
            break;
        }
        // Skip over the instruction that caused the exception:
        pExceptionInfo->ContextRecord->Eip += 2;
    }

    // Game output (lights)
    //--Handle IN DX, AX--
    if ((eip_val & 0xFFFF) == 0xEF66) {
        DWORD VAL = pExceptionInfo->ContextRecord->Edx & 0xFFFF;
        switch (pExceptionInfo->ContextRecord->Edx & 0xFFFF) {
        default:
            pExceptionInfo->ContextRecord->Eax = 0;
            break;
        }
        // Skip over the instruction that caused the exception:
        pExceptionInfo->ContextRecord->Eip += 2;
    }

    /*
    If another handler should be called in sequence,
    use return EXCEPTION_EXECUTE_HANDLER;
    Otherwise - let the thread continue.
    */
    return EXCEPTION_CONTINUE_EXECUTION;
}

DWORD WINAPI virtualTTThread(void* data) {


    while (true) {
        
        if (GetAsyncKeyState(vTT[0].plus) & 0x8000) {
            vTT[0].pos += 1;
        }
        if (GetAsyncKeyState(vTT[0].minus) & 0x8000) {
            vTT[0].pos -= 1;
        }

        if (GetAsyncKeyState(vTT[1].plus) & 0x8000) {
            vTT[1].pos += 1;
        }
        if (GetAsyncKeyState(vTT[1].minus) & 0x8000) {
            vTT[1].pos -= 1;
        }
        Sleep(5);
    }

    return 0;
}

DWORD WINAPI alternateInputThread(void* data) {

    if (strcmp(currGame.name, "Final:EX") == 0){
        bool autoPlayButtonState = false;
        uintptr_t fnexApOffset = 0x175F2E0;

        while (true) {
            if (GetAsyncKeyState(apKey) & 0x8000) {
                if (!autoPlayButtonState) {
                    autoPlayButtonState = true;
                    ChangeMemory(baseAddress, 1 + (0 - (*reinterpret_cast<int*>(baseAddress + fnexApOffset))), fnexApOffset);
                }
            }
            else if (autoPlayButtonState) {
                autoPlayButtonState = false;
            }
        }
    }

    if (strcmp(currGame.name, "Final") == 0) {
        bool autoPlayButtonState = false;
        uintptr_t apOffset = 0x175E290;

        while (true) {
            if (GetAsyncKeyState(apKey) & 0x8000) {
                if (!autoPlayButtonState) {
                    autoPlayButtonState = true;
                    ChangeMemory(baseAddress, 1 + (0 - (*reinterpret_cast<int*>(baseAddress + apOffset))), apOffset);
                }
            }
            else if (autoPlayButtonState) {
                autoPlayButtonState = false;
            }
        }
    }

    if (strcmp(currGame.name, "Night Traveller") == 0) {
        bool autoPlayButtonState = false;
        uintptr_t apOffset = 0x1360EA4;

        while (true) {
            if (GetAsyncKeyState(apKey) & 0x8000) {
                if (!autoPlayButtonState) {
                    autoPlayButtonState = true;
                    ChangeMemory(baseAddress, 1 + (0 - (*reinterpret_cast<int*>(baseAddress + apOffset))), apOffset);
                }
            }
            else if (autoPlayButtonState) {
                autoPlayButtonState = false;
            }
        }
    }

    if (strcmp(currGame.name, "Endless Circulation") == 0) {
        bool autoPlayButtonState = false;
        uintptr_t apOffset = 0xEF606C;

        while (true) {
            if (GetAsyncKeyState(apKey) & 0x8000) {
                if (!autoPlayButtonState) {
                    autoPlayButtonState = true;
                    ChangeMemory(baseAddress, 1 + (0 - (*reinterpret_cast<int*>(baseAddress + apOffset))), apOffset);
                }
            }
            else if (autoPlayButtonState) {
                autoPlayButtonState = false;
            }
        }
    }

    return 0;
}

void patchName() {

    //Set version text in test menu
    char newName[] = "EZ2AC_FN";
    GetPrivateProfileStringA("Settings", "nameOverride", "COCK", newName, 8, config);
    char pattern[] = "EZ2AC_FN";
    DWORD nameOffset = FindPattern(pattern);
    unsigned long OldProtection;
    VirtualProtect((LPVOID)(nameOffset), sizeof(newName), PAGE_EXECUTE_READWRITE, &OldProtection);
    CopyMemory((void*)(nameOffset), &newName, sizeof(newName));
    VirtualProtect((LPVOID)(nameOffset), sizeof(newName), OldProtection, NULL);

}

DWORD PatchThread() {

    //Get Game config file
    HANDLE ez2Proc = GetCurrentProcess();
    baseAddress = (uintptr_t)GetModuleHandleA(NULL);
    GameVer = GetPrivateProfileIntA("Settings", "GameVer", 0, config);
    currGame = djGames[GameVer];
    
    //Get Button Bindings config file
    char ControliniPath[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, ControliniPath);
    PathAppendA(ControliniPath, (char*)"2EZ.ini");

    patchName();
    
    //Short sleep to fix crash when using legitimate data with dongle, can be overrden in ini if causing issues.
    Sleep(GetPrivateProfileIntA("Settings", "ShimDelay", 10, config));


    //re-enable keyboard if playing FNEX - do this before any delays
    if (strcmp(currGame.name, "Final:EX") == 0) {
        zeroMemory(baseAddress, 0x15D51);
    }

    //Hook IO 
    if (GetPrivateProfileIntA("Settings", "EnableIOHook", 0, config)) {
        SetUnhandledExceptionFilter(IOportHandler);
    }


    //Set version text in test menu
    char pattern[] = "Version %d.%02d";
    DWORD versionText = FindPattern(pattern);
    char newText[] = "2EZConfig 1.03";
    unsigned long OldProtection;
    VirtualProtect((LPVOID)(versionText), sizeof(newText), PAGE_EXECUTE_READWRITE, &OldProtection);
    CopyMemory((void*)(versionText), &newText, sizeof(newText));
    VirtualProtect((LPVOID)(versionText), sizeof(newText), OldProtection, NULL);



    //Setup Buttons
    for (int b = 0; b < NUM_OF_IO; b++) {
        char buff[20];
        GetPrivateProfileStringA(ioButtons[b], "method", NULL, buff, 20, ControliniPath);
        if (buff != NULL) {

            if (strcmp(buff, "Key") == 0) {
                buttonBindings[b].bound = true;
                buttonBindings[b].method = 0;
            }
            else {
                buttonBindings[b].bound = true;
                buttonBindings[b].method = 1;
                joysticks[buttonBindings[b].joyID].init = true;
            }
            buttonBindings[b].joyID = GetPrivateProfileIntA(ioButtons[b], "JoyID", 16, ControliniPath);
            buttonBindings[b].binding = GetPrivateProfileIntA(ioButtons[b], "Binding", NULL, ControliniPath);
        }
       
    }

    //Setup Analogs
    for (int a = 0; a < NUM_OF_ANALOG; a++) {
        char buff[20];
        GetPrivateProfileStringA(analogs[a], "Axis", NULL, buff, 20, ControliniPath);
        if (buff != NULL) {
            analogBindings[a].bound = true;
            joysticks[analogBindings[a].joyID].init = true;
            if (strcmp(buff, "X") == 0) {
                analogBindings[a].axis = 0;
            }
            else {
                analogBindings[a].axis = 1;
            }
            analogBindings[a].joyID = GetPrivateProfileIntA(analogs[a], "JoyID", 16, ControliniPath);
            analogBindings[a].reverse = GetPrivateProfileIntA(analogs[a], "Reverse", 0, ControliniPath);
        }
    }
    
    //auto play key
    apKey =  GetPrivateProfileIntA("Autoplay", "Binding", VK_F11, ControliniPath);

    ///PATCHING SECTION


    //Some of the games (ie final) take a while to initialise and will crash or clear out the bindings unless delayed
    //Doesnt cause any issues so i just set this globally on all games, can be overidden in .ini if needed.
    //since we're already hooking IO theres no problem doing this.
    Sleep(GetPrivateProfileIntA("Settings", "BindDelay", 2000, config));

    if (strcmp(currGame.name, "Evolve") == 0 && GetPrivateProfileIntA("Settings", "EvWin10Fix", 0, config)) {
        NOPMemory(baseAddress, 0x11757);
        NOPMemory(baseAddress, 0x11758);
        NOPMemory(baseAddress, 0x11759);
        NOPMemory(baseAddress, 0x11770);
        NOPMemory(baseAddress, 0x11771);
    }
    //Sometimes GetModuleHandleA wasnt working?
    //set to the known base address offset
    //This has never not worked but I dont want to rely on it
    //if (currGame.baseAddressOverride) {
    //    baseAddress = 0x400000;
    //}

    if (currGame.devOffset != 0x00 && GetPrivateProfileIntA("Settings", "EnableDevControls", 0, config)) {
        setDevBinding(baseAddress, currGame.devOffset, ControliniPath);
    }

    if (currGame.modeTimeOffset != 0x00 && GetPrivateProfileIntA("Settings", "ModeSelectTimerFreeze", 0, config)) {
        zeroMemory(baseAddress, currGame.modeTimeOffset);
    }

    if (currGame.songTimerOffset != 0x00 && GetPrivateProfileIntA("Settings", "SongSelectTimerFreeze", 0, config)) {
        zeroMemory(baseAddress, currGame.songTimerOffset);
    }

    //FINAL save settings patch
    if (strcmp(currGame.name, "Final") == 0 && GetPrivateProfileIntA("KeepSettings", "Enabled", 0, config)) {
            FnKeepSettings(baseAddress);
    }

    if (GetPrivateProfileIntA("StageLock", "Enabled", 0, config)) {
        
        if (strcmp(currGame.name, "Final") == 0) {
            FNStageLock(baseAddress);
        }

        if (strcmp(currGame.name, "Night Traveller") == 0) {
            NTStageLock(baseAddress);
        }

        if (strcmp(currGame.name, "Final:EX") == 0) {
            FNEXStageLock(baseAddress);
        }
        
    }

    vTT[0].plus = GetPrivateProfileIntA("P1 TT+", "Binding", NULL, ControliniPath);
    vTT[0].minus = GetPrivateProfileIntA("P1 TT-", "Binding", NULL, ControliniPath);
    vTT[1].plus = GetPrivateProfileIntA("P2 TT+", "Binding", NULL, ControliniPath);
    vTT[1].minus = GetPrivateProfileIntA("P2 TT-", "Binding", NULL, ControliniPath);

    HANDLE turntableThread = CreateThread(NULL, 0, virtualTTThread, NULL, 0, NULL);
    HANDLE inputThread = CreateThread(NULL, 0, alternateInputThread, NULL, 0, NULL);

    
    
    return NULL;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)PatchThread, 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

