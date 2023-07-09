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

VarBoolean ui_menu_resizable("ui_menu_resizable", "This is a beta feature!", false);

// Note: moved here from the header file for faster compile times
const float CMenuStyle::k_rounding_factor = 20.0f;
const Vector2D CMenuStyle::k_menu_rect_size = { 600, 400 };
const Vector2D CMenuStyle::k_menu_rect_size_max = k_menu_rect_size * Vector2D(1.70f, 1.75f); // sized so that four child columns fit inside contents
const float CMenuStyle::k_tab_select_width = 120.0f;
const float CMenuStyle::k_top_region_size_h = 50.0f;
const float CMenuStyle::k_menu_contents_padding = 10.0f;
const float CMenuStyle::k_menu_contents_padding_bottom = 5.0f;
const float CMenuStyle::k_bottom_reserved_rect_h = 15.0f;
const float CMenuStyle::k_bottom_right_timestamp_rightside_padding = 5.0f;
const float CMenuStyle::k_bottom_right_timestamp_bottomside_padding = 2.0f;
const Vector2D CMenuStyle::k_bottom_left_desc_padding = { 5.0f, 2.0f };
const float CMenuStyle::k_child_contents_rounding = 5.0f;
const Vector2D CMenuStyle::k_child_contents_padding = { 5.0f, 5.0f };
const float CMenuStyle::k_child_width = 210.0f;
const Vector2D CMenuStyle::k_unload_button_padding = { 15.0f, 15.0f };
const Vector2D CMenuStyle::k_unload_button_size = { 105.0f, 20.0f };
const Vector2D CMenuStyle::k_about_button_size = { 50.0f, 20.0f };

Vector2D CMenuStyle::calc_child_size(float height) { return { k_child_width, height }; }
Vector2D CMenuStyle::calc_child_size_2x(float height) { return { k_child_width * 2.0f + k_child_contents_padding.x, height }; }

float CMenuStyle::get_child_width_w_padding() { return CMenuStyle::k_child_width - CMenuStyle::k_child_contents_padding.x * 2; }

// use this instead of width begin -1.0f
Vector2D CMenuStyle::child_full_width(float height) { return { -1.0f - k_menu_contents_padding, height }; }

//---------------------------------------------------------------------------------------------------

void MenuTab::render(Vector2D& offset, Vector2D& relative_offset, EMenuTabId id, TabSectionId& current_tab_section_id)
{
	g_gui_widgets_i->set_cursor_pos(relative_offset);

	auto button_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Bold);
	g_gui_widgets_i->push_font(button_font);

	// text color
	g_gui_widgets_i->push_color(ImGuiCol_Text, g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark));

	bool selected = (current_tab_section_id.first == id);
	if (g_gui_widgets_i->add_toggle_button(m_label.c_str(), { CMenuStyle::k_tab_select_width - 20.0f, 0.0f }, selected))
	{
		if (selected)
		{
			current_tab_section_id.first = UIMENU_Blank; // selected the same tab, set to blank
		}
		else
		{
			current_tab_section_id.first = id; // make current one selected
		}
	}

	if (g_gui_widgets_i->is_last_widget_hovered())
	{
		CMenuWidgetDescription::the().set_desc(m_desc.c_str());
	}

	g_gui_widgets_i->pop_color(1);

	g_gui_widgets_i->pop_font();

	relative_offset.y += button_font->FontSize + 4.0f;
	offset.y += button_font->FontSize + 4.0f;
}

//---------------------------------------------------------------------------------------------------

void MenuTabGroup::render(Vector2D& offset, Vector2D& relative_offset, const Vector2D& child_size, TabSectionId& current_tab_section_id)
{
	render_current_label(offset, relative_offset, child_size);

	for (auto& [id, tab] : m_menu_tabs)
	{
		tab.render(offset, relative_offset, id, current_tab_section_id);
	}
}

void MenuTabGroup::render_current_label(Vector2D& offset, Vector2D& relative_offset, const Vector2D& child_size)
{
	auto section_label_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular);

	// section title
	g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(), 
										  section_label_font,
										  { offset.x, offset.y + 5.0f },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextLight),
										  m_label);

	relative_offset.y += section_label_font->FontSize;
	offset.y += section_label_font->FontSize;

	// separator underneath
	g_gui_window_rendering_i->render_line(g_gui_window_rendering_i->get_current_drawlist(), 
										  { offset.x, offset.y + 4.0f },
										  { offset.x + child_size.x - 10.f * 2.f, offset.y + 5.f },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_Separator));

	relative_offset.y += 8.f;
	offset.y += 8.f;
}

//----------------------------------------------------------------------------------------------------------------------

void CMenuContentsGrid::precompute(const Vector2D& contents_size, const MenuTabSection& tab_section, bool force_compute)
{
	static Vector2D last_screen_size = {};
	if (!force_compute)
	{
		if (GetTickCount() - last_compute_time <= 250)
		{
			return;
		}

		if (last_screen_size == contents_size)
		{
			return;
		}
	}

	float contents_width = contents_size.x;

	const Vector2D& padding = CMenuStyle::k_child_contents_padding;

	int num_big_childern = 0;

	//
	// big children
	//
	{
		int current_column = 0, current_row = 0;
		Vector2D cursor_position = {};
		
		for (auto child : tab_section.m_children)
		{
			bool bigchild = child->get_child_size().x > CMenuStyle::k_child_width;
			if (!bigchild)
			{
				continue;
			}
			
			const Vector2D& child_size = child->get_child_size();

			child->m_computed_position = cursor_position;

			cursor_position.y += child_size.y + padding.y;

			num_big_childern++;
		}
	}

	//
	// regular children
	//
	{
		float x_offset = (num_big_childern != 0) ? ((CMenuStyle::k_child_width + padding.x) * 2.0f) : 0.0f;

		int num_regular_children = std::max((int)tab_section.m_children.size() - num_big_childern, 0);

		int current_column = 0, current_row = 0;
		Vector2D cursor_position = { x_offset, 0.0f };
		int max_children_in_one_row = std::max((int)((contents_width - x_offset) / (CMenuStyle::k_child_width + padding.x)), 1);
		int n_children_per_column = (int)std::floor((float)num_regular_children / max_children_in_one_row);
		int n_children_per_column_remainder = num_regular_children % max_children_in_one_row;
		for (auto child : tab_section.m_children)
		{
			bool bigchild = child->get_child_size().x > CMenuStyle::k_child_width;
			if (bigchild)
			{
				continue;
			}
			const Vector2D& child_size = child->get_child_size();

			child->m_computed_position = cursor_position;

			bool has_remainder = n_children_per_column_remainder != 0;

			if ((++current_row) >= n_children_per_column && n_children_per_column_remainder-- <= 0)
			{
				current_column++;
				current_row = 0;

				cursor_position.x += child_size.x + padding.x;
				cursor_position.y = 0.0f;
			}
			else
			{
				cursor_position.y += child_size.y + padding.y;
			}
		}
	}

	last_compute_time = GetTickCount();
	last_screen_size = contents_size;
}

