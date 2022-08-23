#include "2EZconfig.h"
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include "injector/injector.h"
#include "helpers.h"
#include "inputManager/device.h"
#include "inputManager/input.h"
#include <dirent.h>

//externals
#include <openssl/md5.h>

//imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include <GLFW/glfw3.h>
#include <ShlObj.h>
#include <Shlwapi.h>


using namespace EZConfig;

void settingsWindow();
void analogsWindow();
void buttonsWindow();
void HelpMarker(const char* desc);
int detectGameVersion();

LPSTR ControliniPath;

int EZConfig::RenderUI(GLFWwindow* window) {

    static bool use_work_area = true;
    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

    // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
    // Based on your use case you may want one of the other.
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
    ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

    char exeName[255];
    GetPrivateProfileStringA("Settings", "EXEName", "EZ2AC.exe", exeName, sizeof(exeName), ConfigIniPath);

    if (ImGui::Begin("Menu", (bool*)true, flags))
    {
        ImGui::Text("R U READY 2 GO INSIDA DJ BOX?");
        ImGui::SameLine(ImGui::GetWindowWidth() - 80);
        if (ImGui::Button("Play EZ2!"))
        {
            if (fileExists(exeName)) {
                int GameVer = GetPrivateProfileIntA("Settings", "GameVer", -1, ConfigIniPath);
                if (strcmp(djGames[GameVer].name, "6th Trax ~Self Evolution~") == 0) {
                    glfwDestroyWindow(window);
                    glfwTerminate();
                    return sixthBackgroundLoop(exeName);
                }
                else if (Injector::Inject(exeName)) {
                    //close
                    return 0;
                }
            }
            else {
                ImGui::OpenPopup("EXE Error");
                
            }
        }


        //Selector Tabs
        ImGui::Separator();
        if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
        {
            //Begin settings tab
            if (ImGui::BeginTabItem("Settings"))
            {
                settingsWindow();
                ImGui::EndTabItem();
            }

            //Begin Buttons Tab
            if (ImGui::BeginTabItem("Buttons"))
            {
                buttonsWindow();
                ImGui::EndTabItem();
            }

            //Begin Analogs Tab
            if (ImGui::BeginTabItem("Analogs"))
            {
                analogsWindow();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Lights"))
            {
                //lightsWindow();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }


    if (ImGui::BeginPopupModal("EXE Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("%s Could not be found", exeName);
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Text("");
    ImGui::SameLine(ImGui::GetWindowWidth() - 170);
    ImGui::Text("Made by kasaski - 2022");
    ImGui::End();
	return 1;
}

//Sixth trax uses a launcher for booting and then switching between 6th and remember 1st
//they are technically 2 different games, with the launcher handling the handover between the two
//The launcher, 6th, and remeber1st need to be rehooked each time the games are switched.
int EZConfig::sixthBackgroundLoop(char* launcherName) {
    RedirectIOToConsole();
    bool inSixth = false;
    //6th's launcher name should be anything but EZ2DJ.exe, as remember1st exe needs to be set that, 
    //and the hook will conflict if they are both the same
    printf("lauching 6th loop\n");
    Injector::Inject(launcherName);
    while (isProcessOpen(launcherName) ){
        //both these names should never changes, the 6th trax launcher exe requires these names to function
        if (!inSixth) {
            if (Injector::InjectWithName("EZ2DJ6th.exe")) {
                inSixth = true;
                printf("Found 6th\n");
            }

        }
        else {
            if (Injector::InjectWithName("Ez2DJ.exe")) {
                Injector::InjectWithName(launcherName);
                inSixth = false;
                printf("Found 1st\n");
            }
        }
       
    }

    return 0;
}


void settingsWindow() {

    char buff[30];
    char exeName[255];

    //Get game version currently set, if not set try to detect game
    int GameVer = GetPrivateProfileIntA("Settings", "GameVer", -1, ConfigIniPath);
    if (GameVer < 0 ) {
        GameVer = detectGameVersion();
    }

    GetPrivateProfileStringA("Settings", "EXEName", "EZ2AC.exe", exeName, sizeof(exeName), ConfigIniPath);
   
    bool overrideExe = GetPrivateProfileIntA("Settings", "OverrideExe", 0, ConfigIniPath);
    bool IOHook = GetPrivateProfileIntA("Settings", "EnableIOHook", 1, ConfigIniPath);
    bool DevControl = GetPrivateProfileIntA("Settings", "EnableDevControls", 0, ConfigIniPath);
    bool modeFreeze = GetPrivateProfileIntA("Settings", "ModeSelectTimerFreeze", 0, ConfigIniPath);
    bool songFreeze = GetPrivateProfileIntA("Settings", "SongSelectTimerFreeze", 0, ConfigIniPath);

    //Setting Save Patch for final, wont bother with this again for any other games
    bool saveSettings = GetPrivateProfileIntA("Settings", "KeepSettings", 0, ConfigIniPath);
    bool random = GetPrivateProfileIntA("KeepSettings", "Random", 1, ConfigIniPath);
    bool note = GetPrivateProfileIntA("KeepSettings", "Note", 1, ConfigIniPath);
    bool autoScratchPedal = GetPrivateProfileIntA("KeepSettings", "Auto", 1, ConfigIniPath);
    bool fade = GetPrivateProfileIntA("KeepSettings", "Fade", 1, ConfigIniPath);
    bool scroll = GetPrivateProfileIntA("KeepSettings", "Scroll", 1, ConfigIniPath);
    bool visual = GetPrivateProfileIntA("KeepSettings", "Visual", 1, ConfigIniPath);
    bool panel = GetPrivateProfileIntA("KeepSettings", "Panel", 1, ConfigIniPath);
    int defaultNote = GetPrivateProfileIntA("KeepSettings", "DefaultNote", 0, ConfigIniPath);
    int defaultPanel = GetPrivateProfileIntA("KeepSettings", "DefaultPanel", 0, ConfigIniPath);
    int defaultVisual = GetPrivateProfileIntA("KeepSettings", "DefaultVisual", 0, ConfigIniPath);

    //Begin Settings Window
    ImGui::BeginChild("Settings", { 0, ImGui::GetWindowHeight() - 85 }, false, ImGuiWindowFlags_HorizontalScrollbar);


    //Game version dropdown selector
    if (ImGui::BeginCombo("Game Version", djGames[GameVer].name)) 
    {
        for (int n = 0; n < IM_ARRAYSIZE(djGames); n++)
        {
            bool is_selected = (n == GameVer); 
            if (ImGui::Selectable(djGames[n].name, is_selected)) {
                GameVer = n;
            }
            if (GameVer == n) {
                ImGui::SetItemDefaultFocus();  

            }
        }
        ImGui::EndCombo();
    }
    WritePrivateProfileString("Settings", "GameVer", _itoa(GameVer, buff, 10), ConfigIniPath);


    ImGui::Separator();

    //Override exe name section
    ImGui::Checkbox("Overide Exucutable Target", &overrideExe);
    ImGui::SameLine();
    HelpMarker("Set a custom exucutable target");
    WritePrivateProfileString("Settings", "OverrideExe", _itoa(overrideExe, buff, 10), ConfigIniPath);
    if (overrideExe) {
        ImGui::InputText("Name of EXE", exeName, IM_ARRAYSIZE(exeName));
        WritePrivateProfileString("Settings", "EXEName", exeName, ConfigIniPath);
    }
    else {
        WritePrivateProfileString("Settings", "EXEName", djGames[GameVer].defaultExeName, ConfigIniPath);
    }



    //Input options. IO shim always on by default as 99% of people using this dont own a cab lol
    ImGui::Separator();
    ImGui::Text("Input");

    ImGui::Checkbox("Enable I/O Shim", &IOHook);
    WritePrivateProfileString("Settings", "EnableIOHook", _itoa(IOHook, buff, 10), ConfigIniPath);
    ImGui::SameLine();
    HelpMarker("Enable the IO Shim for IO emulation. This also has the extra benifit of enabling the games to run on modern windows.");


    //Dev binding checkbox
    if (djGames[GameVer].hasDevBindings) {
        ImGui::Checkbox("Enable Dev Input Binding", &DevControl);
        WritePrivateProfileString("Settings", "EnableDevControls", _itoa(DevControl, buff, 10), ConfigIniPath);
        ImGui::SameLine();
        HelpMarker("This will Enable the keybinding of the dev input.");
    }


    //if any of these patches exists create new section
    if (djGames[GameVer].hasSaveSettings || djGames[GameVer].hasModeFreeze || djGames[GameVer].hasSongFreeze) {
        ImGui::Separator();
        ImGui::Text("Patches");
    }

    //Timer freese should probably all be under 1 setting I dont know why ive split them out
    //Reminder to fix timer unfreezing while selecting song difficulty/changing settings (NT and up) and name entry timer freeze
    
    //Mode timer freeze
    if (djGames[GameVer].hasModeFreeze) {

        ImGui::Checkbox("Enable Mode Select Timer Freeze", &modeFreeze);
        ImGui::SameLine();
        HelpMarker("Pauses the count down timer when selecting a game mode.");
        WritePrivateProfileString("Settings", "ModeSelectTimerFreeze", _itoa(modeFreeze, buff, 10), ConfigIniPath);
    }

    //Song timer freeze 
    if (djGames[GameVer].hasSongFreeze) {
        ImGui::Checkbox("Enable Song Select Timer Freeze", &songFreeze);
        ImGui::SameLine();
        HelpMarker("Pauses the count down timer when selecting a song.");
        WritePrivateProfileString("Settings", "SongSelectTimerFreeze", _itoa(songFreeze, buff, 10), ConfigIniPath);
    }


    //Final specific save settings options
    //I cant be bothered adapting this to other game versions. Its fairly unstabkle anyway.
    if (djGames[GameVer].hasSaveSettings) {
        ImGui::Checkbox("Keep Settings Between Credits", &saveSettings);
        ImGui::SameLine();
        HelpMarker("Settings will not be reset at the end of a credit on standard modes");
        WritePrivateProfileString("Settings", "KeepSettings", _itoa(saveSettings, buff, 10), ConfigIniPath);

        if (saveSettings) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cannot guarantee compatibility with 5key Only, Ruby,");
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "EZ2Catch and Turntable modes");
            ImGui::Indent(16.0f);
            ImGui::Columns(2, "settings", false);
            ImGui::Checkbox("Random", &random);
            WritePrivateProfileString("KeepSettings", "Random", _itoa(random, buff, 10), ConfigIniPath);

            ImGui::NextColumn();
            ImGui::Checkbox("Note Skin", &note);
            WritePrivateProfileString("KeepSettings", "Note", _itoa(note, buff, 10), ConfigIniPath);

            ImGui::NextColumn();
            ImGui::Checkbox("Auto Scratch/Pedal", &autoScratchPedal);
            WritePrivateProfileString("KeepSettings", "Auto", _itoa(autoScratchPedal, buff, 10), ConfigIniPath);

            ImGui::NextColumn();
            ImGui::Checkbox("Fade Effect", &fade);
            WritePrivateProfileString("KeepSettings", "Fade", _itoa(fade, buff, 10), ConfigIniPath);

            ImGui::NextColumn();
            ImGui::Checkbox("Scroll Effect", &scroll);
            WritePrivateProfileString("KeepSettings", "Scroll", _itoa(scroll, buff, 10), ConfigIniPath);

            ImGui::NextColumn();
            ImGui::Checkbox("Visual", &visual);
            WritePrivateProfileString("KeepSettings", "Visual", _itoa(visual, buff, 10), ConfigIniPath);

            ImGui::NextColumn();
            ImGui::Checkbox("Panel Skin", &panel);
            WritePrivateProfileString("KeepSettings", "Panel", _itoa(panel, buff, 10), ConfigIniPath);


            ImGui::Columns(1, "", false);

            ImGui::Text("Defaults: ");
            ImGui::SameLine();
            HelpMarker("These will only be set once at start up. If the settings are changed at anytime they will presist until change again or the game is closed and reopened");
            ImGui::Columns(2, "", false);

            if (!note) {
                ImGui::BeginDisabled();
            }
            ImGui::Text("Note Skin:");

            if (ImGui::BeginCombo("##NoteDefault", noteSkins[defaultNote])) 
            {
                for (int n = 0; n < IM_ARRAYSIZE(noteSkins); n++)
                {
                    bool is_selected = (n == defaultNote); 
                    if (ImGui::Selectable(noteSkins[n], is_selected)) {
                        defaultNote = n;
                    }
                    if (defaultNote == n) {
                        ImGui::SetItemDefaultFocus();

                    }
                }
                ImGui::EndCombo();
            }
            if (!note) {
                ImGui::EndDisabled();
            }
            WritePrivateProfileString("KeepSettings", "DefaultNote", _itoa(defaultNote, buff, 10), ConfigIniPath);


            if (!panel) {
                ImGui::BeginDisabled();
            }

            ImGui::NextColumn();
            ImGui::Text("Panel Skin:");
            if (ImGui::BeginCombo("##PanelDefault", panelSkins[defaultPanel])) 
            {
                for (int n = 0; n < IM_ARRAYSIZE(panelSkins); n++)
                {
                    bool is_selected = (n == defaultPanel); 
                    if (ImGui::Selectable(panelSkins[n], is_selected)) {
                        defaultPanel = n;
                    }
                    if (defaultPanel == n) {
                        ImGui::SetItemDefaultFocus();  

                    }
                }
                ImGui::EndCombo();
            }
            WritePrivateProfileString("KeepSettings", "DefaultPanel", _itoa(defaultPanel, buff, 10), ConfigIniPath);

            if (!panel) {
                ImGui::EndDisabled();
            }

            if (!visual) {
                ImGui::BeginDisabled();
            }
            ImGui::NextColumn();
            ImGui::Text("Visual Setting:");
            if (ImGui::BeginCombo("##VisualDefault", VisualSettings[defaultVisual])) 
            {
                for (int n = 0; n < IM_ARRAYSIZE(VisualSettings); n++)
                {
                    bool is_selected = (n == defaultPanel);
                    if (ImGui::Selectable(VisualSettings[n], is_selected)) {
                        defaultVisual = n;
                    }
                    if (defaultVisual == n) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            WritePrivateProfileString("KeepSettings", "DefaultVisual", _itoa(defaultVisual, buff, 10), ConfigIniPath);
            if (!visual) {
                ImGui::EndDisabled();
            }

            ImGui::Unindent(16.0f);
            ImGui::Columns(1);

        }
    }

    ImGui::EndChild();
}

void buttonsWindow() {

    TCHAR ControliniPath[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, ControliniPath);
    PathAppendA(ControliniPath, (char*)"2EZ.ini");

    char buff[10];

    input::InitJoysticks();

    ImGui::BeginChild("buttons", { 0, ImGui::GetWindowHeight() - 85 }, false, ImGuiWindowFlags_HorizontalScrollbar);

    input::JoystickState joystates[15];

    for (int i = 0; i < 15; i++) {
        joystates[i] = input::GetJoyState(i);
    }

    //BEGIN IO BINDING
    ImGui::Text("IO Input");
    ImGui::SameLine();
    HelpMarker("This is the binding for the IO Hook");
    ImGui::Columns(3, "buttons");
    ImGui::Text("Button");
    ImGui::NextColumn();
    ImGui::Text("Binding");
    ImGui::NextColumn();
    ImGui::Text("Action");
    ImGui::Separator();
    ImGui::NextColumn();

    for (int i = 0; i < IM_ARRAYSIZE(ioButtons); i++) {

        char method[20];
        GetPrivateProfileStringA(ioButtons[i], "Method", "", method, sizeof(method), ControliniPath);
        int id = GetPrivateProfileIntA(ioButtons[i], "JoyID", NULL, ControliniPath);
        int binding = GetPrivateProfileIntA(ioButtons[i], "Binding", NULL, ControliniPath);
        if (binding != NULL) {
            if (strcmp(method, "Joy") == 0) {
                if (joystates[id].buttonsPressed & binding && joystates[id].input) {
                    ImGui::TextColored(ImVec4(1, 0.7f, 0, 1), ioButtons[i]);
                }
                else {
                    ImGui::Text(ioButtons[i]);
                }
                ImGui::NextColumn();
                if (joystates[id].input) {
                    ImGui::Text("%sID:%d Btn:%s", method, id, input::getButtonName(binding).c_str());
                }
                else {
                    ImGui::Text("Device Removed");
                }
            }
            else {
                if (GetAsyncKeyState(binding) & 0x8000) {
                    ImGui::TextColored(ImVec4(1, 0.7f, 0, 1), ioButtons[i]);
                }
                else {
                    ImGui::Text(ioButtons[i]);
                }

                ImGui::NextColumn();
                /*  char name[255];
                  UINT scanCode = MapVirtualKeyExA(binding, MAPVK_VK_TO_VSC_EX, NULL);
                  LONG lParamValue = (scanCode << 16);
                  int result = GetKeyNameTextA(lParamValue, name, 255);*/
                ImGui::Text("%s:%s", method, GetKeyName(binding).c_str());
            }
        }
        else {
            ImGui::Text(ioButtons[i]);
            ImGui::NextColumn();
            ImGui::BeginDisabled();
            ImGui::Text("None");
            ImGui::EndDisabled();
        }


        //BINDING CODE THIS IS SHIT I HATE IT
        ImGui::NextColumn();
        char buttonLabel[20];
        sprintf(buttonLabel, "Bind##%d", i);
        if (ImGui::Button(buttonLabel))
            ImGui::OpenPopup(ioButtons[i]);
        if (ImGui::BeginPopupModal(ioButtons[i], NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Press a button to bind it");
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            else {
                for (int n = 0; n < joyGetNumDevs(); n++) {
                    if (input::GetJoyState(n).input && input::GetJoyState(n).NumPressed == 1) {
                        WritePrivateProfileString(ioButtons[i], "Method", "Joy", ControliniPath);
                        WritePrivateProfileString(ioButtons[i], "JoyID", _itoa(n, buff, sizeof(buff)), ControliniPath);
                        WritePrivateProfileString(ioButtons[i], "Binding", _itoa(input::GetJoyState(n).buttonsPressed, buff, sizeof(buff)), ControliniPath);
                        ImGui::CloseCurrentPopup();
                    }
                }

                // grab current keyboard state
                int key = input::checkKbPressedState();
                if (key > 0) {
                    WritePrivateProfileString(ioButtons[i], "Method", "Key", ControliniPath);
                    WritePrivateProfileString(ioButtons[i], "JoyID", NULL, ControliniPath);
                    WritePrivateProfileString(ioButtons[i], "Binding", _itoa(key, buff, sizeof(buff)), ControliniPath); \
                        ImGui::CloseCurrentPopup();
                }

            }
            ImGui::EndPopup();
            //PLEASE SOMEONE REPLCE IT WITH RAWINPUT OF SOMETHING
        }

        if (binding != NULL) {
            //Clear Binding
            ImGui::SameLine();
            sprintf(buttonLabel, "Clear##%d", i);
            if (ImGui::Button(buttonLabel)) {
                WritePrivateProfileString(ioButtons[i], NULL, NULL, ControliniPath);
            }
        }
        ImGui::NextColumn();

    }


    ImGui::Columns(1);
    ImGui::Separator();



    //BEGIN DEV BINDING
    ImGui::Text("Dev Input (Keyboard Only)");
    ImGui::SameLine();
    HelpMarker("Some EZ2 exe's have dev controls, these  bindings will overwrite them");
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Unbind or disable dev input binding if intending");
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "to use Real IO or the IO shim.");
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Developer Bindings disable the IO equivelent and this");
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "behaviour cannot be changed.");
    ImGui::Columns(3, "buttons");
    ImGui::Separator();


    for (int i = 0; i < IM_ARRAYSIZE(devButtons); i++) {

        char method[20];
        GetPrivateProfileString(devButtons[i], "Method", "", method, sizeof(method), ControliniPath);
        int id = GetPrivateProfileIntA(devButtons[i], "JoyID", NULL, ControliniPath);
        int binding = GetPrivateProfileIntA(devButtons[i], "Binding", NULL, ControliniPath);
        if (binding != NULL) {
            if (GetAsyncKeyState(binding) & 0x8000) {
                ImGui::TextColored(ImVec4(1, 0.7f, 0, 1), devButtonsDisplay[i]);

            }
            else {
                ImGui::Text(devButtonsDisplay[i]);
            }
            ImGui::NextColumn();
            ImGui::Text("%s:%s", method, GetKeyName(binding).c_str());
        }
        else {
            ImGui::Text(devButtonsDisplay[i]);
            ImGui::NextColumn();
            ImGui::BeginDisabled();
            ImGui::Text("None");
            ImGui::EndDisabled();
        }


        //BINDING CODE THIS IS SHIT I HATE IT
        ImGui::NextColumn();
        char buttonLabel[20];
        sprintf(buttonLabel, "Bind##%d", i + 100);
        if (ImGui::Button(buttonLabel))
            ImGui::OpenPopup(devButtons[i]);
        if (ImGui::BeginPopupModal(devButtons[i], NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Press a button to bind it");
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            else {
                // grab current keyboard state
                int key = input::checkKbPressedState();
                if (key > 0) {
                    WritePrivateProfileString(devButtons[i], "Method", "Key", ControliniPath);
                    WritePrivateProfileString(devButtons[i], "JoyID", NULL, ControliniPath);
                    WritePrivateProfileString(devButtons[i], "Binding", _itoa(key, buff, sizeof(buff)), ControliniPath);
                        ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
            //PLEASE SOMEONE REPLCE IT WITH RAWINPUT OF SOMETHING
        }

        if (binding != NULL) {
            //Clear Binding
            ImGui::SameLine();
            sprintf(buttonLabel, "Clear##%d", i + 100);
            if (ImGui::Button(buttonLabel)) {
                WritePrivateProfileString(devButtons[i], NULL, NULL, ControliniPath);
            }
        }
        ImGui::NextColumn();

    }

    ImGui::Columns(1);
    input::DestroyJoysticks();
    ImGui::EndChild();
}

void analogsWindow() {

    TCHAR ControliniPath[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, ControliniPath);
    PathAppendA(ControliniPath, (char*)"2EZ.ini");

    input::InitJoysticks();

    ImGui::BeginChild("analogs", { 0,  ImGui::GetWindowHeight() - 85 }, false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::Columns(3, "analogs");
    ImGui::Text("Analog");
    ImGui::NextColumn();
    ImGui::Text("Binding");
    ImGui::NextColumn();
    ImGui::Text("Action");
    ImGui::Separator();
    ImGui::NextColumn();



    for (int i = 0; i < IM_ARRAYSIZE(analogs); i++) {
        int id;
        char axis[20];
        bool reverse = GetPrivateProfileIntA(analogs[i], "Reverse", 0, ControliniPath);


        id = GetPrivateProfileInt(analogs[i], "JoyID", 16, ControliniPath);
        GetPrivateProfileStringA(analogs[i], "Axis", NULL, axis, sizeof(axis), ControliniPath);

        float fraction = 0.0f;
        if (id != 16 && input::GetJoyState(id).input) {
            if (strcmp(axis, "X") == 0) {
                if (reverse) {
                    UINT8 val = ~(input::GetJoyState(id).x);
                    fraction = (float)val / (float)255;
                }
                else {
                    fraction = (float)(input::GetJoyState(id).x) / (float)255;
                }
            }
            else if (strcmp(axis, "Y") == 0) {
                if (reverse) {
                    UINT8 val = ~(input::GetJoyState(id).y);
                    fraction = (float)val / (float)255;
                }
                else {
                    fraction = (float)input::GetJoyState(id).y / (float)255;

                }
            }
        }
        ImGui::ProgressBar(fraction, ImVec2(30.0f, 0.0f));
        ImGui::SameLine();
        ImGui::Text(analogs[i]);


        // Binding Info
        ImGui::NextColumn();

        char method[20];


        if (id < 16) {
            ImGui::Text("Joy:%d axis:%s", id, axis);
        }


        // BINDING CODE THIS IS SHIT I HATE IT
        ImGui::NextColumn();
        char buttonLabel[20];
        sprintf(buttonLabel, "Bind##%d", i);
        if (ImGui::Button(buttonLabel))
            ImGui::OpenPopup(analogs[i]);
        if (ImGui::BeginPopupModal(analogs[i], NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            char formatJoy[20];
            sprintf(formatJoy, "##%d", i + 1 * 2);
            char selectedJoy[20];
            if (id < 16) {
                sprintf(selectedJoy, "Joystick%d", id);

            }
            else {
                sprintf(selectedJoy, "Set Joystick");
            }
            if (ImGui::BeginCombo(formatJoy, selectedJoy))
            {
                if (ImGui::Selectable("None")) {
                    char buff[10];
                    WritePrivateProfileString(analogs[i], "JoyID", _itoa(16, buff, sizeof(buff)), ControliniPath);
                }
                for (int n = 0; n < joyGetNumDevs(); n++)
                {
                    input::JoystickState joystate = input::GetJoyState(n);
                    if (joystate.input) {
                        bool is_selected = (n == id);
                        sprintf(selectedJoy, "Joystick%d", n);
                        if (ImGui::Selectable(selectedJoy, is_selected)) {
                            id = n;
                            char buff[10];
                            WritePrivateProfileString(analogs[i], "JoyID", _itoa(n, buff, sizeof(buff)), ControliniPath);
                        }
                        if (id == n) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }


                }
                ImGui::EndCombo();
            }
            char format[20];
            char selected[20];
            sprintf(format, "##%d", i + 1);
            if (strcmp(axis, "") != 0) {
                sprintf(selected, "axis: %s", axis);
            }
            else {
                sprintf(selected, "Set Axis");
            }
            if (ImGui::BeginCombo(format, selected))
            {

                if (ImGui::Selectable("Axis: X")) {
                    char buff[10];
                    WritePrivateProfileString(analogs[i], "Axis", "X", ControliniPath);
                }

                if (ImGui::Selectable("Axis: Y")) {
                    char buff[10];
                    WritePrivateProfileString(analogs[i], "Axis", "Y", ControliniPath);
                }

                ImGui::EndCombo();
            }

            ImGui::Checkbox("Invert Axis", &reverse);
            char buff[33];
            WritePrivateProfileString(analogs[i], "Reverse", _itoa(reverse, buff, 10), ControliniPath);


            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        if (id < 16) {
            //Clear Binding
            ImGui::SameLine();
            sprintf(buttonLabel, "Clear##%d", i + 100);
            if (ImGui::Button(buttonLabel)) {
                WritePrivateProfileString(analogs[i], NULL, NULL, ControliniPath);
            }
        }
        ImGui::NextColumn();

    }

    ImGui::Columns(1);
    input::DestroyJoysticks();
    ImGui::EndChild();
}

void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


int detectGameVersion() {
    TCHAR fullPath[MAX_PATH];
    TCHAR driveLetter[3];
    TCHAR directory[MAX_PATH];
    TCHAR FinalPath[MAX_PATH];
    DIR* dir;
    struct dirent* ent;
    unsigned char result[MD5_DIGEST_LENGTH];

    //get current directory
    GetModuleFileName(NULL, fullPath, MAX_PATH);
    _splitpath(fullPath, driveLetter, directory, NULL, NULL);
    sprintf(FinalPath, "%s%s", driveLetter, directory);


    if ((dir = opendir(FinalPath)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            
            if (strcmp(ent->d_name, "2EZConfig.exe") == 0 || !hasEnding(toLower(ent->d_name), ".exe")) {
                printf("Skipping %s\n", ent->d_name);
                continue;
            }
            printf("Comapring %s\n", ent->d_name);

            FILE* inFile = fopen(ent->d_name, "rb");
            MD5_CTX mdContext;
            int bytes;
            unsigned char data[1024];


            //cannot open file, continute to next.
            if (inFile == NULL) {
                continue;
            }

            //generate file MD5
            MD5_Init(&mdContext);
            while ((bytes = fread(data, 1, 1024, inFile)) != 0) {
                MD5_Update(&mdContext, data, bytes);
            }
            MD5_Final(result, &mdContext);
            fclose(inFile);

            //COMPARE WITH KNOWN MD5's and return first match
            for (int i = 0; i < IM_ARRAYSIZE(djGames); i++) {
                if (memcmp(result, djGames[i].md5, MD5_DIGEST_LENGTH) == 0) {
                    printf("MD5 Matches: %s", djGames[i].name);
                    return i;
                };
            }
            printf("No Match.. \n");
        }
        closedir(dir);
    }

    // could not open directory set to n-1
    return IM_ARRAYSIZE(djGames) - 2;
}







