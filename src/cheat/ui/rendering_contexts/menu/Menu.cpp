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

//---------------------------------------------------------------------------------------------------

void CUIMenu::on_initialize()
{
	//
	// Visuals
	//
	{
		auto& group_visuals = m_tab_groups[MENU_TAB_GROUP_Visuals];
		group_visuals.initialize("Visuals");

		{
			auto& tab_world = group_visuals.m_tabs[MENU_TAB_World];
			tab_world.initialize("World", "World-related cheats");
			{
				auto& tab_world_main = tab_world.m_sections["main"];
				tab_world_main.m_children.push_back(new MenuChilden::World::Removals({ "Removals", 250, true }));
				tab_world_main.m_children.push_back(new MenuChilden::World::Thirdperson({ "Thirdperson", 125, true }));
				tab_world_main.m_children.push_back(new MenuChilden::World::AntiDemoRecorder({ "📼 Anti demo recorder", 125, true }));
				tab_world_main.m_children.push_back(new MenuChilden::World::AntiScreen({ "🖥 Anti screen", 90, true }));
				tab_world_main.m_children.push_back(new MenuChilden::World::Automation({ "Automation", 120, true }));
				tab_world_main.m_children.push_back(new MenuChilden::World::SpeedControl({ "🏃 Speed control", 225, true }));
				tab_world_main.m_children.push_back(new MenuChilden::World::ViewmodelOffset({ "Viewmodel offset", 90, true }));
				tab_world_main.m_children.push_back(new MenuChilden::World::Backtrack({ "👥 Backtrack", 125, true }));
				tab_world_main.m_children.push_back(new MenuChilden::World::SmokeVisuals({ "Smoke visuals", 180, true }));
				tab_world_main.m_children.push_back(new MenuChilden::World::NonSteamFpsFix({ "Non-steam fps fix", 150, true }));
				tab_world_main.m_children.push_back(new MenuChilden::World::WorldVisuals({ "🌎 World visuals", 235, true }));
				tab_world_main.m_children.push_back(new MenuChilden::World::EnvironmentalEffects({ "⛈ Environmental effects", 300, true }));
			}
		}

		{
			auto& tab_rendering = group_visuals.m_tabs[MENU_TAB_Rendering];
			tab_rendering.initialize("Rendering", "Rendering-related cheats");
			{
				auto& tab_rendering_main = tab_rendering.m_sections["main"];
				tab_rendering_main.m_children.push_back(new MenuChilden::Rendering::FieldOfView({ "🕶 Field of view", 85, true }));
				tab_rendering_main.m_children.push_back(new MenuChilden::Rendering::AspectRatio({ "Aspect ratio", 85, true }));
				tab_rendering_main.m_children.push_back(new MenuChilden::Rendering::StudioRenderer({ "Studio renderer", 300, true }));
				tab_rendering_main.m_children.push_back(new MenuChilden::Rendering::ModelChams({ "Model chams", 350, true }));
				tab_rendering_main.m_children.push_back(new MenuChilden::Rendering::HUDRendering({ "HUD rendering", 215, true }));
				tab_rendering_main.m_children.push_back(new MenuChilden::Rendering::Panic({ "Panic", 85, true }));
			}
		}

		{
			auto& tab_screen = group_visuals.m_tabs[MENU_TAB_Screen];
			tab_screen.initialize("Screen", "Screen-related cheats");
			{
				auto& tab_screen_main = tab_screen.m_sections["main"];
				tab_screen_main.m_children.push_back(new MenuChilden::Screen::FlashbangFade({ "Flashbang fade", 90, true }));
				tab_screen_main.m_children.push_back(new MenuChilden::Screen::ESP({ "🔳 ESP", 290, true }));
				tab_screen_main.m_children.push_back(new MenuChilden::Screen::CustomCrosshair({ "➕ Custom crosshair", 265, true }));
				tab_screen_main.m_children.push_back(new MenuChilden::Screen::ClShowFPSEnhancement({ "cl_showfps enhancement", 130, true }));
				tab_screen_main.m_children.push_back(new MenuChilden::Screen::MovementInfo({ "Movement info", 150, true }));
			}
		}
	}

	//
	// Miscellanous
	//
	{
		auto& group_misc = m_tab_groups[MENU_TAB_GROUP_Miscellaneous];
		group_misc.initialize("Miscellaneous");

		{
			auto& tab_exploits = group_misc.m_tabs[MENU_TAB_Exploits];
			tab_exploits.initialize("Exploits", "Various game exploitations");
			{
				auto& tab_exploits_servercmdfilter = tab_exploits.m_sections["Server Command Filter"];
				tab_exploits_servercmdfilter.initialize("Server Command filter");
				tab_exploits_servercmdfilter.m_children.push_back(new MenuChilden::Exploits::ServerCommandFilter({ "Server command filter", -1, true, MCH_SameAsWidth }));
			}
			{
				auto& tab_exploits_cvarsandbox = tab_exploits.m_sections["Cvar sandbox"];
				tab_exploits_cvarsandbox.initialize("Cvar sandbox");
				tab_exploits_cvarsandbox.m_children.push_back(new MenuChilden::Exploits::CvarSandbox({ "Cvar sandbox", -1, true, MCH_SameAsWidth }));
			}
			{
				auto& tab_exploits_other = tab_exploits.m_sections["Other"];
				tab_exploits_other.initialize("Other");
				tab_exploits_other.m_children.push_back(new MenuChilden::Exploits::BypassGameConstrains({ "Bypass game constrains", 290, true }));
				tab_exploits_other.m_children.push_back(new MenuChilden::Exploits::FrameSkipper({ "Frame skipper", 215, true }));
				tab_exploits_other.m_children.push_back(new MenuChilden::Exploits::ConsistencyBypass({ "Consistency bypass", 135, true }));
				tab_exploits_other.m_children.push_back(new MenuChilden::Exploits::FakeLatency({ "Fake latency", 85, true }));
				tab_exploits_other.m_children.push_back(new MenuChilden::Exploits::SIDSpoofer({ "SID Spoofer", 160, true }));
			}
		}

		{
			auto& tab_movement = group_misc.m_tabs[MENU_TAB_Movement];
			tab_movement.initialize("Movement", "Kreedz/Movement cheats");
			{
				auto& tab_movement_main = tab_movement.m_sections["main"];
				tab_movement_main.m_children.push_back(new MenuChilden::Movement::AirStuck({ "Air stuck", 150, true }));
				tab_movement_main.m_children.push_back(new MenuChilden::Movement::Visualization({ "Visualization", 170, true }));
				tab_movement_main.m_children.push_back(new MenuChilden::Movement::GroundStrafe({ "Ground strafe", 300, true }));
				tab_movement_main.m_children.push_back(new MenuChilden::Movement::Strafehack({ "Strafe hack", 225, true }));
				tab_movement_main.m_children.push_back(new MenuChilden::Movement::StrafeHelper({ "Strafe helper", 220, true }));
				tab_movement_main.m_children.push_back(new MenuChilden::Movement::Bunnyhop({ "🐇 Bunnyhop", 300, true }));
				tab_movement_main.m_children.push_back(new MenuChilden::Movement::Edgebug({ "Edge bug", 270, true }));
				tab_movement_main.m_children.push_back(new MenuChilden::Movement::Fastrun({ "🏃 Fast run", 200, true }));
				tab_movement_main.m_children.push_back(new MenuChilden::Movement::AutoJOF({ "Auto JOF", 200, true }));
			}
		}

		{
			auto& tab_plrlist = group_misc.m_tabs[MENU_TAB_PlayerList];
			tab_plrlist.initialize("Player list", "List of all players");
			{
				auto& tab_plrlist_main = tab_plrlist.m_sections["main"];
				tab_plrlist_main.m_children.push_back(new MenuChilden::PlayerList::PlayerList_({ "Player list", -1, false, MCH_2_5x, MCHILDF_DontApplyFilter }));
			}
		}
	}

	//
	// Configuration
	//
	{
		auto& group_config = m_tab_groups[MENU_TAB_GROUP_Configuration];
		group_config.initialize("Configuration");

		{
			auto& tab_config = group_config.m_tabs[MENU_TAB_Config];
			tab_config.initialize("Config", "Config list/manager");
			{
				auto& tab_config_main = tab_config.m_sections["main"];
				tab_config_main.m_children.push_back(new MenuChilden::Configuration::Config({ "⚙ Configuration", -1, false, MCH_SameAsWidth, MCHILDF_DontApplyFilter }));
			}
		}

		{
			auto& tab_theme = group_config.m_tabs[MENU_TAB_Theme];
			tab_theme.initialize("Theme", "Theme editor");
			{
				auto& tab_theme_main = tab_theme.m_sections["main"];
				tab_theme_main.m_children.push_back(new MenuChilden::Configuration::Theme({ "Theme", -1, false, MCH_SameAsWidth, MCHILDF_DontApplyFilter }));
			}
		}

		{
			auto& tab_language = group_config.m_tabs[MENU_TAB_Language];
			tab_language.initialize("Language", "Language selector");
			{
				auto& tab_language_main = tab_language.m_sections["main"];
				tab_language_main.m_children.push_back(new MenuChilden::Configuration::Language({ "Language", -1, false, MCH_SameAsWidth, MCHILDF_DontApplyFilter }));
			}
		}

		{
			auto& tab_binds = group_config.m_tabs[MENU_TAB_Binds];
			tab_binds.initialize("Bind manager", "Cheat key-bind systems");
			{
				auto& tab_binds_main = tab_binds.m_sections["main"];
				tab_binds_main.m_children.push_back(new MenuChilden::Configuration::KeyBinding({ "Key binding ⌨", -1, false, MCH_SameAsWidth, MCHILDF_DontApplyFilter }));
			}
		}


		{
			auto& tab_incommands = group_config.m_tabs[MENU_TAB_InCommands];
			tab_incommands.initialize("In-Commands", "In-Command manager");
			{
				auto& tab_incommands_main = tab_incommands.m_sections["main"];
				tab_incommands_main.m_children.push_back(new MenuChilden::Configuration::InCommands({ "In-Commands", -1, false, MCH_SameAsWidth, MCHILDF_DontApplyFilter }));
			}
		}
	}

	//
	// Others
	//
	{
		auto& group_others = m_tab_groups[MENU_TAB_GROUP_Other];
		group_others.initialize("Other");

		{
			auto& tab_varlist = group_others.m_tabs[MENU_TAB_VariableList];
			tab_varlist.initialize("Variable list", "Cheat variable list");
			{
				auto& tab_varlist_main = tab_varlist.m_sections["main"];
				tab_varlist_main.m_children.push_back(new MenuChilden::Other::VariableList({ "Variable list", -1, false, MCH_4x, MCHILDF_DontApplyFilter }));
			}
		}

		{
			auto& tab_cmdlist = group_others.m_tabs[MENU_TAB_CommandList];
			tab_cmdlist.initialize("Command list", "Cheat command list");
			{
				auto& tab_cmdlist_main = tab_cmdlist.m_sections["main"];
				tab_cmdlist_main.m_children.push_back(new MenuChilden::Other::CommandList({ "Command list", -1, false, MCH_4x, MCHILDF_DontApplyFilter }));
			}
		}

		{
			auto& tab_other = group_others.m_tabs[MENU_TAB_Others];
			tab_other.initialize("Other", "Other non-related");
			{
				auto& tab_other_main = tab_other.m_sections["main"];
				tab_other_main.m_children.push_back(new MenuChilden::Other::UI({ "UI", 350, false }));
				tab_other_main.m_children.push_back(new MenuChilden::Other::Debug({ "Debug", 320, false }));
				tab_other_main.m_children.push_back(new MenuChilden::Other::Storage({ "📂 Storage", 200, false }));
			}
		}
	}

//#ifndef _RETAIL
//	auto& debug = m_tab_groups["Debug"].m_tabs[MENU_TAB_Debug];
//	debug["main"].initialize("Debug", "Cheat debugging");
//	debug["main"].m_children.push_back(new MenuChilden::Debug::Debugging({ "Debugging", -1, false, MCH_4x, MCHILDF_DontApplyFilter }));
//#endif // _RETAIL
}