//---------------------------------------------------------------------------------------------------

void CUIMenu::on_initialize()
{
	//
	// Visuals
	//
	m_tabgroup_Visuals.set_label("Visuals");
	m_tabgroup_Visuals.add_tab(
		UIMENU_World,
		{ "World", "World-related cheats",
			{{
				0,
				{{
					new MenuChilden::World::Removals({ "Removals", CMenuStyle::calc_child_size(250) }),
					new MenuChilden::World::Thirdperson({ "Thirdperson", CMenuStyle::calc_child_size(100) }),
					new MenuChilden::World::AntiDemoRecorder({ "Anti demo recorder", CMenuStyle::calc_child_size(125) }),
					new MenuChilden::World::AntiScreen({ "Anti screen", CMenuStyle::calc_child_size(90) }),
					new MenuChilden::World::Automation({ "Automation", CMenuStyle::calc_child_size(120) }),
					new MenuChilden::World::SpeedControl({ "Speed control", CMenuStyle::calc_child_size(215) }),
					new MenuChilden::World::ViewmodelOffset({ "Viewmodel offset", CMenuStyle::calc_child_size(90) }),
					new MenuChilden::World::Backtrack({ "Backtrack", CMenuStyle::calc_child_size(105) }),
					new MenuChilden::World::SmokeVisuals({ "Smoke visuals", CMenuStyle::calc_child_size(180) }),
					new MenuChilden::World::NonSteamFpsFix({ "Non-steam fps fix", CMenuStyle::calc_child_size(150) }),
					new MenuChilden::World::WorldVisuals({ "World visuals", CMenuStyle::calc_child_size(225) }),
					new MenuChilden::World::EnvironmentalEffects({ "Environmental effects", CMenuStyle::calc_child_size(300) }),
				}}
			}}
		});

	m_tabgroup_Visuals.add_tab(
	UIMENU_Rendering,
		{ "Rendering", "Rendering-related cheats",
			{{
				0,
				{{
					new MenuChilden::World::FieldOfView({ "Field of view", CMenuStyle::calc_child_size(85) }),
				}}
			}}
		});
	//m_tabgroup_Visuals.add_tab({ UIMENU_Screen, "Screen", "Screen-related cheats" });
	m_tab_groups.push_back(&m_tabgroup_Visuals);
	
	m_tabgroup_Miscellaneous.set_label("Miscellaneous");
	m_tabgroup_Miscellaneous.add_tab(
		UIMENU_Exploits,
		{ "Exploits", "Various game exploitations",
			{{
				0,
				{{
					new MenuChilden::Exploits::ServerCommandFilter(	{ "Server command filter", CMenuStyle::calc_child_size_2x(210) }),
				}}
			}}
		});
	
	//m_tabgroup_Miscellaneous.add_tab({ UIMENU_Movement, &CUIMenu::tab_movement, "Movement", "Kreedz/Movement cheats" });
	////m_tabgroup_Miscellaneous.add_tab({ UIMENU_Miscellaneous2, &CUIMenu::tab_miscellaneous2, "Miscellaneous2", "TODO item description" });
	////m_tabgroup_Miscellaneous.add_tab({ UIMENU_Miscellaneous3, &CUIMenu::tab_miscellaneous3, "Miscellaneous3", "TODO item description" });
	m_tab_groups.push_back(&m_tabgroup_Miscellaneous);
	//
	//m_tabgroup_Configuration.set_label("Configuration");
	//m_tabgroup_Configuration.add_tab({ UIMENU_Config, &CUIMenu::tab_config, "Config", "Cheat configuration and configs" });
	//m_tabgroup_Configuration.add_tab({ UIMENU_Binds, &CUIMenu::tab_binds, "Binds", "Cheat bind manager" });
	//m_tabgroup_Configuration.add_tab({ UIMENU_InCommands, &CUIMenu::tab_incommands, "InCommands", "InCommands manager" });
	//m_tab_groups.push_back(&m_tabgroup_Configuration);
	//
	//m_tabgroup_Other.set_label("Other");
	//m_tabgroup_Other.add_tab({ UIMENU_CommandList, &CUIMenu::tab_cmdlist, "Command list", "Cheat command list" });
	//m_tabgroup_Other.add_tab({ UIMENU_VariableList, &CUIMenu::tab_varlist, "Variable list", "Cheat variable list" });
	//m_tabgroup_Other.add_tab({ UIMENU_Others, &CUIMenu::tab_others, "Others", "Other miscelanoues items" });
	//m_tab_groups.push_back(&m_tabgroup_Other);
}

