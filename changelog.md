# v1.0.0

- Initial cheat release!

# v1.0.1

## Additions and improvements

- Added link to github repository to the bottom right corner of the screen (when menu is up).
- Added custom sprite and HUD rendering, including the option to change HUD colors. 🌈
- Added command support and new prompt inside console for entering commands. See [this commit](https://github.com/oxiKKK/oxware/commit/0824fefd17c17c2a74621c15e5dea72aeea0e916). Some of the commands are:
	- "variable_list"
	- "command_list"
	- "clear"
	- "help"
- Added Server liar that allows you to lie to the server. (more information on that in the cheat menu)
- Added hud_render_velocity variable to display player velocity on the screen using vanilla sprites.
- Improvements to various error dialogs.

## Bugfixes

- Added check to see whenever the user is using the supported Windows version.
- Fixed mouse cursor stuck issue. ([#0001](https://github.com/oxiKKK/oxware/issues/1))
- Added support for C++ exceptions when using manual mapping. See [this commit](https://github.com/oxiKKK/oxware/commit/c29da51696fd0c3c034d821f8261cba139566bfa) for more details.
- Various crash fixes.

# v1.0.2

## Additions and improvements

- Added vanilla-like crosshair changer! ➕
- Implemented skeletal playermodel chams. 💀
- Implemented playermodel head hitbox rendering of greenish color. 🟩

<details><summary>Spoiler</summary>
	<img src="https://i.imgur.com/bYzp8KD.png" width="50%" height="50%"></img>
</details><br>

- Added rainbow model chams! 🌈
- Additions to the ESP.
- Implemented no animations on player models.
- Added fade when opening/closing the menu.
- Added rain to the background of the menu.

<details><summary>Spoiler</summary>
	<img src="https://i.imgur.com/LqvACY8.png" width="50%" height="50%"></img>
</details><br>

- Added svc_stufftext filter (server command filter). See [this video](https://youtu.be/4u0LNqQbY4w) for demonstration.
- Added the ability to view "real" hit-able playermodel + its head hitbox. See [this youtube video](https://youtu.be/xMd9m3McNvo).

<details><summary>Spoiler</summary>
	<img src="https://i.imgur.com/weWGM5C.png" width="50%" height="50%"></img>
</details><br>

- Implemented limit to maximum amount of log files kept inside the logfile directory. (*num_logfiles_to_keep* variable)
- Disabled logging for the loader completely.
- Made currently held weapon sprite more accurate, e.g. this will now work even with custom models. 🔫
- Added about dialog inside the menu and welcome popup dialog when the cheat is firstly injected.

<details><summary>Spoiler</summary>
	<img src="https://i.imgur.com/guPRnTY.png" width="60%" height="60%"></img>
</details><br>

- Added support for non-steam **4554 CS**! 🥳
	- The build will be now automatically chosen if it is not strictly inside the list of what we support. So let's say you inject to 8063 build - 8684 will be chosen. Build 5000 - 4554 will be chosen, etc. 

## Bugfixes

- Changed formatting of CColor class from 0-255 to 0-1 to provide compatibility with the way imgui handle colors. 🌈
- Fixed a bug in Visual Studio where if you would have the cheat inside a directory that has spaces in it such as "folder 1 2 3", the solution would not compile. 🐞
- Fixed bug with static crosshair, where the alpha would softlock to 0 and would never change.
- Fixed a crash on game close that would sometimes happen.
- Fixed a crash when loading a config that could sometimes happen.
- Fixed a bunch of input issues.
- Fixed a bug where the head hitbox wouldn't sometimes be rendered when stood close to a player.

# v1.0.3

Sorry for the wait, I've been busy with IRL stuff. 🙄😪

## Additions and improvements

- Added rechecker bypass, see [this video](https://youtu.be/Razs0s7sgno).
- Implemented default viewmodel enforcement.
- Implemented key binding system (beta).
- Implemented InCommands system (beta).
- Changed the cheat menu key from F1 to `INSERT`!
- Added hand cursor when hovered / held over scrollbar.
- Increased scrollbar width from 9 px to 10 px and added rounding to it.
- Improved console UI.
- Added Variable&Command lists to the menu.
- Added fancy cl_showfps rendering (custom).

<details><summary>Spoiler</summary>
	<img src="https://i.imgur.com/owRASUO.png" width="60%" height="60%"></img>
</details><br>

- Added new "others" tab in the menu.
- Added air stuck, see [this video](https://youtu.be/9snddT03M34).
- Added MOTD removal.
- Added legit/rage bhop hack.
- Added menu section for binds.

## Bugfixes

- Fixed a bug where on the initial launch, when the game was focused before fully injected, the about dialog would not pop up and the window focus would be glitched until the focus was refreshed.
- Fixed a bug where popup windows would retain window size from previously opened popups.
- Fixed crashes when restarting/quitting the game.
- Fixed crash where the cheat would terminate, if injected into game in fullscreen mode while having the UI opened in-game and alt-tabbing from the game.


# v1.0.4

## Additions and improvements

- Improved bunnyhop code.
- Implemented visualization tool to help debug movement code.
- Added customizable GS hack.
- Added customizable EB hack.
- Added customizable Strafe hack.
- Added customizable strafe helper.
- Added viewmodel removal.
- Added players removal.
- Added fastrun & fastwalk.
- Added auto JOF.
- Added thirdperson.
- Added sound esp resolver.
- Added fps fixer for non-steam cs.
- Added anti demo recorder.
- Improved console input UI.
- Added support for cyrillic, thai, vietnamese and chinese.
- Added freetype fonts instead of truetype.
- Added world visuals.

## Bugfixes

- Fixed a bug where sound and entity esp background box wasn't rendering when toggled on.
- Fixed duplicated menu entires inside tabs 💩.
- Fixed a bug where mouse cursor wouldn't change to "hand" when hovered over tab items.
- Fixed random crash that sometimes occured relating to feature list and menu.
- Fixed crosshair not being properly aligned with higher thickness.

# v1.0.5

## Additions and improvements

- Implemented Anti-Screen.
- Implemented aspect ratio changer.
- Added auto zoom, reload, and shoot.
- Improved ESP.
- Added environmental effects, see [this video](https://youtu.be/dsA--TX9MVU).
- Improved world dimlight to affect sky as well.
- Added world fog.
- Added support for 3266 CS! 🥳
- Improved AirStuck.
- Improved chams, see [this video](https://youtu.be/OwkkhIzMJ7k).
- Added backtrack, see [this video](https://youtu.be/G-ZjjsUHHlg).
- Added fake latency.
- Added speed control.
- Reworked the UI.
- Improved various features.
- Added visual panic.
- Added movement visual information.

## Bugfixes

- Fixed ([#0002](https://github.com/oxiKKK/oxware/issues/2))
- Fixed a bug related to mouse cursor where if you were in game alive and you'd alt+tab out of the game, you could not move your mouse.
- Fixed a bug where if you would unload the cheat while in-game, the mouse cursor would get stuck outside of the game, unless you alt-tabbed back to the game and activated the game UI.
- Fixed a lot of stuff and bugs.


# v1.0.6

## Additions and improvements

- Improvements to cheat UI
- Added SID changer.
- Added theme manager.

## Bugfixes

- Fixed a bug with RtlRemoveInvertedFunctionTable.