#include <string>
#include <windows.h>										// We need to include windows.h
#include <stdio.h>
#include <mmsystem.h>
#include "input.h"
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

using namespace input;


bool input::InitJoystick(int joyID){
    DWORD dwResult = 0;
    dwResult = joySetCapture(NULL, joyID, 0, FALSE);

    if (JOYERR_NOERROR) {

        return TRUE;
    }
    return false;
}

bool input::InitJoysticks()
{
    for (int i= 0; i < joyGetNumDevs(); i++) {
        DWORD result = joySetCapture(NULL, i, 0, FALSE);
    }
    return 1;
}

bool input::DestroyJoysticks()
{
    for (int i = 0; i < joyGetNumDevs(); i++) {
        DWORD result;
        result = joyReleaseCapture(i);
    }
    return 1;
}

JoystickState input::GetJoyState(UINT joyID) {
    JoystickState joystate;
    JOYINFOEX joyinfo;
    joyinfo.dwSize = sizeof(joyinfo);
    joyinfo.dwFlags = JOY_RETURNALL;
    DWORD result = joyGetPosEx(joyID, &joyinfo);
    if (result == JOYERR_NOERROR) {
        joystate.input = true;
        joystate.joyID = joyID;
        joystate.NumPressed = joyinfo.dwButtonNumber;
        joystate.buttonsPressed = joyinfo.dwButtons;
        joystate.x = joyinfo.dwXpos / 257;
        joystate.y = joyinfo.dwYpos / 257;
    }
    return joystate;
}

int input::checkKbPressedState()
{
    //dont bind to mouse buttons
    for (unsigned short int i = 0x08; i < 0xFF; i++) {
        
        if (GetAsyncKeyState(i) & 0x8000 && i != (unsigned short int) VK_SHIFT
             && i != (unsigned short int)VK_CONTROL) {
            return i;
        }
    }
    return -1;
}


std::string input::getButtonName(int button)
{

    switch (button) {
        case(JOY_BUTTON1):
            return "1";
            break;
        case(JOY_BUTTON2):
            return "2";
            break;
        case(JOY_BUTTON3):
            return "3";
            break;
        case(JOY_BUTTON4):
            return "4";
            break;
        case(JOY_BUTTON5):
            return "5";
            break;
        case(JOY_BUTTON6):
            return "6";
            break;
        case(JOY_BUTTON7):
            return "7";
            break;
        case(JOY_BUTTON8):
            return "8";
            break;
        case(JOY_BUTTON9):
            return "9";
            break;
        case(JOY_BUTTON10):
            return "10";
            break;
        case(JOY_BUTTON11):
            return "11";
            break;
        case(JOY_BUTTON12):
            return "12";
            break;
        case(JOY_BUTTON13):
            return "13";
            break;
        case(JOY_BUTTON14):
            return "14";
            break;
        case(JOY_BUTTON15):
            return "15";
            break;
        case(JOY_BUTTON16):
            return "16";
            break;
        case(JOY_BUTTON17):
            return "17";
            break;
        case(JOY_BUTTON18):
            return "18";
            break;
        case(JOY_BUTTON19):
            return "19";
            break;
        case(JOY_BUTTON20):
            return "20";
            break;
        case(JOY_BUTTON21):
            return "21";
            break;
        case(JOY_BUTTON22):
            return "22";
            break;
        case(JOY_BUTTON23):
            return "23";
            break;
        case(JOY_BUTTON24):
            return "24";
            break;
        case(JOY_BUTTON25):
            return "25";
            break;
        case(JOY_BUTTON26):
            return "26";
            break;
        case(JOY_BUTTON27):
            return "27";
            break;
        case(JOY_BUTTON28):
            return "28";
            break;
        case(JOY_BUTTON29):
            return "29";
            break;
        case(JOY_BUTTON30):
            return "30";
            break;
        case(JOY_BUTTON31):
            return "31";
            break;
        case(JOY_BUTTON32):
            return "32";
            break;
        }
    return "";
}