void CUIMenu::on_render()
{
	// we have to update it here since the keyscan buttons may be outside of the InCommands tab.
	CUIInCommandKeyBinding::the().update();

	g_gui_widgets_i->set_next_window_pos({ 100, 100 }, ImGuiCond_Once);
	g_gui_widgets_i->set_next_window_size(CMenuStyle::k_menu_rect_size, ImGuiCond_Once);
	g_gui_widgets_i->set_next_window_rounding(CMenuStyle::k_rounding_factor, ImDrawFlags_RoundCornersTopRight | ImDrawFlags_RoundCornersBottomLeft);

	g_gui_widgets_i->set_next_window_size_constrains(CMenuStyle::k_menu_rect_size, CMenuStyle::k_menu_rect_size_max);

	auto window_flags =
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;

	//if (ui_menu_resizable.get_value())
	{
		window_flags &= ~ImGuiWindowFlags_NoResize;
	}

	auto segoeui_extra = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_30px, FDC_Bold);

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

	CMenuWidgetDescription::the().clear_desc();

	g_gui_widgets_i->create_new_window(
		"menu", window_flags, [&]()
		{
			auto window_pos = g_gui_widgets_i->get_current_window_pos();
			auto window_size = g_gui_widgets_i->get_current_window_size();

			g_gui_widgets_i->set_next_window_rounding(CMenuStyle::k_rounding_factor, ImDrawFlags_RoundCornersBottomLeft);
			g_gui_widgets_i->add_child(
				"menu_left",
				Vector2D(CMenuStyle::k_tab_select_width, window_size.y),
				false,
				ImGuiWindowFlags_None,
				[&]()
				{
					auto child_pos = g_gui_widgets_i->get_current_window_pos();
					auto child_size = g_gui_widgets_i->get_current_window_size();

					const char* label_text = "OXWARE";
					auto label_size = g_gui_fontmgr_i->calc_font_text_size(segoeui_extra, label_text);

					g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(), 
														  segoeui_extra,
														  { child_pos.x + child_size.x / 2 - label_size.x / 2, window_pos.y + 10 },
														  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark),
														  label_text);

					m_sectab_relative_active_offset = { 10.0f, 50.0f };
					m_sectab_active_offs = child_pos + m_sectab_relative_active_offset;

					for (auto group : m_tab_groups)
					{
						group->render(m_sectab_active_offs, m_sectab_relative_active_offset,
									  child_size, m_active_tab_section);
					}
				}
			);

			//
			// now render contents of the menu.
			//

			g_gui_widgets_i->set_cursor_pos({
				CMenuStyle::k_tab_select_width + CMenuStyle::k_menu_contents_padding,
				CMenuStyle::k_top_region_size_h + CMenuStyle::k_menu_contents_padding });

			g_gui_widgets_i->add_child(
				"menu_contents",
				{
					window_size.x - CMenuStyle::k_tab_select_width - (CMenuStyle::k_menu_contents_padding * 2.0f),
					window_size.y - CMenuStyle::k_top_region_size_h - (CMenuStyle::k_menu_contents_padding * 1.0f + CMenuStyle::k_menu_contents_padding_bottom) - CMenuStyle::k_bottom_reserved_rect_h,
				},
				false,
				ImGuiWindowFlags_NoBackground,
				[&]()
				{
					handle_menu_contents_rendering();
				});

			render_menu_decoration(window_pos, window_size);
		});

	render_github_repo_link_decor();

	g_gui_widgets_i->pop_stylevar(1); // window padding
}

void CUIMenu::on_destroy()
{
	CConsole::the().info(__FUNCTION__ "()");
}

void CUIMenu::handle_menu_contents_rendering()
{
	auto window_size = g_gui_widgets_i->get_current_window_size();

	// how many child columns will fit on screen
	float padding_between_childern_x = CMenuStyle::k_child_contents_padding.x;
	float padding_between_childern_y = CMenuStyle::k_child_contents_padding.y;
	int num_columns_fitted = (int)(window_size.x / (CMenuStyle::k_child_width + padding_between_childern_x));

	// blank tab, nothing selected
	if (m_active_tab_section.first == UIMENU_Blank)
	{
		g_gui_widgets_i->add_window_centered_text_disabled("Start by looking through the tabs on the left!");
		return;
	}

	// find the current menu tab
	MenuTab* current_menu_tab = nullptr;
	for (auto group : m_tab_groups)
	{
		for (auto& [id, tab] : group->m_menu_tabs)
		{
			if (id == m_active_tab_section.first)
			{
				current_menu_tab = &tab;
				break;
			}
		}

		if (current_menu_tab)
		{
			break; // already found
		}
	}

	if (!current_menu_tab)
	{
		return; // didn't find the tab
	}

	bool contents_changed = false;
	static EMenuTabId last_id;
	if (last_id != m_active_tab_section.first)
	{
		contents_changed = true;
		last_id = m_active_tab_section.first;
	}

	static int last_section_id;
	if (last_section_id != m_active_tab_section.second)
	{
		contents_changed = true;
		last_section_id = m_active_tab_section.second;
	}

	for (auto& [id, section] : current_menu_tab->m_sections)
	{
		if (id != m_active_tab_section.second)
		{
			continue;
		}

		CMenuContentsGrid::the().precompute(window_size, section, contents_changed);

		int child_num = 0;
		for (auto child : section.m_children)
		{
			g_gui_widgets_i->set_cursor_pos(child->m_computed_position);

			CUIMenuWidgets::the().add_menu_child_collapsible(
				child->get_header(), child->get_child_size(), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
				[&child]()
				{
					child->contents();
				});

			child_num++;
		}
	}
}

void CUIMenu::render_github_repo_link_decor()
{
	auto screen_size = g_imgui_platform_layer_i->get_screen_size();

	auto link_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular);

	const char* lnk_label = "www.github.com/oxiKKK/oxware";
	auto label_size = g_gui_fontmgr_i->calc_font_text_size(link_font, lnk_label);

	g_gui_widgets_i->set_next_window_pos({ screen_size.x - label_size.x + 10.0f, screen_size.y - label_size.y }, ImGuiCond_Always);

	static constexpr auto window_flags1 =
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoMove;
	g_gui_widgets_i->create_new_window(
		"github_link", window_flags1,
		[&]()
		{
			if (g_gui_widgets_i->add_hypertext_link(lnk_label))
			{
				CGenericUtil::the().open_link_inside_browser(lnk_label);
			}
		});
}