void CUIMenu::on_render()
{
	// we have to update it here since the keyscan buttons may be outside of the InCommands tab.
	CUIInCommandKeyBinding::the().update();

	// update search filter
	g_search_filter_context.update();

	m_menu_contents_changed = false;

	g_gui_widgets_i->set_next_window_pos({ 100, 100 }, ImGuiCond_Once);
	g_gui_widgets_i->set_next_window_size(MenuStyle::menu_rect_size, ImGuiCond_Once);
	g_gui_widgets_i->set_next_window_rounding(MenuStyle::rounding_factor, ImDrawFlags_RoundCornersTopRight | ImDrawFlags_RoundCornersBottomLeft);

	g_gui_widgets_i->set_next_window_size_constrains(MenuStyle::menu_rect_size, MenuStyle::menu_rect_size_max);

	auto window_flags =
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse;

	auto segoeui_extra = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_33px, FDC_Bold);

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

	CMenuWidgetDescription::the().clear_desc();

	g_gui_widgets_i->create_new_window(
		"menu", window_flags, [&]()
		{
			auto window_pos = g_gui_widgets_i->get_current_window_pos();
			auto window_size = g_gui_widgets_i->get_current_window_size();

			g_gui_widgets_i->set_next_window_rounding(MenuStyle::rounding_factor, ImDrawFlags_RoundCornersBottomLeft);
			g_gui_widgets_i->add_child(
				"menu_left",
				Vector2D(MenuStyle::tab_select_width, window_size.y),
				false,
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse,
				[&]()
				{
					auto child_pos = g_gui_widgets_i->get_current_window_pos();
					auto child_size = g_gui_widgets_i->get_current_window_size();

					const char* label_text = "OXWARE";
					auto label_size = g_gui_fontmgr_i->calc_font_text_size(segoeui_extra, label_text);

					g_gui_window_rendering_i->render_text(
						g_gui_window_rendering_i->get_current_drawlist(),
						segoeui_extra,
						{ child_pos.x + (child_size.x - label_size.x) / 2.0f, window_pos.y + (MenuStyle::top_region_size_h - label_size.y) / 2.0f },
						g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark),
						label_text);

					render_menu_tabs();
				}
			);

			//
			// now render contents of the menu.
			//

			g_gui_widgets_i->set_cursor_pos({ MenuStyle::tab_select_width + 1, MenuStyle::top_region_size_h + 1 });

			g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, MenuStyle::child_contents_padding);

			g_gui_widgets_i->add_child(
				"menu_contents",
				{
					window_size.x - MenuStyle::tab_select_width,
					window_size.y - MenuStyle::top_region_size_h - 1.0f - MenuStyle::bottom_reserved_rect_h - 1.0f,
				},
				true,
				ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_HorizontalScrollbar,
				[&]()
				{
					handle_menu_contents_rendering();
				});

			g_gui_widgets_i->pop_stylevar(); // window padding

			render_menu_decoration(window_pos, window_size);
		});

	render_github_repo_link_decor();

	g_gui_widgets_i->pop_stylevar(1); // window padding
}

