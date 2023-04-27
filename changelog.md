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

<details><summary>See for yourself! 😉</summary>
	<img src="https://i.imgur.com/bYzp8KD.png"></img>
</details>

## Bugfixes

- Changed formatting of CColor class from 0-255 to 0-1 to provide compatibility with the way imgui handle colors. 🌈
- Fixed a bug in Visual Studio where if you would have the cheat inside a directory that has spaces in it such as "folder 1 2 3", the solution would not compile. 🐞

