#pragma once
#include <string>
#include <Windows.h>
namespace input {

   typedef struct JoystickState {
        UINT joyID = 16;
        UINT8 x = 0;
        UINT8 y = 0;
        DWORD buttonsPressed = 0;
        int NumPressed = 0;
        bool input = false;
    };

    bool InitJoystick(int joyID);
    bool isJoyButtonPressed(int joyID, DWORD Button);
    UINT8 JoyAxisPos(int joyID, int axis);
    bool isKbButtonPressed(DWORD key);

}