void CUIMenu::render_menu_decoration(const Vector2D& window_pos, const Vector2D& window_size)
{
	// menu and tab select separator
	g_gui_window_rendering_i->render_line(g_gui_window_rendering_i->get_current_drawlist(),
										  { window_pos.x + CMenuStyle::k_tab_select_width, window_pos.y - 0.5f },
										  { window_pos.x + CMenuStyle::k_tab_select_width, window_pos.y + window_size.y },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_Separator));


	// top side menu separator
	g_gui_window_rendering_i->render_line(g_gui_window_rendering_i->get_current_drawlist(),
										  { window_pos.x + CMenuStyle::k_tab_select_width, window_pos.y + CMenuStyle::k_top_region_size_h },
										  { window_pos.x + window_size.x, window_pos.y + CMenuStyle::k_top_region_size_h },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_Separator));

	//
	// render topside contents
	//
	auto topside_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular);

	auto version_label = std::format("Version: {}", OXVER_STRING);
	g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(),
										  topside_font,
										  { window_pos.x + CMenuStyle::k_tab_select_width + 5.0f, window_pos.y + 0.0f + 3.0f },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark),
										  version_label);

	auto build_label = std::format("Build: {}", OX_BUILD);
	g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(),
										  topside_font,
										  { window_pos.x + CMenuStyle::k_tab_select_width + 5.0f, window_pos.y + 15.0f + 3.0f },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark),
										  build_label);

	auto compiletm_label = std::format("Compiled at: {}", OX_COMPILE_TIMESTAMP);
	g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(),
										  topside_font,
										  { window_pos.x + CMenuStyle::k_tab_select_width + 5.0f, window_pos.y + 30.0f + 3.0f },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark),
										  compiletm_label);

	// bottom separator
	g_gui_window_rendering_i->render_line(g_gui_window_rendering_i->get_current_drawlist(),
										  { window_pos.x + CMenuStyle::k_tab_select_width + 1.0f, window_pos.y + (window_size.y - CMenuStyle::k_bottom_reserved_rect_h - 1.0f) },
										  { window_pos.x + window_size.x,  window_pos.y + (window_size.y - CMenuStyle::k_bottom_reserved_rect_h - 1.0f) },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_Separator));

	// bottom background
	g_gui_window_rendering_i->render_box(g_gui_window_rendering_i->get_current_drawlist(),
										 { window_pos.x + CMenuStyle::k_tab_select_width + 1.0f, window_pos.y + (window_size.y - CMenuStyle::k_bottom_reserved_rect_h) },
										  { window_pos.x + window_size.x, window_pos.y + (window_size.y - 1.0f) },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_ChildBackground));

	auto segoeui_small = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Bold);

	// bottom right timestamp label
	auto timestamp_label = CStringTools::the().format_date(std::chrono::system_clock::now());
	auto timestamp_label_size = g_gui_fontmgr_i->calc_font_text_size(segoeui_small, timestamp_label.c_str());
	g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(),
										  segoeui_small,
										  {
											  window_pos.x + (window_size.x - CMenuStyle::k_bottom_right_timestamp_rightside_padding - timestamp_label_size.x),
											  window_pos.y + (window_size.y - CMenuStyle::k_bottom_right_timestamp_bottomside_padding - timestamp_label_size.y)
										  },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark),
										  timestamp_label);

	// bottom-left description
	auto desc_label = CMenuWidgetDescription::the().get_desc();
	auto desc_label_size = g_gui_fontmgr_i->calc_font_text_size(segoeui_small, desc_label);
	g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(),
										  segoeui_small,
										  {
											  window_pos.x + CMenuStyle::k_tab_select_width + CMenuStyle::k_bottom_left_desc_padding.x,
											  window_pos.y + (window_size.y - CMenuStyle::k_bottom_left_desc_padding.y - desc_label_size.y)
										  },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark),
										 desc_label);

	// Unload button
	Vector2D unload_button_pos = Vector2D(window_size.x - CMenuStyle::k_unload_button_size.x - CMenuStyle::k_unload_button_padding.x, CMenuStyle::k_unload_button_padding.y);
	g_gui_widgets_i->set_cursor_pos(unload_button_pos);

	if (g_gui_widgets_i->add_button("manually unload", CMenuStyle::k_unload_button_size, false, BUTTONFLAG_CenterLabel))
	{
		// end next frame.
		CoXWARE::the().end_cheat_execution();
	}

	// About button
	Vector2D about_button_pos = unload_button_pos - Vector2D(CMenuStyle::k_about_button_size.x + 5.0f, 0.0f);
	g_gui_widgets_i->set_cursor_pos(about_button_pos);

	if (g_gui_widgets_i->add_button("about", CMenuStyle::k_about_button_size, false, BUTTONFLAG_CenterLabel))
	{
		COxWareUI::the().create_welcome_popup();
	}
}

//---------------------------------------------------------------------------------------------------

void CUIMenu::tab_exploits()
{
	if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
	{
		g_gui_widgets_i->goto_next_column();

		CUIMenuWidgets::the().add_menu_child_collapsible(
			"Bypass game constrains", CMenuStyle::calc_child_size(290), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[]()
			{
				
			});

		g_gui_widgets_i->goto_next_column();

		CUIMenuWidgets::the().add_menu_child_collapsible(
			"Frame skip", CMenuStyle::calc_child_size(155), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[]()
			{
				
			});

		CUIMenuWidgets::the().add_menu_child_collapsible(
			"Consistency bypass", CMenuStyle::calc_child_size(135), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[]()
			{
				
			});

		CUIMenuWidgets::the().add_menu_child_collapsible(
			"Fake latency", CMenuStyle::calc_child_size(85), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[]()
			{
				
			});

		g_gui_widgets_i->end_columns();
	}

	CUIMenuWidgets::the().add_menu_child_collapsible(
		"Lie to the server", CMenuStyle::child_full_width(400.0f), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[this]()
		{
			
		});
}

