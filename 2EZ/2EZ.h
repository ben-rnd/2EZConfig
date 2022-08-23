#pragma  once
#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <iostream>
#pragma comment( lib, "psapi.lib" )

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))
#define NUM_OF_IO 24
#define NUM_OF_ANALOG 2
#define NUM_OF_JOYSTICKS 15

DWORD byteArray[8] = { 0b01111111, 0b10111111, 0b11011111,0b11101111,0b11110111,0b11111011,0b11111101, 0b11111110 };

//ez2dancer panels have 4 sensors each, and rather than the IO reading each of these individually and determining a press
//the game receives all 4 states and determines it itself. For Accuracy sake, i think all sensors should be mappable
//But for now will just combine the inputs.
//DWORD byteArrayDancer[12] = {0b011111111111, 0b101111111111, 0b110111111111, 0b111011111111
//							   0b111101111111, 0b111110111111, 0b111111011111, 0b111111101111
//							   0b111111110111, 0b111111111011, 0b111111111101, 0b111111111110};

DWORD byteArrayDancer[4] = {0b000011111111, 
							0b111100001111, 
							0b111111110000,
							0b1111111111111111};

DWORD byteArrayDancerService[12] = {0b1111011111111111, 0b1111101111111111, 0b1101111111111111,0b1110111111111111, 
									0b1111110111111111, 0b1111111011111111, 0b0111111111111111, 0b1011111111111111,  
									0b00000001111011111, 0b0000000011101111, 0b0000000011111011, 0b0000000011111111, };



typedef struct IOBinding {
	bool bound = false;
	int method = 3; // 0 = kb 1= joypad
	int joyID = 16;
	DWORD binding = NULL;
	bool pressed = false;
} ioBinding;

typedef struct IOAnalogs {
	bool bound = false;
	int joyID = 16; //must be 15 or below for valid
	int axis = 3;   //0=X  1=Y
	UINT8 pos = 255;
	bool reverse = false;
} ioAnalogs;

typedef struct JoySticks {
	bool init = false;
} Joysticks;


//Easily expandable array with settings for common game hacks,
//Special game hacks such as save setting with need a game specific function due to how differently theyre handled in each game
//I do not plan no expanding that anytime soon anyway, gathering the offsets was annoying.

//MAKE SURE THIS ARRAY ALWAYS MATCHES THE 2EZCONFIG djGames ARRAY ORDER & SIZE
#define OFFSET_SIZE 10
static struct djGame {
	const char* name;
	bool baseAddressOverride;
	uintptr_t devOffset;
	uintptr_t modeTimeOffset;
	uintptr_t songTimerOffset;
} djGames[] = {
	{"The 1st Tracks", false, 0x00, 0x00, 0x00}, //no dump
	{"The 1st Tracks Special Edition", false, 0x00, 0x00, 0x00},
	{"2nd Trax ~It rules once again~", false, 0x00, 0x00, 0x00}, //no dump
	{"3rd Trax ~Absolute Pitch~ ", false, 0x00, 0x00, 0x00},
	{"4th Trax ~OVER MIND~ ", false, 0x00, 0x00, 0x00},
	{"Platinum", false, 0x00, 0x00, 0x00 },
	{"6th Trax ~Self Evolution~", false, 0x00, 0x00, 0x00},
	{"7th Trax ~Resistance~", false, 0x00, 0x00, 0x00},
	{"7th Trax Ver 1.5", false, 0x00, 0x00, 0x00},
	{"7th Trax Ver 2.0", false, 0x00, 0x00, 0x00},
	{"Codename: Violet", false, 0x00, 0x00, 0x00},
	{"Bonus Edition", false, 0x00, 0x00, 0x00}, //no dump
	{"Bonus Edition revision A", false, 0x00, 0x00, 0x00},
	{"Azure ExpressioN", false, 0x00, 0x00, 0x00}, //no dump
	{"Azure ExpressioN Integral Composition", false, 0x00, 0x00, 0x00}, //no dump
	{"Endless Circulation", false, 0xEFD4CC, 0x5978A, 0x53289}, //no good dump 
	{"Evolve (Win98)", false, 0x00, 0x00, 0x00},
	{"Evolve", false, 0x944C9C, 0x3A0DE, 0x38257},
	{"Night Traveller", true, 0xF0FE14, 0x42A6D, 0x41854},
	{"Time Traveller (1.83)", true, 0x1309D74, 0x4740A, 0x45F6C},// need updated dump
	{"Final", false, 0x130CFB4, 0x4A693, 0x46C75},
	{"Final:EX", true, 0x130DFFC, 0x4B773, 0x47CBA},
};

const char* devButtons[] = { "dQuit", "dTest", "dService", "dCoin","dP1 Start", "dP2 Start", "dEffector 1", 
                             "dEffector 2", "dEffector 3", "dEffector 4","dP1 1", "dP1 2", "dP1 3", "dP1 4", "dP1 5", 
                             "dP1 TT+", "dP1 TT-","dP1 Pedal", "dP2 1", "dP2 2", "dP2 3", "dP2 4", "dP2 5",
                             "dP2 TT+", "dP2 TT-", "dP2 Pedal" };

const char* ioButtons[] = { "Test", "Service", "Effector 4", "Effector 3", "Effector 2", "Effector 1",
						   "P2 Start", "P1 Start","P1 Pedal","QE", "QE", "P1 5", "P1 4", "P1 3", "P1 2", "P1 1",
						   "P2 Pedal","QE", "QE", "P2 5", "P2 4", "P2 3", "P2 2", "P2 1" };