void CUIMenu::on_destroy()
{
}

//---------------------------------------------------------------------------------------------------

CMenuSearchFilterContext g_search_filter_context;

//---------------------------------------------------------------------------------------------------

void IMenuChild::render()
{
	g_gui_widgets_i->set_cursor_pos(m_computed_position);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

	//
	// setup collapsible data
	//
	if (m_supports_collapse && m_is_collapsed)
	{
		window_flags |= ImGuiWindowFlags_NoScrollbar;
	}

	g_gui_widgets_i->push_color(ImGuiCol_ChildBg, g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_ChildBackground>());

	g_gui_widgets_i->add_child(
	m_header, Vector2D(m_child_size.x, m_is_collapsed ? k_collapsed_height : m_child_size.y),
	false, window_flags, 
	[this]()
	{
		auto child_pos = g_gui_widgets_i->get_current_window_pos();
		auto child_size = g_gui_widgets_i->get_current_window_size();

		auto header_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_24px, FDC_Regular);
		auto header_text_size = g_gui_fontmgr_i->calc_font_text_size(header_font, m_header.c_str());

		Vector2D header_text_pos = { child_pos.x + MenuStyle::childhdr_text_padding_x, child_pos.y + MenuStyle::childhdr_text_padding_y };

		auto header_color = (m_supports_collapse && m_is_hovered) ? 
			g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_URLHovered>() :
			g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_TextRegular>();

		// header text
		g_gui_window_rendering_i->render_text(
			g_gui_window_rendering_i->get_current_drawlist(),
			header_font,
			header_text_pos,
			header_color,
			m_header);

		// separator beneath
		g_gui_window_rendering_i->render_line(
			g_gui_window_rendering_i->get_current_drawlist(),
			{ child_pos.x + MenuStyle::childhdr_line_padding.x, child_pos.y + header_text_size.y + MenuStyle::childhdr_line_padding.y },
			{ child_pos.x + child_size.x - MenuStyle::childhdr_line_padding.x, child_pos.y + header_text_size.y + MenuStyle::childhdr_line_padding.y },
			g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_Separator>());

		if (m_supports_collapse)
		{
			Vector2D arrow_pos = child_pos + Vector2D(child_size.x - 21.0f, 7.0f);
			Vector2D arrow_size = { header_font->FontSize, header_font->FontSize }; // hack!

			bool pressed = g_gui_widgets_i->add_invisible_button_behaviour(m_header + "_btn", header_text_pos, (arrow_pos + arrow_size) - header_text_pos);

			m_is_hovered = g_gui_widgets_i->is_last_widget_hovered();
			if (m_is_hovered)
			{
				g_imgui_platform_layer_i->override_cursor(GUICURSOR_Hand);
			}

			if (pressed)
			{
				m_is_collapsed ^= 1; // flip

				// update the grid
				CMenuContentsGrid::the().force_precompute_next_frame();
			}

			// for debugging, draws a rect around the arrow hitbox.
			//window->DrawList->AddRect(bb.Min, bb.Max, ImColor(0, 230, 0, 230));

			g_gui_widgets_i->render_arrow(arrow_pos, header_color, m_is_collapsed ? ARROWDIR_Left : ARROWDIR_Down);
		}

		// render child contents if not collapsed
		if (!m_is_collapsed)
		{
			g_gui_widgets_i->set_cursor_pos(Vector2D(MenuStyle::childhdr_line_padding.x, header_text_size.y + MenuStyle::childhdr_line_padding.y + MenuStyle::childhdr_contents_padding_y));

			g_gui_widgets_i->add_child(
				m_header + "_inner", Vector2D(-1.0f, -1.0f),
				false, ImGuiWindowFlags_None,
				[this]()
				{
					contents();
				});
		}
	});

	g_gui_widgets_i->pop_color(); // ChildBg
}