void CUIMenu::tab_movement()
{
	if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
	{
		g_gui_widgets_i->goto_next_column();

		CUIMenuWidgets::the().add_menu_child_collapsible(
			"Air stuck", CMenuStyle::calc_child_size(150), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[]()
			{
				
			});

		CUIMenuWidgets::the().add_menu_child_collapsible(
			"Visualization", CMenuStyle::calc_child_size(170), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[]()
			{
				
			});

		CUIMenuWidgets::the().add_menu_child_collapsible(
			"Ground strafe", CMenuStyle::calc_child_size(300), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[]()
			{
				
			});

		CUIMenuWidgets::the().add_menu_child_collapsible(
		"Strafe hack", CMenuStyle::calc_child_size(225), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[]()
		{
			
		});

		CUIMenuWidgets::the().add_menu_child_collapsible(
		"Strafe helper", CMenuStyle::calc_child_size(240), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[]()
		{
			
		});

		g_gui_widgets_i->goto_next_column();

		CUIMenuWidgets::the().add_menu_child_collapsible(
		"Bunny hop", CMenuStyle::calc_child_size(300), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[]()
		{
			
		});

		CUIMenuWidgets::the().add_menu_child_collapsible(
		"Edge bug", CMenuStyle::calc_child_size(290), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[]()
		{
			
		});

		CUIMenuWidgets::the().add_menu_child_collapsible(
		"Fast run", CMenuStyle::calc_child_size(200), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[]()
		{
			
		});

		CUIMenuWidgets::the().add_menu_child_collapsible(
		"Auto JOF", CMenuStyle::calc_child_size(200), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[]()
		{
			

		g_gui_widgets_i->end_columns();
	}
}

void CUIMenu::tab_miscellaneous2()
{
	g_gui_widgets_i->add_text(__FUNCTION__ "()");
}

void CUIMenu::tab_miscellaneous3()
{
	g_gui_widgets_i->add_text(__FUNCTION__ "()");
}

void CUIMenu::tab_config()
{
	CUIMenuWidgets::the().add_menu_child(
		"Configuration", CMenuStyle::child_full_width(-1.0f), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[]()
		{
			
		});
}

void CUIMenu::tab_binds()
{
	CUIKeyBinding::the().render_interactible_bind_list();
}

void CUIMenu::tab_incommands()
{
	CUIInCommandKeyBinding::the().render_interactible_list();
}

void CUIMenu::tab_cmdlist()
{
	CUIMenuWidgets::the().add_menu_child(
		"Command list", CMenuStyle::child_full_width(-1.0f), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[]()
		{
			static char buffer[256];
			static bool once = false;
			if (!once)
			{
				strcpy_s(buffer, "");
				once = true;
			}

			if (g_gui_widgets_i->add_text_input_ex("Search for a variable", buffer, sizeof(buffer),
												   Vector2D(-1.0f, 0.0f)))
			{
			};

			bool should_filter = buffer[0];

			g_gui_widgets_i->add_table(
				"command_list", 4, 
				ImGuiTableFlags_HeaderTextOnly | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY,
				[]()
				{
					static auto flags = ImGuiTableColumnFlags_NoReorder;

					g_gui_widgets_i->table_setup_column_fixed_width("N", 25.0f, flags);
					g_gui_widgets_i->table_setup_column("Name", flags);
					g_gui_widgets_i->table_setup_column("Usage", flags);
					g_gui_widgets_i->table_setup_column("Description", flags);

					g_gui_widgets_i->table_headers_row();
				}, 
				[&]()
				{
					auto small_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular);
					g_gui_widgets_i->push_font(small_font);

					int n = 0;
					g_variablemgr_i->for_each_command(
						[&](BaseCommand* cmd)
						{
							if (should_filter && !strstr(cmd->get_name(), buffer))
							{
								return;
							}
							
							g_gui_widgets_i->table_next_column();
							g_gui_widgets_i->add_text(std::format("{}", ++n));
							g_gui_widgets_i->table_next_column();
							g_gui_widgets_i->add_text(cmd->get_name());
							g_gui_widgets_i->table_next_column();
							g_gui_widgets_i->add_text(cmd->has_usage() ? cmd->get_usage() : "");
							g_gui_widgets_i->table_next_column();
							g_gui_widgets_i->add_text(cmd->get_description());
						});

					g_gui_widgets_i->pop_font();

				});
		});
}

void CUIMenu::tab_varlist()
{
	CUIMenuWidgets::the().add_menu_child(
		"Variable list", CMenuStyle::child_full_width(-1.0f), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[]()
		{
			static char buffer[256];
			static bool once = false;
			if (!once)
			{
				strcpy_s(buffer, "");
				once = true;
			}

			if (g_gui_widgets_i->add_text_input_ex("Search for a variable", buffer, sizeof(buffer),
												   Vector2D(-1.0f, 0.0f)))
			{
			};

			bool should_filter = buffer[0];

			g_gui_widgets_i->add_table(
				"variable_list", 7,
				ImGuiTableFlags_HeaderTextOnly | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY,
				[]()
				{
					static auto flags = ImGuiTableColumnFlags_NoReorder;

					g_gui_widgets_i->table_setup_column_fixed_width("N", 25.0f, flags);
					g_gui_widgets_i->table_setup_column("Name", flags);
					g_gui_widgets_i->table_setup_column_fixed_width("Value", 60.0f, flags);
					g_gui_widgets_i->table_setup_column_fixed_width("Default", 60.0f, flags);
					g_gui_widgets_i->table_setup_column_fixed_width("Min", 60.0f, flags);
					g_gui_widgets_i->table_setup_column_fixed_width("Max", 60.0f, flags);
					g_gui_widgets_i->table_setup_column("Description", flags);

					g_gui_widgets_i->table_headers_row();
				},
				[&]()
				{
					auto small_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular);
					g_gui_widgets_i->push_font(small_font);

					int n = 0;
					g_variablemgr_i->for_each_variable(
						[&](BaseVariable* var)
						{
							if (should_filter && !strstr(var->get_name(), buffer))
							{
								return;
							}

							g_gui_widgets_i->table_next_column();
							g_gui_widgets_i->add_text(std::format("{}", ++n));
							g_gui_widgets_i->table_next_column();
							g_gui_widgets_i->add_text(var->get_name());
							g_gui_widgets_i->table_next_column();
							g_gui_widgets_i->add_text(var->get_value_string());
							g_gui_widgets_i->table_next_column();
							g_gui_widgets_i->add_text(var->get_default_value_string());
							g_gui_widgets_i->table_next_column();
							g_gui_widgets_i->add_text(var->get_min_string());
							g_gui_widgets_i->table_next_column();
							g_gui_widgets_i->add_text(var->get_max_string());
							g_gui_widgets_i->table_next_column();
							g_gui_widgets_i->add_text(var->get_description());
						});

					g_gui_widgets_i->pop_font();

				});
		});
}

