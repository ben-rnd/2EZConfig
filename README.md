# 2EZProject
disclaimer: This project was migrated from dev.s-ul.eu gitlab ni 2023

This is a tool designed to enable USB controllers to work with all versions of EZ2DJ/AC, aswell as offering various patches and hacks to make the home experience more enjoyable. The motivation behind this tool is for the preservation of the most important gamemode of all time: EZ2Catch.

The tool was initally a small personal project that was hacked together in a few nights after frustration when playing EC with joy2key, it was released due to the attention that EZ2 had been gaining with the public release of final, so i apologise in advance for some awful programming. Now that its open source please feel free to improve it! :D

## Current Compatibility and Features

All EZ2DJ/AC games support this method of IO input emulation.

### Patches 
All Patches are applied in memory after launching the game, and make no modification to original files.
#### Endless Circulation -> FNEX
- Enable and bind deveveloper inputs.
- Timer freeze patches
#### FN
- "Keep Settings" patch. An experimental patch that persists your game settings set between credits - Only stable when playing 5k and 7k Standard.
	

## Get Involved!
Very open to contributions or suggestions, so please get involved to improve the tool :). 
### Current Priority
 - HID Integration for greater compatibilty with input devices such as midi (and also hid lights out), dropping the need for the shit windows JoyAPI.

### WIN XP COMPATIBILTY IS A MUST.
All contributions will be tested on a EZ2AC winxp install with hardware matching that of an EZ2 PC.

## Building
All external library files should be included and pre linked so building should be straight forward. 
Requires the Visual studio 2015 - Windows XP(v140_XP) Platform toolset.

## Roadmap
- HID Input, support for various input device (eg.MIDI)
- HID Lighting out
- EZ2Dancer support - IOhooking done, just need to RE the button mappings on ports and create config interface for it (kind of done, check out the ez2dancer branch)
- Increase library of game patches

## Contact

Please feel free to contact me on sows, or through my discord @ kissAss#0744



