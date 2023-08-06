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

gui_color_translation_t g_gui_color_translation[GUICLR_MAX] =
{
	{ GUICLR_Invalid,						"invalid" },

	// text
	{ GUICLR_TextLight,						"Text light" },
	{ GUICLR_TextRegular,					"Text regular" },
	{ GUICLR_TextDark,						"Text dark" },
	{ GUICLR_TextBlack,						"Text black" },
	{ GUICLR_TextWhite,						"Text white" },

	// active color
	{ GUICLR_FrameBg,						"Frame bg" },
	{ GUICLR_FrameBgHovered,				"Frame bg hovered" },
	{ GUICLR_FrameBgActive,					"Frame bg active" },
	{ GUICLR_FrameBgSelected,				"Frame bg selected" },
	{ GUICLR_FrameActive,					"Frame active" },

	// windows
	{ GUICLR_WindowBackground,				"Window bg" },
	{ GUICLR_ChildBackground,				"Child bg" },

	// buttons
	{ GUICLR_Button,						"Button" },

	// url
	{ GUICLR_URLHovered,					"URL hovered" },

	// scrollbar
	{ GUICLR_ScrollbarGrab,					"Scrollbar grab" },

	// checkbox
	{ GUICLR_CheckBoxBackground,			"Checkbox bg" },
	{ GUICLR_CheckBoxCheckMark,				"Checkbox check mark" },

	// slider
	{ GUICLR_SliderHovered,					"Slider hovered" },

	// input text
	{ GUICLR_InputTextBg,					"Intput text bg" },

	// separator
	{ GUICLR_Separator,						"Separator" },

	// tabs
	{ GUICLR_Tab,							"Tab" },
	{ GUICLR_TabHovered,					"Tab hovered" },
	{ GUICLR_TabActive,						"Tab active" },

	// resize grip
	{ GUICLR_ResizeGrip,					"Resize grip" },
	{ GUICLR_ResizeGripHovered,				"Resize grip hovered" },
	{ GUICLR_ResizeGripActive,				"Resize grip active" },

	// tables
	{ GUICLR_TableHeaderBg,					"Table header bg" },

	// listbox
	{ GUICLR_ListBoxBackground,				"Listbox bg" },
	{ GUICLR_ListBoxBorder,					"Listbox border" },
	{ GUICLR_ListBoxArrowBackground,		"Listbox arrow bg" },
};