void CUIMenu::tab_others()
{
	if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
	{
		g_gui_widgets_i->goto_next_column();

		CUIMenuWidgets::the().add_menu_child_collapsible(
			"UI", CMenuStyle::calc_child_size(180), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[]()
			{
				CUIMenuWidgets::the().add_checkbox("Enable rain", &ui_background_rain);
				CUIMenuWidgets::the().add_checkbox("Enable background fade", &ui_background_fade);

				g_gui_widgets_i->add_separtor_with_text("Menu");
				CUIMenuWidgets::the().add_checkbox("Resizable (beta)", &ui_menu_resizable);

				g_gui_widgets_i->add_separtor_with_text("Feature list");
				CUIMenuWidgets::the().feature_enabled_section(
				&ui_render_feature_list,
				[]()
				{
					CUIMenuWidgets::the().add_listbox("Font type", &ui_feature_list_font, { "Bold", "Regular" });
					CUIMenuWidgets::the().add_listbox("Font size", &ui_feature_list_font_size, { "Smaller", "Bigger" });

					CUIMenuWidgets::the().add_checkbox("Check for overlow", &ui_feature_list_overflow);
				}, "Show");

				g_gui_widgets_i->add_separtor_with_text("Fonts");

				static bool no_hinting = g_gui_fontmgr_i->get_freetype_builder_flags() & FreeTypeBuilderFlags_NoHinting;
				static bool no_auto_hint = g_gui_fontmgr_i->get_freetype_builder_flags() & FreeTypeBuilderFlags_NoAutoHint;
				static bool force_auto_hint = g_gui_fontmgr_i->get_freetype_builder_flags() & FreeTypeBuilderFlags_ForceAutoHint;
				static bool light_hinting = g_gui_fontmgr_i->get_freetype_builder_flags() & FreeTypeBuilderFlags_LightHinting;
				static bool mono_hinting = g_gui_fontmgr_i->get_freetype_builder_flags() & FreeTypeBuilderFlags_MonoHinting;

				if (g_gui_widgets_i->add_checkbox("No hinting", &no_hinting)) g_gui_fontmgr_i->add_freetype_builder_flags(FreeTypeBuilderFlags_NoHinting, no_hinting);
				if (g_gui_widgets_i->add_checkbox("No auto-hint", &no_auto_hint)) g_gui_fontmgr_i->add_freetype_builder_flags(FreeTypeBuilderFlags_NoAutoHint, no_auto_hint);
				if (g_gui_widgets_i->add_checkbox("Force auto-hint", &force_auto_hint)) g_gui_fontmgr_i->add_freetype_builder_flags(FreeTypeBuilderFlags_ForceAutoHint, force_auto_hint);
				if (g_gui_widgets_i->add_checkbox("Light hinting", &light_hinting)) g_gui_fontmgr_i->add_freetype_builder_flags(FreeTypeBuilderFlags_LightHinting, light_hinting);
				if (g_gui_widgets_i->add_checkbox("Mono hinting", &mono_hinting)) g_gui_fontmgr_i->add_freetype_builder_flags(FreeTypeBuilderFlags_MonoHinting, mono_hinting);
			});

		CUIMenuWidgets::the().add_menu_child_collapsible(
			"Debug", CMenuStyle::calc_child_size(300), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[]()
			{
				CUIMenuWidgets::the().feature_enabled_section(
				&debug,
				[]()
				{
					CUIMenuWidgets::the().add_description_text("This feature is used for developement only. You have been warned.");

					g_gui_widgets_i->begin_tab("debug_tab", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll);

					float tab_height = -1.0f;

					g_gui_widgets_i->add_tab_item(
						"Rendering", false,
						{ -1.0f, tab_height },
						[]()
						{
							CUIMenuWidgets::the().feature_enabled_section(
							&debug_render_info,
							[]()
							{
								CUIMenuWidgets::the().add_checkbox("Misc", &debug_render_info_misc);
								g_gui_widgets_i->add_spacing();
								g_gui_widgets_i->add_separtor_with_text("Movement");
								g_gui_widgets_i->add_spacing();
								CUIMenuWidgets::the().add_checkbox("Bhop", &debug_render_info_movement_bhop);
								CUIMenuWidgets::the().add_checkbox("Strafe hack", &debug_render_info_movement_strafe);
								CUIMenuWidgets::the().add_checkbox("Strafe helper", &debug_render_info_movement_strafe_helper);
							});
						});

					g_gui_widgets_i->end_tab();
				});
			});

		g_gui_widgets_i->goto_next_column();
		
		CUIMenuWidgets::the().add_menu_child_collapsible(
			"Storage", CMenuStyle::calc_child_size(200), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[]()
			{
				auto font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular);

				auto cheat_directory = g_filesystem_i->get_appdata_path();

				static uintmax_t cheat_directory_size = 0, logs_directory_size = 0, cfg_directory_size = 0;

				static uint32_t t1 = GetTickCount();

				if (GetTickCount() - t1 > 250)
				{
					cheat_directory_size = g_filesystem_i->directory_size(cheat_directory);
					logs_directory_size = g_filesystem_i->directory_size(cheat_directory / "log");
					cfg_directory_size = g_filesystem_i->directory_size(cheat_directory / "config");

					t1 = GetTickCount();
				}

				g_gui_widgets_i->add_text(std::format("Currently occupied space on your computer: {}", 
													  CStringTools::the().pretify_file_size((float)cheat_directory_size)), TEXTPROP_Wrapped, font);

				g_gui_widgets_i->add_separator();
				
				g_gui_widgets_i->add_text(std::format("logs: {}",
													  CStringTools::the().pretify_file_size((float)logs_directory_size)), TEXTPROP_Wrapped, font);
				g_gui_widgets_i->add_text(std::format("configs: {}", 
													  CStringTools::the().pretify_file_size((float)cfg_directory_size)), TEXTPROP_Wrapped, font);

				g_gui_widgets_i->add_separator();

				if (g_gui_widgets_i->add_button("open cheat storage directory", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
				{
					CGenericUtil::the().open_folder_inside_explorer(cheat_directory.string());
				}

				if (g_gui_widgets_i->add_button("clear unnecessary files", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
				{
					g_filesystem_i->iterate_through_files(
						cheat_directory / "log", false, 
						[](const FilePath_t& dir)
						{
							if (g_filesystem_i->is_file(dir))
							{
								g_filesystem_i->remove(dir);
							}
						});
				}

				VarInteger* var = (VarInteger*)g_variablemgr_i->query_variable("num_logfiles_to_keep");

				if (var)
				{
					g_gui_widgets_i->add_spacing();
					CUIMenuWidgets::the().add_slider("Logfiles to keep", "%0.0f", var);
				}
			});

		g_gui_widgets_i->end_columns();
	}
}

//----------------------------------------------------------------------------------------------------------------------

void MenuChilden::World::Removals::contents()
{
	CUIMenuWidgets::the().add_checkbox("Remove screenshake", &remove_screenshake);
	CUIMenuWidgets::the().add_checkbox("Remove viewmodel", &remove_viewmodel);
	CUIMenuWidgets::the().add_checkbox("Remove MOTD", &remove_motd);
	CUIMenuWidgets::the().add_checkbox("Remove sniper scopes", &remove_sniper_scope);

	g_gui_widgets_i->add_separtor_with_text("Remove players");
	CUIMenuWidgets::the().add_checkbox("All", &remove_players_all);

	if (remove_players_all.get_value())
	{
		g_gui_widgets_i->push_disabled();
	}

	if (g_gui_widgets_i->begin_columns("player_removals", 2))
	{
		g_gui_widgets_i->goto_next_column();

		CUIMenuWidgets::the().add_checkbox("Enemy", &remove_players_enemy);
		CUIMenuWidgets::the().add_checkbox("Teammates", &remove_players_teammates);

		g_gui_widgets_i->goto_next_column();

		CUIMenuWidgets::the().add_checkbox("Ts", &remove_players_t);
		CUIMenuWidgets::the().add_checkbox("CTs", &remove_players_ct);

		g_gui_widgets_i->end_columns();
	}

	if (remove_players_all.get_value())
	{
		g_gui_widgets_i->pop_disabled();
	}

	g_gui_widgets_i->add_spacing();
	CUIMenuWidgets::the().add_description_text("In order to disable in-game fog, use \"gl_fog\" command.");

	g_gui_widgets_i->add_separtor_with_text("Remove HUD");

	CUIMenuWidgets::the().feature_enabled_section(
	&remove_hud_enable,
	[]()
	{
		g_gui_widgets_i->add_padding({ 0, 5.0f });

		if (g_gui_widgets_i->begin_columns("removals_hud", 2))
		{
			g_gui_widgets_i->goto_next_column();
			CUIMenuWidgets::the().add_checkbox("Weapons", &remove_hud_weapons);
			CUIMenuWidgets::the().add_checkbox("Crosshair", &remove_hud_crosshair);
			CUIMenuWidgets::the().add_checkbox("Flashlight", &remove_hud_flashlight);

			g_gui_widgets_i->goto_next_column();

			CUIMenuWidgets::the().add_checkbox("Health", &remove_hud_health);
			CUIMenuWidgets::the().add_checkbox("Timer", &remove_hud_timer);
			CUIMenuWidgets::the().add_checkbox("Money", &remove_hud_money);

			g_gui_widgets_i->end_columns();
		}
	});
}

void MenuChilden::World::Thirdperson::contents()
{
	g_gui_widgets_i->add_spacing();
	g_gui_widgets_i->add_spacing();

	CUIMenuWidgets::the().add_slider("Distance", "%0.0f units", &thirdperson_dist);
	CUIMenuWidgets::the().add_checkbox("Block wall", &thirdperson_block_wall);
}

void MenuChilden::World::AntiDemoRecorder::contents()
{
	CUIMenuWidgets::the().add_checkbox("Enable", &anti_demo_recorder_enable);

	CUIMenuWidgets::the().add_description_text(
		"If enabled, server will not be able to record demos on your game.\n"
		"You'll be notified every time server wants to start a demo inside the console.");
}

void MenuChilden::World::AntiScreen::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&antiscreen_enable,
	[]()
	{
		CUIMenuWidgets::the().add_slider("Frames to wait", "%0.0f second(s)", &antiscreen_time);
	});
}

void MenuChilden::World::Automation::contents()
{
	CUIMenuWidgets::the().add_checkbox("Auto reload", &auto_reload);
	CUIMenuWidgets::the().add_checkbox("Auto pistol", &auto_pistol);
	CUIMenuWidgets::the().add_checkbox("Auto zoom", &auto_zoom);
}

void MenuChilden::World::SpeedControl::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&speed_control_enable,
	[]()
	{
		if (g_gui_widgets_i->begin_columns("speed_control", 2))
		{
			g_gui_widgets_i->goto_next_column();
			CUIMenuWidgets::the().add_checkbox("Invert", &speed_control_invert);

			g_gui_widgets_i->goto_next_column();
			CUIMenuWidgets::the().add_checkbox("Negative", &speed_control_negative);

			g_gui_widgets_i->end_columns();
		}

		g_gui_widgets_i->add_text("Engine");
		CUIMenuWidgets::the().add_slider_nolabel("Amount", "%0.1f", &speed_control_amount_engine, "off");

		g_gui_widgets_i->add_text("Server");
		CUIMenuWidgets::the().add_slider_nolabel("Amount##1", "%0.0f", &speed_control_amount_server, "off");

		g_gui_widgets_i->add_text("Net");
		CUIMenuWidgets::the().add_slider_nolabel("Amount##2", "%0.01f", &speed_control_amount_net, "off");
	});
}

