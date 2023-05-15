<div align="center">

<img src="https://i.imgur.com/rMxVH3j.png" width="27%" height="27%"></img>


Welcome to **oxWARE**, a free Counter-Strike 1.6 cheat. This cheat is still in <b>alpha stage</b> of development!<br>
I will be contributing to it in the near future as much as possible, adding new features et cetera.<br>
So stay tuned! 😉<br><br>

Please, before proceeding, **read this README**!

</div>

---

## Table of contents

- [Special thanks to](#special-thanks-to)
- [Usage](#usage)
- [Multi-platform support](#multi-platform-support)
- [Support on non-steam and older versions of the game](#support-on-non-steam-and-older-versions-of-the-game)
- [Anti-cheats](#anti-cheats)
	- [VAC](#vac)
- [Building](#building)
- [Features](#features)
- [To be done](#to-be-done)
- [Changelog](#changelog)

## Special thanks to

- [phnt](https://github.com/processhacker/phnt) for providing great collection of native Windows kernel API, definitions, and more.
- [Microsoft Detours](https://github.com/microsoft/Detours) for their Detouring open-source library.
- [GLFW](https://github.com/glfw/glfw) for their robust open-source OpenGL library.
- [imgui](https://github.com/ocornut/imgui) for literally the best light-weight GUI out there.
- [json](https://github.com/nlohmann/json) for the very mature json library.
- [soil](https://github.com/littlstar/soil) for its functionality over simplifying the process of loading opengl images.
- Voooolve for making an excelent decision of building their linux build of the game with debug symbols, making it easier to make cheats for their game 😉 ... Thanks Valve.
- Anyone who plays using this cheat client or contributes to it somehow. 😎

## Usage

The cheat is composed out of two parts: **the loader (oxware.exe)** and **the cheat module (cheater.dll)**. By using the loader you can inject the cheat module to the game. After injecting the cheat, you can use **INSERT** key to open up the menu. 

Both the cheat and loader have developer consoles built inside of them that you can access at any time. (inside the cheat it is the '~' key, below the escape key)

## Cheat menu in-game

![The cheat](https://i.imgur.com/KcBhkEW.png)

## The loader

![The loader](https://i.imgur.com/j0hOePU.png)

# Support on other operating systems

This cheat is currently only available on `Windows 10 and newer`.

# Support on non-steam and older versions of the game

This cheat currently supports following builds:

- `8684 steam version`
- `4554 non-steam version`
- xguard, gsclient or other custom implementations **aren't supported**.

## Anti-cheats

The cheat wasn't tested on any anticheats so far. It may or may not be detected. **USE AT YOUR OWN RISK**!

### VAC

As far as I know, the CS 1.6 VAC is not so smart, so **YOU SHOULD NOT GET VAC BANNED BY USING THIS**, but having said that, **USE STILL AT YOUR OWN RISK!**

## Building

This cheat is currently only available to be built inside the **Visual Studio** environment, any other building software haven't been tested or added yet.

In order to build, you will need **Visual Studio 2022** with **C++20 support**, and **v143 toolset**.

There are three main build configurations:

- **debug**: for debugging and development.

- **release**: for testing retail-like circumstances without the debug overhead.

- **retail**: for final deployment only.

In order to fully build everything, all you need to do is just hit **ctrl + shift + b** or navigate to the solution and select **Rebuild Solution**. New **build\\** directory will be created and all of the executables will be placed here.

**Also see building.md in the root directory of this repo!**

## Features

- Removals
	- screen shake
- HUD removals
	- weapon, crosshair, money, etc...
- viewmodel offset
- smoke visuals, see [this video](https://youtu.be/PL2f-weiF84). 🚬
- flashbang fade modifier
- built-in console with custom commands 🖥
- fov changer
- server liar (sending false cvar values to the server)
- skeletal chams 💀
- playermodel head hitbox rendering, see [this image](https://i.imgur.com/bYzp8KD.png). 🟩
- server command filter (svc_stufftext filter)
- ability to view "real" hit-able playermodel and its head hitbox. See [this image](https://i.imgur.com/weWGM5C.png) and [this youtube video](https://youtu.be/xMd9m3McNvo).
- esp
	- player
	- entities
	- sound
	- custom box types
- model chams
	- viewmodel
	- players (T and CT)
	- rainbow colors
	- flat/shaded
	- disabled playermodel animations
- bypassing game constrains
	- re-enabling noclip (normally disabled in CS 1.6)
	- custom render distance
	- re-enabling some renderer cvars (normally unable to set in SP & MP)
	- allow SP-only cvars also in MP
- Frame skipping/simulated framerate
- Vanilla-like crosshair changer ➕
- Custom HUD rendering 🌈 such as:
	- displaying currently held weapon 🔫
	- displaying current velocity 
- Default viewmodel enforcement
- rechecker bypass
- instantaneous cheat unload (either via loader or cheat menu)
- injection using manual mapping 🗺
- custom UI (modified version of imgui)
- key binding
- configuration files ⚙

## To be done

All of the things that are left to be done are inside our [trello board](https://trello.com/b/mmVAu6Gt/oxware).

## Changelog

See [changelog.md](https://github.com/oxiKKK/oxware/blob/main/changelog.md) for more information.
