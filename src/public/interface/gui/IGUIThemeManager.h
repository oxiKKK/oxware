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
#ifndef IGUITHEMEMANAGER_H
#define IGUITHEMEMANAGER_H
#pragma once

#include "Color.h"

#include <array>

enum EGUIColor
{
	GUICLR_NONE, 

	GUICLR_TextLight,
	GUICLR_TextRegular,
	GUICLR_TextDark,
	GUICLR_TextBlack,
	GUICLR_TextWhite,
	
	GUICLR_WindowBackground,
	GUICLR_ChildBackground,
	GUICLR_PopupBackground,

	GUICLR_Button,
	GUICLR_ButtonHovered,
	GUICLR_ButtonActive,

	GUICLR_ScrollbarBackground,
	GUICLR_ScrollbarGrab,

	GUICLR_CheckBoxBackground,
	GUICLR_CheckBoxOutline,
	GUICLR_CheckBoxSelected,
	GUICLR_CheckBoxCheckmark,

	GUICLR_SliderFrameBg,
	GUICLR_SliderGrab,
	GUICLR_SliderHovered,
	GUICLR_SliderActive,

	GUICLR_InputTextBg,

	GUICLR_ColorButtonBorderHovered,

	GUICLR_Separator,

	GUICLR_Tab,
	GUICLR_TabHovered,
	GUICLR_TabActive,

	GUICLR_ProgressBar,

	GUICLR_ResizeGrip,
	GUICLR_ResizeGripHovered,
	GUICLR_ResizeGripActive,

	GUICLR_MAX
};

class GUIThemeObject_t
{
public:
	constexpr GUIThemeObject_t() :
		m_Colors()
	{
	}
	constexpr GUIThemeObject_t(const std::array<CColor, GUICLR_MAX>& colors) :
		m_Colors(colors)
	{
	}
	constexpr GUIThemeObject_t(std::array<CColor, GUICLR_MAX>&& colors) :
		m_Colors(std::move(colors))
	{
	}

	// try to avoid using this, use IGUIThemeManager::push_color instead.
	void set_using_color(EGUIColor id, const CColor& clr) { m_Colors[id] = clr; }

	// compile-time
	template<EGUIColor clr> requires(clr > GUICLR_NONE && clr < GUICLR_MAX)
	const CColor& get_color() const { return m_Colors[clr]; }
	template<EGUIColor clr> requires(clr > GUICLR_NONE && clr < GUICLR_MAX)
	const uint32_t get_color_u32() const { return m_Colors[clr].as_u32(); }

	// run-time
	const CColor& get_color(EGUIColor clr) const { return m_Colors[clr]; }
	const uint32_t get_color_u32(EGUIColor clr) const { return m_Colors[clr].as_u32(); }

private:
	std::array<CColor, GUICLR_MAX> m_Colors;
};

class IGUIThemeManager : public IBaseInterface
{
public:
	virtual void initialize() = 0;

	virtual GUIThemeObject_t* get_theme_by_name(const char* name_identifier) = 0;
	virtual GUIThemeObject_t* get_current_theme() = 0;
	virtual void set_new_theme(const char* name_identifier) = 0;

	virtual void push_color(EGUIColor id, const CColor& color) = 0;
	virtual void pop_color(size_t amount = 1) = 0;
};

extern IGUIThemeManager* g_gui_thememgr_i;

#define IGUITHEMEMANAGER_INTERFACEID "IGUIThemeManager"

#endif // IGUITHEMEMANAGER_H