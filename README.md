<div align="center">

<img src="https://i.imgur.com/rMxVH3j.png" width="35%" height="35%"></img>


Welcome to oxWARE, a free Counter-Strike 1.6 cheat. This cheat is still in <b>alpha stage</b> of development!<br>
I will be contributing to it in the near future as much as possible, adding new features et cetera.<br>
So stay tuned! 😉<br><br>

Please, before proceeding, **read this README**!

</div>

---

- [Usage](#usage)
- [Multi-platform support](#multi-platform-support)
- [Anti-cheats](#anti-cheats)
	- [VAC](#vac)
- [Building](#building)
- [Features](#features)
- [To be done](#to-be-done)

## Usage

The cheat is composed out of two parts: **the loader (oxware.exe)** and **the cheat module (cheater.dll)**. By using the loader you can inject the cheat module to the game. After injecting the cheat, you can use **F1** key to open up the menu. 

Both the cheat and loader have developer consoles built inside of them that you can access at any time. (inside the cheat it is the '~' key, below the escape key)

![The loader on the left, cheat menu ingame on the right](https://i.imgur.com/27Lgnsg.png)


# Multi-platform support

This cheat is currently only available on **Windows**. `TODO: versions`

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
- viewmodel offset
- smoke visuals
- flashbang fade modifier
- fov changer
- esp
	- player
	- entities
	- sound
- model chams
	- viewmodel
	- players (T and CT)
- bypassing game constrains
	- re-enabling noclip (normally disabled in CS 1.6)
	- custom render distance
	- re-enabling some renderer cvars (normally unable to set in SP & MP)
	- allow SP-only cvars also in MP
- configuration files

## To be done

All of the things that are left to be done are inside our [trello board](https://trello.com/b/mmVAu6Gt/oxware).
