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
	{ GUICLR_TextLight,						"text_light" },
	{ GUICLR_TextRegular,					"text_regular" },
	{ GUICLR_TextDark,						"text_dark" },
	{ GUICLR_TextBlack,						"text_black" },
	{ GUICLR_TextWhite,						"text_white" },

	// windows
	{ GUICLR_WindowBackground,				"window_bg" },
	{ GUICLR_ChildBackground,				"child_bg" },
	{ GUICLR_PopupBackground,				"popup_bg" },

	// buttons
	{ GUICLR_Button,						"button" },
	{ GUICLR_ButtonHovered,					"button_hovered" },
	{ GUICLR_ButtonActive,					"button_active" },
	{ GUICLR_ButtonSelected,				"button_selected" },

	// url
	{ GUICLR_HyperTextLink,					"urltext" },
	{ GUICLR_HyperTextLinkHovered,			"urltext_hovered" },
	{ GUICLR_HyperTextLinkActive,			"urltext_active" },

	// scrollbar
	{ GUICLR_ScrollbarBackground,			"scrollbar_bg" },
	{ GUICLR_ScrollbarGrab,					"scrollbar_grab" },

	// checkbox
	{ GUICLR_CheckBoxBackground,			"checkbox_bg" },
	{ GUICLR_CheckBoxOutline,				"checkbox_outline" },
	{ GUICLR_CheckBoxSelected,				"checkbox_selected" },
	{ GUICLR_CheckBoxCheckmark,				"checkbox_checkmark" },

	// slider
	{ GUICLR_SliderFrameBg,					"slider_bg" },
	{ GUICLR_SliderGrab,					"slider_grab" },
	{ GUICLR_SliderHovered,					"slider_hovered" },
	{ GUICLR_SliderActive,					"slider_active" },

	// input text
	{ GUICLR_InputTextBg,					"inputtext_bg" },

	// color button
	{ GUICLR_ColorButtonBorderHovered,		"colorbutton_border_hovered" },

	// separator
	{ GUICLR_Separator,						"separator" },

	// tabs
	{ GUICLR_Tab,							"tab" },
	{ GUICLR_TabHovered,					"tab_hovered" },
	{ GUICLR_TabActive,						"tab_active" },

	// progressbar
	{ GUICLR_ProgressBar,					"progressbar" },

	// resize grip
	{ GUICLR_ResizeGrip,					"resize_grip" },
	{ GUICLR_ResizeGripHovered,				"resize_grip_hovered" },
	{ GUICLR_ResizeGripActive,				"resize_grip_active" },

	// tables
	{ GUICLR_TableHeaderBg,					"table_header_bg" },

	// listbox
	{ GUICLR_ListBoxBackground,				"listbox_bg" },
	{ GUICLR_ListBoxBorder,					"listbox_border" },
	{ GUICLR_ListBoxBorderHovered,			"listbox_border_hovered" },
	{ GUICLR_ListBoxArrowBoxBackground,		"listbox_arrow_box_background" },
};