//---------------------------------------------------------------------------------------------------

bool MenuTabSection::render_button(const Vector2D& button_size, const std::string& section_id, ContentsSelectionContext& contents_selection)
{
	// text color
	g_gui_widgets_i->push_color(ImGuiCol_Text, g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark));

	auto selected_button_clr = g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_FrameBgSelected);
	selected_button_clr.a *= 1.25f;
	g_gui_thememgr_i->push_color(GUICLR_FrameBgSelected, selected_button_clr);

	bool ret = false;

	bool selected = (contents_selection.section == section_id);
	if (g_gui_widgets_i->add_toggle_button_custom(section_id, button_size, selected, false, BUTTONFLAG_CenterLabel))
	{
		contents_selection.section = section_id; // make current one selected
		ret = true;
	}

	g_gui_thememgr_i->pop_color();

	if (g_gui_widgets_i->is_last_widget_hovered())
	{
		CMenuWidgetDescription::the().set_desc(m_desc.c_str());
	}

	g_gui_widgets_i->pop_color(1);

	return ret;
}

//---------------------------------------------------------------------------------------------------

bool MenuTab::render(Vector2D& offset, Vector2D& relative_offset, EMenuTabId id, ContentsSelectionContext& contents_selection)
{
//	g_gui_widgets_i->set_cursor_pos(relative_offset);

	auto button_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Bold);
	g_gui_widgets_i->push_font(button_font);

	// text color
	g_gui_widgets_i->push_color(ImGuiCol_Text, g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark));

	bool ret = false;

	bool selected = (contents_selection.id == id);
	if (g_gui_widgets_i->add_toggle_button(m_label.c_str(), { MenuStyle::tab_select_width - 15.0f, 0.0f }, selected))
	{
		contents_selection.id = id; // make current one selected
		ret = true;
	}

	if (g_gui_widgets_i->is_last_widget_hovered())
	{
		CMenuWidgetDescription::the().set_desc(m_desc.c_str());
	}

	g_gui_widgets_i->pop_color(1);

	g_gui_widgets_i->pop_font();

	relative_offset.y += button_font->FontSize + 4.0f;
	offset.y += button_font->FontSize + 4.0f;
	return ret;
}