const char* analogs[] = { "P1 Turntable", "P2 Turntable" };

const char* lights[] =  {"Effector 1", "Effector 2", "Effector 3", "Effector 4", "P1 Start", "P2 Start",
                        "P2 Turntable", "P2 1", "P1 2", "P1 3", "P1 4", "P1 5", "P1 Turntable", "P2 1", "P2 2",
                        "P2 3", "P2 4", "P2 5", "Neons", "Red Lamp L", "Red Lamp R", "Blue Lamp L", "Blue Lamp R" };



void ChangeMemory(uintptr_t baseaddress, int value, uintptr_t offset, bool isProtected)
{
	unsigned long OldProtection;
	VirtualProtect((LPVOID)(baseaddress + offset), sizeof(value), PAGE_EXECUTE_READWRITE, &OldProtection);
    *(BYTE*)(baseaddress + offset) = value;
	VirtualProtect((LPVOID)(baseaddress + offset), sizeof(value), OldProtection, NULL);
}


void zeroMemory(uintptr_t baseaddress, uintptr_t offset)
{
	unsigned long OldProtection;
	VirtualProtect((LPVOID)(baseaddress + offset), 8, PAGE_EXECUTE_READWRITE, &OldProtection);
	*(BYTE*)(baseaddress + offset) = 0x00;
	VirtualProtect((LPVOID)(baseaddress + offset), 8, OldProtection, NULL);

}

void NOPMemory(uintptr_t baseaddress, uintptr_t offset)
{
	unsigned long OldProtection;
	VirtualProtect((LPVOID)(baseaddress + offset), 8, PAGE_EXECUTE_READWRITE, &OldProtection);
	*(BYTE*)(baseaddress + offset) = 0x90;
	VirtualProtect((LPVOID)(baseaddress + offset), 8, OldProtection, NULL);

}

//each game felt too different to have a common function for this
void fnNOPResetCode(uintptr_t baseAddress, uintptr_t resetAddresses) {
	for (int z = 0; z < 5; z++) {
		NOPMemory(baseAddress, resetAddresses + z);
	}
}

void setDevBinding(uintptr_t baseAddress, uintptr_t startAddress, LPCSTR bindings) {
	DWORD inputOffset = 0x14;
	for (int i = 0; i < IM_ARRAYSIZE(devButtons); i++) {
		//if binding is not this crap, set binding
		int binding = GetPrivateProfileIntA(devButtons[i], "Binding", 0, bindings);
		if (binding != 0) {

			//sets the key that will be checked by the getAsyncKeypress call in the ez2 exe  
			//this is why its kb binding only, theres no point around allowing button bidning as itll just be a joy2key type solution
			//written into the DLL, users should just use the IO binding instead
			ChangeMemory(baseAddress, binding, startAddress + (inputOffset * i), false);

			//Setting this address to 0 enables the key - but disabled the IO equiv
			//Known values:
			//0 = Enabled  - Disable IO
			//1 = ??
			//2 = ??
			//3 = Disabled - use IO
			ChangeMemory(baseAddress, 0, startAddress + (inputOffset * i) - 4, false);
		}
	}
}


//experimental save settings patch, crashes lots of game modes :)
void FnKeepSettings(uintptr_t baseAddress) {			//Random  //Note    //Auto    /Fade     /Scroll  //Visual	//Panel
	DWORD resetAddresses[7] = {0x30C0F, 0x30C23, 0x30C46, 0x30C0A, 0x30C19, 0x30BEC, 0x30C87};
	DWORD Panel = 0x33D0F3C;
	DWORD Note = 0x33D0EC4;
	DWORD Visual = 0x33D0EF0;
	int defaultNote = GetPrivateProfileIntA("KeepSettings", "DefaultNote", 7, ".\\2EZ.ini");
	int defaultPanel  = GetPrivateProfileIntA("KeepSettings", "DefaultPanel", 0, ".\\2EZ.ini");
	int defaultVisual = GetPrivateProfileIntA("KeepSettings", "DefaultVisual", 0, ".\\2EZ.ini");

	if (GetPrivateProfileIntA("KeepSettings", "Random", 1, ".\\2EZ.ini")) {
		fnNOPResetCode(baseAddress, resetAddresses[0]);
	}

	if (GetPrivateProfileIntA("KeepSettings", "Note", 1, ".\\2EZ.ini")) {
		fnNOPResetCode(baseAddress, resetAddresses[1]);
		ChangeMemory(baseAddress, defaultNote, Note, false);
	}

	if (GetPrivateProfileIntA("KeepSettings", "Auto", 1, ".\\2EZ.ini")) {
		fnNOPResetCode(baseAddress, resetAddresses[2]);
	}

	if (GetPrivateProfileIntA("KeepSettings", "Fade", 1, ".\\2EZ.ini")) {
		fnNOPResetCode(baseAddress, resetAddresses[3]);
	}

	if (GetPrivateProfileIntA("KeepSettings", "Scroll", 1, ".\\2EZ.ini")) {
		fnNOPResetCode(baseAddress, resetAddresses[4]);
	}

	if (GetPrivateProfileIntA("KeepSettings", "Visual", 1, ".\\2EZ.ini")) {
		fnNOPResetCode(baseAddress, resetAddresses[5]);
		ChangeMemory(baseAddress, defaultVisual, Visual, false);
	}

	if (GetPrivateProfileIntA("KeepSettings", "Panel", 1, ".\\2EZ.ini")) {
		fnNOPResetCode(baseAddress, resetAddresses[6]);
		ChangeMemory(baseAddress, defaultPanel, Panel, false);
	}

}