void MenuChilden::World::ViewmodelOffset::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&viewmodel_offset_enable,
	[]()
	{
		CUIMenuWidgets::the().add_slider("Amount", "%0.1f", &viewmodel_offset_value);
	});
}

void MenuChilden::World::Backtrack::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&backtrack_enable,
	[]()
	{
		CUIMenuWidgets::the().add_listbox("Team", &backtrack_team, { "Ts", "CTs", "Teammates", "Enemy", "Both" });
	});
}

void MenuChilden::World::SmokeVisuals::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&smoke_visuals,
	[]()
	{
		CUIMenuWidgets::the().add_color_edit("Color", &smoke_color);
		CUIMenuWidgets::the().add_checkbox("Rainbow smoke", &smoke_rainbow);

		CUIMenuWidgets::the().add_slider("Opacity", "%0.0f %%", &smoke_opacity);

		CUIMenuWidgets::the().add_description_text("Everything changed here will take effect only on new smoke creation.");
	});
}

void MenuChilden::World::NonSteamFpsFix::contents()
{
	CUIMenuWidgets::the().add_checkbox("Enable", &nonsteam_fps_fix);

	CUIMenuWidgets::the().add_description_text(
		"This fixes the FPS movement dependence, where with higher fps you would get slomotion movement and \"higher\" gravity."
		"\nFor more technical details, follow this link:");

	g_gui_widgets_i->push_font(g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Regular));
	if (g_gui_widgets_i->add_hypertext_link("github.com/ValveSoftware/halflife/issues/1940"))
	{
		CGenericUtil::the().open_link_inside_browser("https://github.com/ValveSoftware/halflife/issues/1940");
	}
	g_gui_widgets_i->pop_font();
}