//---------------------------------------------------------------------------------------------------

bool MenuTabGroup::render(Vector2D& offset, Vector2D& relative_offset, const Vector2D& child_size, ContentsSelectionContext& contents_selection)
{
	render_current_label(offset, relative_offset, child_size);

	bool did_change_tab = false;
	for (auto& [id, tab] : m_tabs)
	{
		if (tab.render(offset, relative_offset, id, contents_selection))
		{
			// it is enough when we've registered change only once.
			did_change_tab = true;
		}
	}

	return did_change_tab;
}

void MenuTabGroup::render_current_label(Vector2D& offset, Vector2D& relative_offset, const Vector2D& child_size)
{
	auto cursor_pos = g_gui_widgets_i->get_cursor_pos();

	g_gui_widgets_i->add_separated_heading(m_label);

	relative_offset.y += g_gui_widgets_i->get_cursor_pos().y - cursor_pos.y;
	offset.y += g_gui_widgets_i->get_cursor_pos().y - cursor_pos.y;
}

//----------------------------------------------------------------------------------------------------------------------

void CMenuContentsGrid::precompute(const Vector2D& contents_size, const MenuTabSection& section, bool force_compute)
{
	static Vector2D last_screen_size = {};
	if (!force_compute && !m_scheduled_precomputation)
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

	if (section.m_children.empty())
	{
		return;
	}

	const Vector2D& padding = MenuStyle::child_contents_padding;

	//
	// borderless children
	//
	//	There can be children that have "infinite size" (basically -1.0f notation in imgui).
	//	If there is such children, it must contain only ONE children inside the section.
	//
	{
		auto first_child = section.m_children[0];

		if (first_child->get_child_size().y == -1.0f)
		{
			// borderless children, we don't need to do any reorganization of children.
			return;
		}
	}

	int num_big_childern = 0;

	//
	// big children
	//
	{
		int current_column = 0, current_row = 0;
		Vector2D cursor_position = {};
		
		for (auto child : section.m_children)
		{
			// apply the search filter
			if (g_search_filter_context.apply_filter(child))
			{
				continue;
			}

			const Vector2D& child_size = child->get_collapsible_child_size();
			
			bool bigchild = child_size.x > MenuStyle::child_width;
			if (!bigchild)
			{
				continue;
			}
			
			child->m_computed_position = cursor_position;

			cursor_position.y += child_size.y + padding.y;

			num_big_childern++;
		}
	}

	//
	// regular children
	//
	{
		float x_offset = (num_big_childern != 0) ? ((MenuStyle::child_width + padding.x) * 2.0f) : 0.0f;

		int num_regular_children = std::max((int)section.m_children.size() - num_big_childern, 0);

		int current_column = 0, current_row = 0;
		Vector2D cursor_position = { x_offset, 0.0f };
		int max_children_in_one_row = std::max((int)((contents_size.x - x_offset) / (MenuStyle::child_width + padding.x)), 1);
		int n_children_per_column = (int)std::floor((float)num_regular_children / max_children_in_one_row);
		int n_children_per_column_remainder = num_regular_children % max_children_in_one_row;

		if (n_children_per_column == 0)
		{
			n_children_per_column = 1;
			n_children_per_column_remainder = 0;
		}
		for (auto child : section.m_children)
		{
			// apply the search filter
			if (g_search_filter_context.apply_filter(child))
			{
				continue;
			}

			const Vector2D& child_size = child->get_collapsible_child_size();
			
			bool bigchild = child_size.x > MenuStyle::child_width;
			if (bigchild)
			{
				continue;
			}

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

	// reset data
	last_compute_time = GetTickCount();
	last_screen_size = contents_size;
	m_scheduled_precomputation = false;
}

//---------------------------------------------------------------------------------------------------

void CUIMenu::handle_menu_contents_rendering()
{
	auto window_pos = g_gui_widgets_i->get_current_window_pos();
	auto window_size = g_gui_widgets_i->get_current_window_size();

	// blank tab, nothing selected
	if (m_current_context_selection.id == MENU_TAB_Blank)
	{
		g_gui_widgets_i->add_window_centered_text_disabled("👈👈 Start by looking through the tabs on the left!");
		return;
	}

	// render section buttons and handle section selection
	render_menu_contents_section_buttons();

	auto selection = get_selected_section();
	if (!selection)
	{
		g_gui_widgets_i->add_window_centered_text_disabled("👆 Select a section.");
		return;
	}

	//
	// handle scroll
	//

	if (m_menu_contents_changed)
	{
		g_gui_widgets_i->set_scroll((*selection)->m_scroll);
	}
	else
	{
		auto window_scroll = g_gui_widgets_i->get_current_scroll();
		if (!window_scroll.IsZero())
		{
			(*selection)->m_scroll = window_scroll;
		}
	}

	CMenuContentsGrid::the().precompute(window_size, *(*selection), m_menu_contents_changed);

	//
	// render children
	//

	if (m_current_context_selection.section != "main")
	{
		// shift vertically
		g_gui_widgets_i->set_cursor_pos(
			{ 
				MenuStyle::menu_contents_padding,
				MenuStyle::tab_section_button_padding.y * 2.0f + MenuStyle::tab_section_button_size.y
			});
	}

	g_gui_widgets_i->add_child(
		"menu_contents_inner",
		{ -1.0f, -1.0f },
		false,
		ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_HorizontalScrollbar,
		[&]()
		{
			g_gui_widgets_i->set_next_window_rounding(MenuStyle::child_rounding, ImDrawFlags_RoundCornersAll);

			for (auto child : (*selection)->m_children)
			{
				// apply the search filter, if it is active
				if (g_search_filter_context.apply_filter(child))
				{
					continue;
				}

				child->render();
			}

			if (g_search_filter_context.filter_active() && !g_search_filter_context.have_at_least_one_result())
			{
				g_gui_widgets_i->add_window_centered_text_disabled("Oops! Didn't find any results. 🧐😧");
			}
		});
}

void CUIMenu::render_menu_contents_section_buttons()
{
	float offset = MenuStyle::tab_section_button_padding.x;

	for (auto& [group_id, group] : m_tab_groups)
	{
		for (auto& [tab_id, tab] : group.m_tabs)
		{
			if (m_current_context_selection.id != tab_id)
			{
				continue;
			}

			// if the tab have only on section called "main", we don't render buttons at all
			if (tab.m_sections.size() == 1)
			{
				if (tab.m_sections.find("main") != tab.m_sections.end())
				{
					m_current_context_selection.section = "main";
					return;
				}
			}

			// render section buttons
			for (auto& [section_id, section] : tab.m_sections)
			{
				g_gui_widgets_i->set_cursor_pos(Vector2D(offset, MenuStyle::tab_section_button_padding.y));

				auto button_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_18px, FDC_Regular);
				auto label_size = g_gui_fontmgr_i->calc_font_text_size(button_font, section_id.c_str());
				label_size += MenuStyle::tab_section_button_inner_padding * 2.0f;

				g_gui_widgets_i->push_font(button_font);

				bool did_change_section = section.render_button(label_size, section_id, m_current_context_selection);
				if (!m_menu_contents_changed && did_change_section)
				{
					m_menu_contents_changed = true;
				}

				g_gui_widgets_i->pop_font();

				offset += label_size.x + MenuStyle::tab_section_button_padding.x;
			}

			// we're done here.
			break;
		}
	}
}

void CUIMenu::render_menu_tabs()
{
	auto child_pos = g_gui_widgets_i->get_current_window_pos();
	auto child_size = g_gui_widgets_i->get_current_window_size();

	g_gui_widgets_i->set_next_window_pos(child_pos + Vector2D(MenuStyle::tab_select_pos.x, MenuStyle::tab_select_pos.y), ImGuiCond_Always);

	g_gui_widgets_i->add_child(
		"menu_left_tabs",
		Vector2D(MenuStyle::tab_select_width, child_size.y),
		false,
		ImGuiWindowFlags_NoBackground,
		[&]()
		{
			m_sectab_relative_active_offset = 0.0f;
			m_sectab_absolute_active_offset = child_pos + m_sectab_relative_active_offset;

			g_gui_widgets_i->push_stylevar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

			for (auto& [group_id, group] : m_tab_groups)
			{
				bool did_change_tab = group.render(m_sectab_absolute_active_offset, m_sectab_relative_active_offset,
												   child_size, m_current_context_selection);

				g_gui_widgets_i->add_padding({ 0.0f, 4.0f });

				if (!m_menu_contents_changed && did_change_tab)
				{
					m_menu_contents_changed = true;
				}
			}

			g_gui_widgets_i->pop_stylevar();
		});
}

void CUIMenu::render_github_repo_link_decor()
{
	auto screen_size = g_imgui_platform_layer_i->get_screen_size();

	auto link_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular);

	const char* lnk_label = "www.github.com/oxiKKK/oxware";
	auto label_size = g_gui_fontmgr_i->calc_font_text_size(link_font, lnk_label);

	g_gui_widgets_i->set_next_window_pos({ screen_size.x - label_size.x - 6.0f, screen_size.y - label_size.y }, ImGuiCond_Always);
	g_gui_widgets_i->set_next_window_size(label_size + Vector2D(2.0f, 0.0f), ImGuiCond_Always);

	g_gui_widgets_i->push_font(link_font);

	static constexpr auto window_flags1 =
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoMove;
	g_gui_widgets_i->create_new_window(
		"github_link", window_flags1,
		[&]()
		{
			if (g_gui_widgets_i->add_url_text(lnk_label))
			{
				CGenericUtil::the().open_link_inside_browser(lnk_label);
			}
		});

	g_gui_widgets_i->pop_font();
}

