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

#ifndef UIMENU_H
#define UIMENU_H
#pragma once

// constants that describe some of the paddings and margins used in the menu
class CMenuStyle
{
public:
	inline static constexpr float k_rounding_factor = 20.0f;
	inline static constexpr Vector2D k_menu_rect_size = { 600, 400 };
	inline static constexpr Vector2D k_tab_select_size = { 120, 400 };
	inline static constexpr float k_top_region_size_h = 50.0f;
	inline static constexpr float k_menu_contents_padding = 10.0f;
	inline static constexpr float k_menu_contents_padding_bottom = 5.0f;
	inline static constexpr float k_bottom_reserved_rect_h = 15.0f;
	inline static constexpr float k_bottom_right_timestamp_rightside_padding = 5.0f;
	inline static constexpr float k_bottom_right_timestamp_bottomside_padding = 2.0f;
	inline static constexpr Vector2D k_bottom_left_desc_padding = { 5.0f, 2.0f };
	inline static constexpr float k_child_contents_rounding = 10.0f;
	inline static constexpr Vector2D k_child_contents_padding = { 10.0f, 5.0f };
	inline static constexpr float k_child_width = 210.0f;
	inline static constexpr Vector2D k_about_button_padding = { 15.0f, 15.0f };
	inline static constexpr Vector2D k_about_button_size = { 105.0f, 20.0f };
	inline static constexpr Vector2D k_about_button_pos = { k_menu_rect_size.x - k_about_button_size.x - k_about_button_padding.x, k_about_button_padding.y };

	inline static Vector2D calc_child_size(float height) { return { CMenuStyle::k_child_width, height }; }
	inline static float get_child_width_w_padding() { return CMenuStyle::k_child_width - CMenuStyle::k_child_contents_padding.x * 2; }
};

//----------------------------------------------------------------------------------------------------------------------
//
//	Menu tabs
//

enum EMenuTabId
{
	UIMENU_Blank,

	// AI
	UIMENU_RageBot,
	UIMENU_LegitBot,
	UIMENU_AntiBot,
	UIMENU_WayBot,

	// Visuals
	UIMENU_Viewmodel,
	UIMENU_World,
	UIMENU_Render,
	UIMENU_Visuals4,

	// Miscellaneous
	UIMENU_Exploits,
	UIMENU_Miscellaneous2,
	UIMENU_Miscellaneous3,

	// Config
	UIMENU_Config,

	UIMENU_Max,
};

class CUIMenu;
typedef void(__thiscall CUIMenu::*TabCallbackFn)();

class MenuTabItem
{
public:
	MenuTabItem(EMenuTabId id, TabCallbackFn func, const char* label, const char* desc)
		: m_tab_id(id), m_tab_callback(func), m_label(label), m_desc(desc)
	{
	}

	void render(Vector2D& offset, Vector2D& relative_offset, EMenuTabId& active_tab_id);

	std::string m_label, m_desc;

	EMenuTabId m_tab_id;

	TabCallbackFn m_tab_callback;
};

class MenuTabSection
{
public:
	void set_label(const char* label)
	{
		m_label = label;
	}

	void add_tab(const MenuTabItem& item)
	{
		m_menu_tabs.push_back(item);
	}

	MenuTabItem* get_tab(EMenuTabId id)
	{
		for (auto& tab : m_menu_tabs)
		{
			if (tab.m_tab_id == id)
				return &tab;
		}

		assert(0);
		return nullptr;
	}

	void render(Vector2D& offset, Vector2D& relative_offset, const Vector2D& child_size, EMenuTabId& active_tab_id);

private:
	void render_current_label(Vector2D& offset, Vector2D& relative_offset, const Vector2D& child_size);

public:
	std::vector<MenuTabItem> m_menu_tabs;

	std::string m_label;
};

//----------------------------------------------------------------------------------------------------------------------
//
// Menu class
//

class CUIMenu final : public IRenderingContext_KeyActivate
{
public:
	CUIMenu(const std::string& id, int virtual_key) :
		IRenderingContext_KeyActivate(id, virtual_key)
	{
	}

	void on_initialize();
	void on_render();
	void on_destroy();

private:
	// tab callbacks
	void tab_blank();
	void tab_ragebot();
	void tab_legitbot();
	void tab_antibot();
	void tab_waybot();
	void tab_viewmodel();
	void tab_world();
	void tab_render();
	void tab_visuals4();
	void tab_exploits();
	void tab_miscellaneous2();
	void tab_miscellaneous3();
	void tab_config();

	MenuTabSection m_tabsec_AIHelpers;
	MenuTabSection m_tabsec_Visuals;
	MenuTabSection m_tabsec_Miscellaneous;
	MenuTabSection m_tabsec_Configuration;

	void add_menu_child(const std::string& label, const Vector2D& size, bool border, ImGuiWindowFlags flags, const std::function<void()>& pfn_contents);

	EMenuTabId m_active_tab_id = UIMENU_Blank;

	// Active offset for rendering tabs & section labels.
	// Relative active offset is not counting the child position.
	Vector2D m_sectab_active_offs, m_sectab_relative_active_offset;

	std::vector<MenuTabSection*> m_tab_sections;
	static std::array<TabCallbackFn, UIMENU_Max> s_active_tab_callback_translation;
};

class CMenuWidgetDescription
{
public:
	DECL_BASIC_CLASS(CMenuWidgetDescription);

public:
	void set_desc(const char* desc) { m_description = desc; };
	const char* get_desc() { return m_description.c_str(); };
	void clear_desc() { m_description.clear(); };

private:
	std::string m_description;
};

#endif // UIMENU_H