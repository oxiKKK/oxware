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

#ifndef GUICOLORS_H
#define GUICOLORS_H
#pragma once

//
// NOTE:
// 
//	Don't forget your new color to the translation table below.
//

enum EGUIColorId
{
	GUICLR_Invalid, // just a place holder for like invalid state, etc

	// text
	GUICLR_TextLight,
	GUICLR_TextRegular,
	GUICLR_TextDark,
	GUICLR_TextBlack,
	GUICLR_TextWhite,

	// windows
	GUICLR_WindowBackground,
	GUICLR_ChildBackground,
	GUICLR_PopupBackground,

	// buttons
	GUICLR_Button,
	GUICLR_ButtonHovered,
	GUICLR_ButtonActive,
	GUICLR_ButtonSelected,

	// url
	GUICLR_HyperTextLink,
	GUICLR_HyperTextLinkHovered,
	GUICLR_HyperTextLinkActive,

	// scrollbar
	GUICLR_ScrollbarBackground,
	GUICLR_ScrollbarGrab,

	// checkbox
	GUICLR_CheckBoxBackground,
	GUICLR_CheckBoxOutline,
	GUICLR_CheckBoxSelected,
	GUICLR_CheckBoxCheckmark,

	// slider
	GUICLR_SliderFrameBg,
	GUICLR_SliderGrab,
	GUICLR_SliderHovered,
	GUICLR_SliderActive,

	// input text
	GUICLR_InputTextBg,

	// color button
	GUICLR_ColorButtonBorderHovered,

	// separator
	GUICLR_Separator,

	// tabs
	GUICLR_Tab,
	GUICLR_TabHovered,
	GUICLR_TabActive,

	// progressbar
	GUICLR_ProgressBar,

	// resize grip
	GUICLR_ResizeGrip,
	GUICLR_ResizeGripHovered,
	GUICLR_ResizeGripActive,

	// tables
	GUICLR_TableHeaderBg,

	// listbox
	GUICLR_ListBoxBackground,
	GUICLR_ListBoxBorder,
	GUICLR_ListBoxBorderHovered,
	GUICLR_ListBoxArrowBoxBackground,

	GUICLR_MAX
};

#endif // GUICOLORS_H