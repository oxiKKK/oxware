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
	const CColor& get_color() const { return m_mutable_colors[clr]; }
	template<EGUIColorId clr> requires(clr > GUICLR_Invalid && clr < GUICLR_MAX)
	const uint32_t get_color_u32() const { return m_mutable_colors[clr].as_u32(); }

	// run-time
	const CColor& get_color(EGUIColorId clr) const { return m_mutable_colors[clr]; }
	const uint32_t get_color_u32(EGUIColorId clr) const { return m_mutable_colors[clr].as_u32(); }

	//
	// [INTERNAL]
	//

	// try to avoid using this, use IGUIThemeManager::push_color instead.
	void set_color(EGUIColorId id, const CColor& clr)
	{
		assert(id > GUICLR_Invalid && id < GUICLR_MAX);
		m_mutable_colors[id] = clr;
	}

	// used to initialize both color lists. called when both colors need to be re-set.
	void initialize_colors(EGUIColorId id, const CColor& clr)
	{
		assert(id > GUICLR_Invalid && id < GUICLR_MAX);
		m_mutable_colors[id] = m_immutable_colors[id] = clr;
	}

	// should be avoided, this is used in the color changer code.
	inline std::array<CColor, GUICLR_MAX>& immutable_color_list()
	{
		return m_immutable_colors;
	}

	// getters for immutable
	const CColor& get_immutable_color(EGUIColorId clr) const { return m_immutable_colors[clr]; }
	const uint32_t get_immutable_color_u32(EGUIColorId clr) const { return m_immutable_colors[clr].as_u32(); }

private:
	// we got two lists of colors: 1) that can be affected by push/pop 
	// operations, and 2) that cannot.
	std::array<CColor, GUICLR_MAX> m_mutable_colors;
	std::array<CColor, GUICLR_MAX> m_immutable_colors;
};

//----------------------------------------------------------------------------------------

class IGUIThemeManager : public IBaseInterface
{
public:
	virtual void initialize() = 0;
	virtual void load_font_from_file_if_present() = 0; // called inside cheat at late initialization.
	virtual void initialize_imgui() = 0; // call this after ImGui::CreateContext

	// returns theme by id
	virtual std::optional<const CGUITheme*> get_theme_by_id(const std::string& id) = 0;

	// returns true if theme exist
	virtual bool does_theme_exist(const std::string& id) = 0;

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

	// convenience funcs
	virtual void set_current_theme_export_name(const std::string& name) = 0;
	virtual void reset_current_theme_export_name() = 0;

	// translation. if an invalid id is passed, the function returns 1) GUICLR_Invalid and 2) "" (empty string).
	// the function doesn't print any errors, handle errors yourself.
	virtual EGUIColorId string_to_color_id(const std::string& id) = 0;
	virtual std::string color_id_to_string(EGUIColorId id) = 0;

	// opens the theme file and reads json filed "theme_name" and returns it.
	// returns empty string on failure.
	virtual std::string extract_theme_name_from_file(const std::filesystem::path& theme_file) = 0;

	// should be called after you change some color. we still use some imgui colors such as text, windowbg, etc.
	// so that we don't need to push our color before literally every add_text call. this is good when you need
	// to change colors in real-time, e.g. in the color palette editor.
	virtual void sync_colors_with_imgui() = 0;

	// number of built-in themes
	virtual size_t get_num_themes() = 0;

	// iterate for each built-in themes
	virtual void for_each_built_in(const std::function<void(const std::string& id, const CGUITheme& theme)>& callback) = 0;
};

extern IGUIThemeManager* g_gui_thememgr_i;

#define IGUITHEMEMANAGER_INTERFACEID "IGUIThemeManager"

#endif // IGUITHEMEMANAGER_H