void CUIMenu::render_menu_decoration(const Vector2D& window_pos, const Vector2D& window_size)
{
	// menu and tab select separator
	g_gui_window_rendering_i->render_line(g_gui_window_rendering_i->get_current_drawlist(),
										  { window_pos.x + MenuStyle::tab_select_width, window_pos.y - 0.5f },
										  { window_pos.x + MenuStyle::tab_select_width, window_pos.y + window_size.y },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_Separator));


	// top side menu separator
	g_gui_window_rendering_i->render_line(g_gui_window_rendering_i->get_current_drawlist(),
										  { window_pos.x + MenuStyle::tab_select_width, window_pos.y + MenuStyle::top_region_size_h },
										  { window_pos.x + window_size.x, window_pos.y + MenuStyle::top_region_size_h },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_Separator));

	//
	// render topside contents
	//
	auto topside_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_14px, FDC_Regular);
	auto topside_color = g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextBlack);

	auto version_label = std::format("v{} " OX_BUILD " build", OXVER_STRING);
	g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(),
										  topside_font,
										  { window_pos.x + MenuStyle::tab_select_width + 5.0f, window_pos.y + 0.0f + 3.0f },
										  topside_color,
										  version_label);

	auto compiletm_label = std::format("{}", OX_COMPILE_TIMESTAMP);
	g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(),
										  topside_font,
										 { window_pos.x + MenuStyle::tab_select_width + 5.0f, window_pos.y + 15.0f + 3.0f },
										  topside_color,
										  compiletm_label);

	// this gets filled automatically by a python script.
	// DO NOT EDIT FOLLOWING LINE OR THE SCRIPT WILL BREAK.
	auto lines_label = std::format(/*LINES&FILESMARKER*/"{} lines in {} files", "72,896", "363");
	g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(),
										  topside_font,
										 { window_pos.x + MenuStyle::tab_select_width + 5.0f, window_pos.y + 30.0f + 3.0f },
										  topside_color,
										  lines_label);

	// bottom separator
	g_gui_window_rendering_i->render_line(g_gui_window_rendering_i->get_current_drawlist(),
										  { window_pos.x + MenuStyle::tab_select_width + 1.0f, window_pos.y + (window_size.y - MenuStyle::bottom_reserved_rect_h - 1.0f) },
										  { window_pos.x + window_size.x,  window_pos.y + (window_size.y - MenuStyle::bottom_reserved_rect_h - 1.0f) },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_Separator));

	// bottom background
	g_gui_window_rendering_i->render_box(g_gui_window_rendering_i->get_current_drawlist(),
										 { window_pos.x + MenuStyle::tab_select_width + 1.0f, window_pos.y + (window_size.y - MenuStyle::bottom_reserved_rect_h) },
										  { window_pos.x + window_size.x, window_pos.y + (window_size.y - 1.0f) },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_ChildBackground));

	auto segoeui_small = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_13px, FDC_Bold);

	// bottom right timestamp label
	auto timestamp_label = CStringTools::the().format_date(std::chrono::system_clock::now());
	auto timestamp_label_size = g_gui_fontmgr_i->calc_font_text_size(segoeui_small, timestamp_label.c_str());
	g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(),
										  segoeui_small,
										  {
											  window_pos.x + (window_size.x - MenuStyle::bottom_right_timestamp_rightside_padding - timestamp_label_size.x),
											  window_pos.y + (window_size.y - MenuStyle::bottom_right_timestamp_bottomside_padding - timestamp_label_size.y)
										  },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark),
										  timestamp_label);

	// bottom-left description
	auto desc_label = CMenuWidgetDescription::the().get_desc();
	auto desc_label_size = g_gui_fontmgr_i->calc_font_text_size(segoeui_small, desc_label);
	g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(),
										  segoeui_small,
										  {
											  window_pos.x + MenuStyle::tab_select_width + MenuStyle::bottom_left_desc_padding.x,
											  window_pos.y + (window_size.y - MenuStyle::bottom_left_desc_padding.y - desc_label_size.y)
										  },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark),
										 desc_label);

	// Unload button
	Vector2D unload_button_pos = Vector2D(window_size.x - MenuStyle::unload_button_size.x - MenuStyle::unload_button_padding.x, MenuStyle::unload_button_padding.y);
	g_gui_widgets_i->set_cursor_pos(unload_button_pos);

	if (g_gui_widgets_i->add_button("❌", MenuStyle::unload_button_size, false, BUTTONFLAG_CenterLabel))
	{
		// end next frame.
		COxWare::the().end_cheat_execution();
	}

	// About button
	Vector2D about_button_pos = unload_button_pos - Vector2D(MenuStyle::about_button_size.x + MenuStyle::about_button_spacing_x, 0.0f);
	g_gui_widgets_i->set_cursor_pos(about_button_pos);

	if (g_gui_widgets_i->add_button("❔", MenuStyle::about_button_size, false, BUTTONFLAG_CenterLabel))
	{
		CUIWindowPopups::the().create_welcome_popup();
	}

	//
	// input search
	//

	float searchbar_padding_from_right_x = MenuStyle::top_rightside_decoration_width + MenuStyle::search_dialog_padding_width;
	float searchbar_pos_x = window_size.x - searchbar_padding_from_right_x - MenuStyle::search_dialog_spacing_right;

	g_search_filter_context.render_search_box({ searchbar_pos_x, 15.0f });
}

