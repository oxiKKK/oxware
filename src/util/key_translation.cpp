/*
*	OXWARE developed by oxiKKK
*	Copyright (c) 2023
*
*	This program is licensed under the MIT license. By downloading, copying,
*	installing or using this software you agree to this license.
*
*	License Agreement
*
*	Permission is hereby granted, free of charge, to any person obtaining a
*	copy of this software and associated documentation files (the "Software"),
*	to deal in the Software without restriction, including without limitation
*	the rights to use, copy, modify, merge, publish, distribute, sublicense,
*	and/or sell copies of the Software, and to permit persons to whom the
*	Software is furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included
*	in all copies or substantial portions of the Software.
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
*	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
*	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
*	IN THE SOFTWARE.
*/

#include "precompiled.h"

#include <hlsdk/engine/keydefs.h>

// allows backward/forward translations between virtual and engine keys and key names.
const std::array<key_t, k_key_range> g_virtual_key_translation =
{
	{
		{ "tab",			VK_TAB,			K_TAB				},
		{ "enter",			VK_RETURN,		K_ENTER				},
		{ "escape",			VK_ESCAPE,		K_ESCAPE			},
		{ "space",			VK_SPACE,		K_SPACE				},
		{ "backspace",		VK_BACK,		K_BACKSPACE			},
		{ "uparrow",		VK_UP,			K_UPARROW			},
		{ "downarrow",		VK_DOWN,		K_DOWNARROW			},
		{ "leftarrow",		VK_LEFT,		K_LEFTARROW			},
		{ "rightarrow",		VK_RIGHT,		K_RIGHTARROW		},
		{ "scrolllock",		VK_SCROLL,		NULL/*undefined*/	},
		{ "numlock",		VK_NUMLOCK,		NULL/*undefined*/	},

		{ "alt",			VK_MENU,		K_ALT				},
		{ "ctrl",			VK_CONTROL,		K_CTRL				},
		{ "shift",			VK_SHIFT,		K_SHIFT				},
		{ "capslock",		VK_CAPITAL,		K_CAPSLOCK			},

		{ "f1",				VK_F1,			K_F1				},
		{ "f2",				VK_F2,			K_F2				},
		{ "f3",				VK_F3,			K_F3				},
		{ "f4",				VK_F4,			K_F4				},
		{ "f5",				VK_F5,			K_F5				},
		{ "f6",				VK_F6,			K_F6				},
		{ "f7",				VK_F7,			K_F7				},
		{ "f8",				VK_F8,			K_F8				},
		{ "f9",				VK_F9,			K_F9				},
		{ "f10",			VK_F10,			K_F10				},
		{ "f11",			VK_F11,			K_F11				},
		{ "f12",			VK_F12,			K_F12				},

		{ "ins",			VK_INSERT,		K_INS				},
		{ "del",			VK_DELETE,		K_DEL				},
		{ "pgdn",			VK_NEXT,		K_PGDN				},
		{ "pgup",			VK_PRIOR,		K_PGUP				},
		{ "home",			VK_HOME,		K_HOME				},
		{ "end",			VK_END,			K_END				},

		{ "mouse1",			VK_LBUTTON,		K_MOUSE1			},
		{ "mouse2",			VK_RBUTTON,		K_MOUSE2			},
		{ "mouse3",			VK_MBUTTON,		K_MOUSE3			},
		{ "mouse4",			VK_XBUTTON1,	K_MOUSE4			},
		{ "mouse5",			VK_XBUTTON2,	K_MOUSE5			},

		{ "kp_home",		VK_NUMPAD7,		K_KP_HOME			},
		{ "kp_uparrow",		VK_NUMPAD8,		K_KP_UPARROW		},
		{ "kp_pgup",		VK_NUMPAD9,		K_KP_PGUP			},
		{ "kp_leftarrow",	VK_NUMPAD4,		K_KP_LEFTARROW		},
		{ "kp_5",			VK_NUMPAD5,		K_KP_5				},
		{ "kp_rightarrow",	VK_NUMPAD6,		K_KP_RIGHTARROW		},
		{ "kp_end",			VK_NUMPAD1,		K_KP_END			},
		{ "kp_downarrow",	VK_NUMPAD2,		K_KP_DOWNARROW		},
		{ "kp_pgdn",		VK_NUMPAD3,		K_KP_PGDN			},
		{ "kp_enter",		VK_RETURN, 		K_KP_ENTER			},
		{ "kp_ins",			VK_NUMPAD0,		K_KP_INS			},
		{ "kp_del",			VK_SEPARATOR,	K_KP_DEL			},
		{ "kp_slash",		VK_DIVIDE,		K_KP_SLASH			},
		{ "kp_minus",		VK_SUBTRACT,	K_KP_MINUS			},
		{ "kp_plus",		VK_ADD,			K_KP_PLUS			},

		// custom user-defined kb codes
		{ "mwheelup",		VK_MWHEELUP,	K_MWHEELUP			},
		{ "mwheeldown",		VK_MWHEELDOWN,	K_MWHEELDOWN		},

		{ "pause",			VK_PAUSE,		K_PAUSE				},

		{ "kp_mul",			VK_MULTIPLY,	K_KP_MUL			},
		{ "win",			VK_LWIN,		K_WIN				},
		{ "win",			VK_RWIN,		K_WIN				},

		{ "a",				'A',			'a'					},
		{ "A",				'A',			'a'					},
		{ "b",				'B',			'b'					},
		{ "B",				'B',			'b'					},
		{ "c",				'C',			'c'					},
		{ "C",				'C',			'c'					},
		{ "d",				'D',			'd'					},
		{ "D",				'D',			'd'					},
		{ "e",				'E',			'e'					},
		{ "E",				'E',			'e'					},
		{ "f",				'F',			'f'					},
		{ "F",				'F',			'f'					},
		{ "g",				'G',			'g'					},
		{ "G",				'G',			'g'					},
		{ "h",				'H',			'h'					},
		{ "H",				'H',			'h'					},
		{ "i",				'I',			'i'					},
		{ "I",				'I',			'i'					},
		{ "j",				'J',			'j'					},
		{ "J",				'J',			'j'					},
		{ "k",				'K',			'k'					},
		{ "K",				'K',			'k'					},
		{ "l",				'L',			'l'					},
		{ "L",				'L',			'l'					},
		{ "m",				'M',			'm'					},
		{ "M",				'M',			'm'					},
		{ "n",				'N',			'n'					},
		{ "N",				'N',			'n'					},
		{ "o",				'O',			'o'					},
		{ "O",				'O',			'o'					},
		{ "p",				'P',			'p'					},
		{ "P",				'P',			'p'					},
		{ "q",				'Q',			'q'					},
		{ "Q",				'Q',			'q'					},
		{ "r",				'R',			'r'					},
		{ "R",				'R',			'r'					},
		{ "s",				'S',			's'					},
		{ "S",				'S',			's'					},
		{ "t",				'T',			't'					},
		{ "T",				'T',			't'					},
		{ "u",				'U',			'u'					},
		{ "U",				'U',			'u'					},
		{ "v",				'V',			'v'					},
		{ "V",				'V',			'v'					},
		{ "w",				'W',			'w'					},
		{ "W",				'W',			'w'					},
		{ "x",				'X',			'x'					},
		{ "X",				'X',			'x'					},
		{ "y",				'Y',			'y'					},
		{ "Y",				'Y',			'y'					},
		{ "z",				'Z',			'z'					},
		{ "Z",				'Z',			'z'					},

		{ "0",				'0',			'0'					},
		{ "1",				'1',			'1'					},
		{ "2",				'2',			'2'					},
		{ "3",				'3',			'3'					},
		{ "4",				'4',			'4'					},
		{ "5",				'5',			'5'					},
		{ "6",				'6',			'6'					},
		{ "7",				'7',			'7'					},
		{ "8",				'8',			'8'					},
		{ "9",				'9',			'9'					},

		{ "+",				VK_OEM_PLUS,	'+'					},
		{ "-",				VK_OEM_MINUS,	'-'					},
		{ ",",				VK_OEM_COMMA,	','					},
		{ ".",				VK_OEM_PERIOD,	'.'					},
		{ ";",				VK_OEM_1,		'`'					}, // gs uses '`' as key for the console
		{ "~",				VK_OEM_3,		'~'					},
		{ "/",				VK_OEM_2,		'/'					},
		{ "[",				VK_OEM_4,		'['					},
		{ "\\",				VK_OEM_5,		'\\'				},
		{ "]",				VK_OEM_6,		']'					},
	}
};

//----------------------------------------------------------------------------------

