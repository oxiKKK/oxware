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

#include <gui/GUIColors.h>

#include <array>

class CGUITheme
{
public:
	// compile-time
	template<EGUIColorId clr> requires(clr > GUICLR_Invalid && clr < GUICLR_MAX)
	const CColor& get_color() const { return m_colors[clr]; }
	template<EGUIColorId clr> requires(clr > GUICLR_Invalid && clr < GUICLR_MAX)
	const uint32_t get_color_u32() const { return m_colors[clr].as_u32(); }

	// run-time
	const CColor& get_color(EGUIColorId clr) const { return m_colors[clr]; }
	const uint32_t get_color_u32(EGUIColorId clr) const { return m_colors[clr].as_u32(); }

	//
	// [INTERNAL]
	//

	// try to avoid using this, use IGUIThemeManager::push_color instead.
	void set_color(EGUIColorId id, const CColor& clr)
	{
		assert(id > GUICLR_Invalid && id < GUICLR_MAX);
		m_colors[id] = clr;
	}

private:
	std::array<CColor, GUICLR_MAX> m_colors;
};

//----------------------------------------------------------------------------------------

class IGUIThemeManager : public IBaseInterface
{
public:
	virtual void initialize() = 0;
	virtual void initialize_imgui() = 0; // call this after ImGui::CreateContext

	// returns theme by id
	virtual std::optional<const CGUITheme*> get_theme_by_id(const std::string& id) = 0;

	// create new theme object. if the theme already exist, the function returns the already existing theme.
	// use this object to modify the contens of the theme (colors).
	virtual CGUITheme& create_theme(const std::string& id) = 0;

	// sets new theme by id. if the theme wasn't found, an error message is printed and the function returns false.
	virtual bool set_new_theme(const std::string& id) = 0;

	// should not return nullptr. current theme is being initialized at the very beginning.
	virtual CGUITheme* get_current_theme() = 0;
	virtual std::string get_current_theme_name() = 0;

	// temporarily changes/restores a color
	virtual void push_color(EGUIColorId id, const CColor& color) = 0;
	virtual void pop_color(size_t amount = 1) = 0;

	// json parsing/writing
	virtual void load_theme_from_json(const nh::json& json) = 0;
	virtual void export_theme_to_json(nh::json& json) = 0;

	// translation. if an invalid id is passed, the function returns 1) GUICLR_Invalid and 2) "" (empty string).
	// the function doesn't print any errors, handle errors yourself.
	virtual EGUIColorId string_to_color_id(const std::string& id) = 0;
	virtual std::string color_id_to_string(EGUIColorId id) = 0;
};

extern IGUIThemeManager* g_gui_thememgr_i;

#define IGUITHEMEMANAGER_INTERFACEID "IGUIThemeManager"

#endif // IGUITHEMEMANAGER_H