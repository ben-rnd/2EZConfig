#include "input.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
using namespace input;


bool input::InitJoystick(int joyID) {
    joySetCapture(NULL, joyID, 0, FALSE);
    //sometimes init works even if joy not there, so request info and check for error
    JOYINFOEX joyinfo;
    joyinfo.dwSize = sizeof(joyinfo);
    joyinfo.dwFlags = JOY_RETURNALL;
    DWORD result = joyGetPosEx(joyID, &joyinfo);
    if (result == JOYERR_NOERROR) {
        return TRUE;
    }
    return false;
}

bool input::isJoyButtonPressed(int joyID, DWORD Button) {
    JoystickState joystate;
    JOYINFOEX joyinfo;
    joyinfo.dwSize = sizeof(joyinfo);
    joyinfo.dwFlags = JOY_RETURNBUTTONS;
    DWORD result = joyGetPosEx(joyID, &joyinfo);
    if (result == JOYERR_NOERROR) {
        if (joyinfo.dwButtons & Button) {
            return true;
        }
    }
    return false;
}

UINT8 input::JoyAxisPos(int joyID, int axis) {
    JoystickState joystate;
    JOYINFOEX joyinfo;
    joyinfo.dwSize = sizeof(joyinfo);
    joyinfo.dwFlags = JOY_RETURNALL;
    DWORD result = joyGetPosEx(joyID, &joyinfo);
    if (result == JOYERR_NOERROR) {
        if (!axis) {
            return joyinfo.dwYpos / 257;
        }
        else {
            return joyinfo.dwXpos / 257;
        }

    }
    return 255;
}


bool input::isKbButtonPressed(DWORD key)
{
    //dont bind to mouse buttons
    if (GetAsyncKeyState(key) & 0x8000) {
        return true;
    }
    return false;
}