void MenuChilden::World::WorldVisuals::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&world_visuals_enable,
	[]()
	{
		if (g_gui_widgets_i->begin_columns("world_visuals", 2))
		{
			g_gui_widgets_i->goto_next_column();
			CUIMenuWidgets::the().add_checkbox("On sky", &world_visuals_dimlight_sky);
			g_gui_widgets_i->goto_next_column();
			CUIMenuWidgets::the().add_checkbox("On world", &world_visuals_dimlight_world);

			g_gui_widgets_i->end_columns();
		}
		CUIMenuWidgets::the().add_slider("Dimlight", "%0.0f %%", &world_visuals_dimlight, NULL, "off");

		CUIMenuWidgets::the().add_checkbox("Rainbow world", &world_visuals_rainbow);

		g_gui_widgets_i->add_separtor_with_text("World fog");

		CUIMenuWidgets::the().feature_enabled_section(
		&world_visuals_fog, &world_visuals_fog_color,
		[]()
		{
			CUIMenuWidgets::the().add_slider("Density", "%0.1f", &world_visuals_fog_density);
		});
	});
}

void MenuChilden::World::EnvironmentalEffects::contents()
{
	CUIMenuWidgets::the().feature_enabled_section(
	&env_enable,
	[]()
	{
		CUIMenuWidgets::the().add_slider("Generate radius", "%0.0f units", &env_radius);

		g_gui_widgets_i->add_separtor_with_text("Ground fog");
		CUIMenuWidgets::the().feature_enabled_section(
		&env_ground_fog,
		[]()
		{
			CUIMenuWidgets::the().add_slider("Density", "%0.0f", &env_ground_fog_density);
			CUIMenuWidgets::the().add_slider("Size", "%0.1fx", &env_ground_fog_size);
		});

		g_gui_widgets_i->add_separtor_with_text("Other");
		g_gui_widgets_i->add_spacing();
		CUIMenuWidgets::the().add_slider("Wind speed", "%0.1fx", &env_wind_speed);
		CUIMenuWidgets::the().add_slider("Fall speed", "%0.1fx", &env_particle_fallspeed);

		g_gui_widgets_i->begin_tab("env_effects", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll);

		float tab_height = 100.0f;

		g_gui_widgets_i->add_tab_item(
			"Rain", false,
			{ -1.0f, tab_height },
			[]()
			{
				CUIMenuWidgets::the().feature_enabled_section(
				&env_rain,
				[]()
				{
					CUIMenuWidgets::the().add_slider("Density", "%0.1f", &env_rain_density);
					CUIMenuWidgets::the().add_checkbox("Ambient sound", &env_rain_ambient);
					CUIMenuWidgets::the().add_checkbox("Occasional Thunder", &env_rain_ambient_thunder);
				});
			});

		g_gui_widgets_i->add_tab_item(
			"Snow", false,
			{ -1.0f, tab_height },
			[]()
			{
				CUIMenuWidgets::the().feature_enabled_section(
				&env_snow,
				[]()
				{
					CUIMenuWidgets::the().add_slider("Density", "%0.1f", &env_snow_density);
					CUIMenuWidgets::the().add_slider("Flake size", "%0.0fx", &env_snow_flake_size);
					CUIMenuWidgets::the().add_slider("Die time", "%0.1f seconds", &env_snow_flake_die_time);
				});
			});

		g_gui_widgets_i->end_tab();
				});
}

//----------------------------------------------------------------------------------------------------------------------

void MenuChilden::Rendering::FieldOfView::contents()
{
	
}

//----------------------------------------------------------------------------------------------------------------------

void MenuChilden::Exploits::ServerCommandFilter::contents()
{
	if (g_gui_widgets_i->begin_columns("server_cmd_filter", 3))
	{
		g_gui_widgets_i->setup_column_fixed_width(100);
		g_gui_widgets_i->setup_column_fixed_width(150);

		g_gui_widgets_i->goto_next_column();

		CUIMenuWidgets::the().add_checkbox("Enable", &cmdfilter_enable);
		CUIMenuWidgets::the().add_checkbox("Filter all", &cmdfilter_filter_all);

		g_gui_widgets_i->goto_next_column();

		CUIMenuWidgets::the().add_checkbox("Print blocked cmds", &cmdfilter_print_blocked);

		g_gui_widgets_i->goto_next_column();

		CUIMenuWidgets::the().add_checkbox("Print every cmds", &cmdfilter_print_every);

		g_gui_widgets_i->end_columns();
	}

	CUIMenuWidgets::the().add_description_text_ex(
		"This filter allows you to block commands that are send to you from the server."
		" You can add a list of commands separated by a comma \";\" that you want to block, such as:\n"
		"\"fps_max; bind; exit\" and such.",

		[&]()
		{
	g_gui_widgets_i->add_text(
		"How does it work",
		TEXTPROP_Wrapped,
		g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular));

	g_gui_widgets_i->add_text(
		"For example, often the server sends command to you such as \"fps_max 100; developer 0\" etc."
		" Therefore, you can add the command that you don't want to have executed by the server here."
		" In theory, this is the same as cl_filterstuffcmd however, more customizable.",
		TEXTPROP_Wrapped);
		}
	);

	g_gui_widgets_i->add_table(
		"cmdfilter_table", 2,
		ImGuiTableFlags_HeaderTextOnly,
		[&]()
		{
			static auto column_flags = ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize;
			g_gui_widgets_i->table_setup_column_fixed_width("Commands to be filtered", 350.0f, column_flags);

			g_gui_widgets_i->table_headers_row();
		},
		[&]()
		{
			g_gui_widgets_i->table_next_column();

			static char cmd_buffer[1024];
			static bool at_init = false;
			if (!at_init)
			{
				strcpy_s(cmd_buffer, cmdfilter_filtered_commands.get_value_string());
				at_init = true;
			}

			bool reclaim_focus_key = false;
			if (g_gui_widgets_i->add_text_input_ex("Commands to be filtered", cmd_buffer, sizeof(cmd_buffer),
												   Vector2D(-1.0f, 0.0f)))
			{
				reclaim_focus_key = true;
			};

			// Auto-focus on window apparition
			g_gui_widgets_i->set_item_default_focus();
			if (reclaim_focus_key)
			{
				g_gui_widgets_i->set_keyboard_focus_here(-1); // Auto focus previous widget
			}

			g_gui_widgets_i->table_next_column();

			if (g_gui_widgets_i->add_button("Apply", { -1.0f, 25.0f }, false, BUTTONFLAG_CenterLabel))
			{
				if (cmd_buffer[0])
				{
					cmdfilter_filtered_commands.set_value(cmd_buffer);
				}
			}
		});
}
