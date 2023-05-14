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

const std::array<key_t, k_key_range> g_virtual_key_translation =
{
	{
		{ "tab",			VK_TAB			},
		{ "enter",			VK_RETURN		},
		{ "escape",			VK_ESCAPE		},
		{ "space",			VK_SPACE		},
		{ "backspace",		VK_BACK			},
		{ "uparrow",		VK_UP			},
		{ "downarrow",		VK_DOWN			},
		{ "leftarrow",		VK_LEFT			},
		{ "rightarrow",		VK_RIGHT		},
		{ "scrolllock",		VK_SCROLL		},
		{ "numlock",		VK_NUMLOCK		},

		{ "alt",			VK_MENU			},
		{ "ctrl",			VK_CONTROL		},
		{ "shift",			VK_SHIFT		},
		{ "capslock",		VK_CAPITAL		},

		{ "f1",				VK_F1			},
		{ "f2",				VK_F2			},
		{ "f3",				VK_F3			},
		{ "f4",				VK_F4			},
		{ "f5",				VK_F5			},
		{ "f6",				VK_F6			},
		{ "f7",				VK_F7			},
		{ "f8",				VK_F8			},
		{ "f9",				VK_F9			},
		{ "f10",			VK_F10			},
		{ "f11",			VK_F11			},
		{ "f12",			VK_F12			},

		{ "ins",			VK_INSERT		},
		{ "del",			VK_DELETE		},
		{ "pgdn",			VK_NEXT			},
		{ "pgup",			VK_PRIOR		},
		{ "home",			VK_HOME			},
		{ "end",			VK_END			},

		{ "mouse1",			VK_LBUTTON		},
		{ "mouse2",			VK_RBUTTON		},
		{ "mouse3",			VK_MBUTTON		},
		{ "mouse4",			VK_XBUTTON1		},
		{ "mouse5",			VK_XBUTTON2		},

		{ "kp_home",		VK_HOME			},
		{ "kp_uparrow",		VK_NUMPAD8		},
		{ "kp_pgup",		VK_NUMPAD9		},
		{ "kp_leftarrow",	VK_NUMPAD4		},
		{ "kp_5",			VK_NUMPAD5		},
		{ "kp_rightarrow",	VK_NUMPAD6		},
		{ "kp_end",			VK_NUMPAD1		},
		{ "kp_downarrow",	VK_NUMPAD2		},
		{ "kp_pgdn",		VK_NUMPAD3		},
		{ "kp_enter",		VK_RETURN 		},
		{ "kp_ins",			VK_NUMPAD0		},
		{ "kp_del",			VK_SEPARATOR	},
		{ "kp_slash",		VK_DIVIDE		},
		{ "kp_minus",		VK_SUBTRACT		},
		{ "kp_plus",		VK_ADD			},

		// custom user-defined kb codes
		{ "mwheelup",		VK_MWHEELUP		},
		{ "mwheeldown",		VK_MWHEELDOWN	},

		{ "pause",			VK_PAUSE		},

		{ "kp_mul",			VK_MULTIPLY		},
		{ "win",			VK_LWIN			},
		{ "win",			VK_RWIN			},

		{ "a",				'A'				},
		{ "A",				'A'				},
		{ "b",				'B'				},
		{ "B",				'B'				},
		{ "c",				'C'				},
		{ "C",				'C'				},
		{ "d",				'D'				},
		{ "D",				'D'				},
		{ "e",				'E'				},
		{ "E",				'E'				},
		{ "f",				'F'				},
		{ "F",				'F'				},
		{ "g",				'G'				},
		{ "G",				'G'				},
		{ "h",				'H'				},
		{ "H",				'H'				},
		{ "i",				'I'				},
		{ "I",				'I'				},
		{ "j",				'J'				},
		{ "J",				'J'				},
		{ "k",				'K'				},
		{ "K",				'K'				},
		{ "l",				'L'				},
		{ "L",				'L'				},
		{ "m",				'M'				},
		{ "M",				'M'				},
		{ "n",				'N'				},
		{ "N",				'N'				},
		{ "o",				'O'				},
		{ "O",				'O'				},
		{ "p",				'P'				},
		{ "P",				'P'				},
		{ "q",				'Q'				},
		{ "Q",				'Q'				},
		{ "r",				'R'				},
		{ "R",				'R'				},
		{ "s",				'S'				},
		{ "S",				'S'				},
		{ "t",				'T'				},
		{ "T",				'T'				},
		{ "u",				'U'				},
		{ "U",				'U'				},
		{ "v",				'V'				},
		{ "V",				'V'				},
		{ "w",				'W'				},
		{ "W",				'W'				},
		{ "x",				'X'				},
		{ "X",				'X'				},
		{ "y",				'Y'				},
		{ "Y",				'Y'				},
		{ "z",				'Z'				},
		{ "Z",				'Z'				},

		{ "0",				'0'				},
		{ "1",				'1'				},
		{ "2",				'2'				},
		{ "3",				'3'				},
		{ "4",				'4'				},
		{ "5",				'5'				},
		{ "6",				'6'				},
		{ "7",				'7'				},
		{ "8",				'8'				},
		{ "9",				'9'				},

		{ "+",				VK_OEM_PLUS		},
		{ "-",				VK_OEM_MINUS	},
		{ ",",				VK_OEM_COMMA	},
		{ ".",				VK_OEM_PERIOD	},
		{ ";",				VK_OEM_1		},
		{ "~",				VK_OEM_3		},
		{ "/",				VK_OEM_2		},
		{ "[",				VK_OEM_4		},
		{ "\\",				VK_OEM_5		},
		{ "]",				VK_OEM_6		},
		{ "]",				VK_OEM_7		},

		{ NULL,				0				}
	}
};