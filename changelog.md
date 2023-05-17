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

## Additions and improvements

- Added rechecker bypass.
- Implemented default viewmodel enforcement.
- Implemented key binding system (beta).
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
- Added air stuck.

## Bugfixes

- Fixed a bug where on the initial launch, when the game was focused before fully injected, the about dialog would not pop up and the window focus would be glitched until the focus was refreshed.
- Fixed a bug where popup windows would retain window size from previously opened popups.