std::optional<MenuTabSection*> CUIMenu::get_selected_section()
{
	for (auto& [group_id, group] : m_tab_groups)
	{
		for (auto& [tab_id, tab] : group.m_tabs)
		{
			if (m_current_context_selection.id != tab_id)
			{
				continue;
			}

			for (auto& [section_id, section] : tab.m_sections)
			{
				if (m_current_context_selection.section != section_id)
				{
					continue;
				}

				return std::make_optional(&section);
			}
		}
	}

	return std::nullopt;
}

//----------------------------------------------------------------------------------------------------------------------

int CMenuSearchFilterContext::search_input_callback(ImGuiInputTextCallbackData* data)
{
	switch (data->EventFlag)
	{
		case ImGuiInputTextFlags_CallbackEdit:
		{
			CMenuContentsGrid::the().force_precompute_next_frame();

			break;
		}
	}
	return 0;
}

void CMenuSearchFilterContext::update()
{
	m_at_least_one_result = false;
}

void CMenuSearchFilterContext::render_search_box(const Vector2D& position)
{
	auto window_pos = g_gui_widgets_i->get_current_window_pos();

	g_gui_widgets_i->set_cursor_pos(position);

	auto search_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular);
	g_gui_window_rendering_i->render_text(g_gui_window_rendering_i->get_current_drawlist(),
										  search_font, // use this font, too
										  window_pos + Vector2D(position.x - 20.0f, 15.0f),
										  CColor(0, 0, 0, 255),
										  "🔎");

	g_gui_widgets_i->push_font(search_font);

	static const ImGuiInputTextFlags input_text_flags =
		ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll |
		ImGuiInputTextFlags_CallbackEdit;
	if (g_gui_widgets_i->add_text_input_ex(
		"menu_search", m_menu_search_input, sizeof(m_menu_search_input), Vector2D(MenuStyle::search_dialog_padding_width, 20.0f),
		input_text_flags, search_input_callback_stub, (void*)this, search_font))
	{
		// perform the search
	}

	g_gui_widgets_i->pop_font();
}

bool CMenuSearchFilterContext::apply_filter(IMenuChild* child)
{
	if (!filter_active())
	{
		return false;
	}

	// don't apply search filters on this child.
	if (child->get_flags() & MCHILDF_DontApplyFilter)
	{
		return true;
	}

	if (!CStringTools::the().case_insensitive_string_search(child->get_header(), m_menu_search_input))
	{
		return true;
	}
	else
	{
		m_at_least_one_result = true;
		return false;
	}
}

