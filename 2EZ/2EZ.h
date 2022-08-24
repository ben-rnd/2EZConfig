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

DWORD byteArrayDancerFeet[4] = {0b111111110000,  //LEFT
								0b111100001111,  //CENTER 
								0b000011111111 };//RIGHT 

							       //         L Hand Top         L Hand Bottom        R Hand Top	     R Hand Bottom
DWORD byteArrayDancerHands[10] = { /*P1*/ 0b1111011111111111, 0b1110111111111111, 0b1111101111111111, 0b1101111111111111,
								   /*P2*/ 0b1111110111111111, 0b1011111111111111, 0b1111111011111111, 0b0111111111111111,
								   //            Test              Service   
						      /*Service*/ 0b1111111100100000, 0b1111111100010000 };//, Coin: 0b1111111100000100



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
static struct game {
	const char* name;
	bool baseAddressOverride;
	uintptr_t devOffset;
	uintptr_t modeTimeOffset;
	uintptr_t songTimerOffset;
	bool isDjGame;
} games[] = {
	{"EZ2Dancer The 1st MOVE",false, 0x00, 0x00, 0x00, false},
	{"EZ2Dancer The 2nd MOVE",false, 0x00, 0x00, 0x00, false}, //no dump
	{"EZ2Dancer UK MOVE ",false, 0x00, 0x00, 0x00, false},
	{"EZ2Dancer UK MOVE Special Edition", false, 0x00, 0x00, 0x00, false}, //no dump
	{"EZ2Dancer SuperChina",false, 0x00, 0x00, 0x00, false}, //no dump
	{"The 1st Tracks", false, 0x00, 0x00, 0x00, true}, //no dump
	{"The 1st Tracks Special Edition", false, 0x00, 0x00, 0x00, true},
	{"2nd Trax ~It rules once again~", false, 0x00, 0x00, 0x00, true}, //no dump
	{"3rd Trax ~Absolute Pitch~ ", false, 0x00, 0x00, 0x00, true},
	{"4th Trax ~OVER MIND~ ", false, 0x00, 0x00, 0x00, true},
	{"Platinum", false, 0x00, 0x00, 0x00, true },
	{"6th Trax ~Self Evolution~", false, 0x00, 0x00, 0x00, true},
	{"7th Trax ~Resistance~", false, 0x00, 0x00, 0x00, true},
	{"7th Trax Ver 1.5", false, 0x00, 0x00, 0x00, true},
	{"7th Trax Ver 2.0", false, 0x00, 0x00, 0x00, true},
	{"Codename: Violet", false, 0x00, 0x00, 0x00, true},
	{"Bonus Edition", false, 0x00, 0x00, 0x00, true}, //no dump
	{"Bonus Edition revision A", false, 0x00, 0x00, 0x00, true},
	{"Azure ExpressioN", false, 0x00, 0x00, 0x00, true}, //no dump
	{"Azure ExpressioN Integral Composition", false, 0x00, 0x00, 0x00, true}, //no dump
	{"Endless Circulation", false, 0xEFD4CC, 0x5978A, 0x53289, true}, //no good dump 
	{"Evolve (Win98)", false, 0x00, 0x00, 0x00, true},
	{"Evolve", false, 0x944C9C, 0x3A0DE, 0x38257, true},
	{"Night Traveller", true, 0xF0FE14, 0x42A6D, 0x41854, true},
	{"Time Traveller (1.83)", true, 0x1309D74, 0x4740A, 0x45F6C, true},// need updated dump
	{"Final", false, 0x130CFB4, 0x4A693, 0x46C75, true},
	{"Final:EX", true, 0x130DFFC, 0x4B773, 0x47CBA, true},
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

static const char* ez2DancerIOButtons[] = { 


	//Should we split the foot buttons out to the 4 sensors each? 
	"P1 Left",
	"P1 Centre",
	"P1 Right",

	"P2 Left",
	"P2 Centre",
	"P2 Right",

	"P1 L Sensor Top",
	"P1 L Sesor Bottom",
	"P1 R Sensor Top",
	"P1 R Sesor Bottom",

	"P2 L Sensor Top",
	"P2 L Sesor Bottom",
	"P2 R Sensor Top",
	"P2 R Sesor Bottom",
	
	"Test",
	"Service"
	//"Coin", //IO does this really weird.. will leave out for now
};


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




