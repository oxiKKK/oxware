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

VarBoolean ui_menu_resizable("ui_menu_resizable", "This is a beta feature!", false);

// Note: moved here from the header file for faster compile times
const float CMenuStyle::k_rounding_factor = 20.0f;
const Vector2D CMenuStyle::k_menu_rect_size = { 620, 420 };
const Vector2D CMenuStyle::k_menu_rect_size_max = k_menu_rect_size * Vector2D(1.73f, 1.75f); // sized so that four child columns fit inside contents
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
const float CMenuStyle::k_child_width = 225.0f;
const Vector2D CMenuStyle::k_unload_button_padding = { 15.0f, 15.0f };
const Vector2D CMenuStyle::k_unload_button_size = { 105.0f, 20.0f };
const Vector2D CMenuStyle::k_about_button_size = { 50.0f, 20.0f };
const Vector2D CMenuStyle::k_tab_section_button_size = { 130.0f, 22.0f } ;
const Vector2D CMenuStyle::k_tab_section_button_padding = { 5.0f, 3.0f };
const float CMenuStyle::k_childhdr_text_padding_x = 5.0f;
const float CMenuStyle::k_childhdr_text_padding_y = 1.0f;
const Vector2D CMenuStyle::k_childhdr_line_padding = { 5.0f, 2.0f };
const float CMenuStyle::k_childhdr_contents_padding_y = 5.0f;

Vector2D CMenuStyle::calc_child_size(float height) { return { k_child_width, height }; }
Vector2D CMenuStyle::calc_child_size_2x(float height) { return { k_child_width * 2.0f + k_child_contents_padding.x, height }; }

float CMenuStyle::get_child_width_w_padding() { return CMenuStyle::k_child_width - CMenuStyle::k_child_contents_padding.x * 2; }

// use this instead of width begin -1.0f
Vector2D CMenuStyle::child_full_width(float height) { return { -1.0f - k_menu_contents_padding, height }; }

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

		Vector2D header_text_pos = { child_pos.x + CMenuStyle::k_childhdr_text_padding_x, child_pos.y + CMenuStyle::k_childhdr_text_padding_y };

		auto header_color = (m_supports_collapse && m_is_hovered) ? 
			g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_HyperTextLinkHovered>() :
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
			{ child_pos.x + CMenuStyle::k_childhdr_line_padding.x, child_pos.y + header_text_size.y + CMenuStyle::k_childhdr_line_padding.y },
			{ child_pos.x + child_size.x - CMenuStyle::k_childhdr_line_padding.x, child_pos.y + header_text_size.y + CMenuStyle::k_childhdr_line_padding.y },
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
			g_gui_widgets_i->set_cursor_pos(Vector2D(CMenuStyle::k_child_contents_padding.x, header_text_size.y + CMenuStyle::k_childhdr_line_padding.y + CMenuStyle::k_childhdr_contents_padding_y));

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

void MenuTabSection::render_button(const Vector2D& button_size, const std::string& id, TabSectionId& current_tab_section_id)
{
	auto button_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_18px, FDC_Regular);
	g_gui_widgets_i->push_font(button_font);

	// text color
	g_gui_widgets_i->push_color(ImGuiCol_Text, g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark));

	auto unselected_color = g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_Button);
	unselected_color.a *= 0.35f;

	bool selected = (current_tab_section_id.second == id);
	if (g_gui_widgets_i->add_toggle_button(id, button_size, selected, false, BUTTONFLAG_CenterLabel, unselected_color))
	{
		current_tab_section_id.second = id; // make current one selected
	}

	if (g_gui_widgets_i->is_last_widget_hovered())
	{
		CMenuWidgetDescription::the().set_desc(m_desc.c_str());
	}

	g_gui_widgets_i->pop_color(1);

	g_gui_widgets_i->pop_font();
}

//---------------------------------------------------------------------------------------------------

void MenuTab::render(Vector2D& offset, Vector2D& relative_offset, EMenuTabId id, TabSectionId& current_tab_section_id)
{
	g_gui_widgets_i->set_cursor_pos(relative_offset);

	auto button_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Bold);
	g_gui_widgets_i->push_font(button_font);

	// text color
	g_gui_widgets_i->push_color(ImGuiCol_Text, g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_TextDark));

	bool selected = (current_tab_section_id.first == id);
	if (g_gui_widgets_i->add_toggle_button(m_label.c_str(), { CMenuStyle::k_tab_select_width - 15.0f, 0.0f }, selected))
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
										  { offset.x + child_size.x - 15.0f, offset.y + 5.f },
										  g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_Separator));

	relative_offset.y += 8.f;
	offset.y += 8.f;
}

//----------------------------------------------------------------------------------------------------------------------

void CMenuContentsGrid::precompute(const Vector2D& contents_size, const MenuTabSection& tab_section, bool force_compute)
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

	if (tab_section.m_children.empty())
	{
		return;
	}

	float contents_width = contents_size.x;

	const Vector2D& padding = CMenuStyle::k_child_contents_padding;

	//
	// borderless children
	//
	//	There can be children that have "infinite size" (basically -1.0f notation in imgui).
	//	If there is such children, it must contain only ONE children inside the tab.
	//
	{
		auto first_child = tab_section.m_children[0];

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
		
		for (auto child : tab_section.m_children)
		{
			const Vector2D& child_size = child->get_collapsible_child_size();
			
			bool bigchild = child_size.x > CMenuStyle::k_child_width;
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
		float x_offset = (num_big_childern != 0) ? ((CMenuStyle::k_child_width + padding.x) * 2.0f) : 0.0f;

		int num_regular_children = std::max((int)tab_section.m_children.size() - num_big_childern, 0);

		int current_column = 0, current_row = 0;
		Vector2D cursor_position = { x_offset, 0.0f };
		int max_children_in_one_row = std::max((int)((contents_width - x_offset) / (CMenuStyle::k_child_width + padding.x)), 1);
		int n_children_per_column = (int)std::floor((float)num_regular_children / max_children_in_one_row);
		int n_children_per_column_remainder = num_regular_children % max_children_in_one_row;
		for (auto child : tab_section.m_children)
		{
			const Vector2D& child_size = child->get_collapsible_child_size();
			
			bool bigchild = child_size.x > CMenuStyle::k_child_width;
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

void CUIMenu::on_initialize()
{
	//
	// Visuals
	//
	m_tabgroup_Visuals.set_label("Visuals");
	{
		m_tabgroup_Visuals.add_tab(
			UIMENU_World,
			{ "World", "World-related cheats",
				{
					{
						"none",
						{
							"Description",
							{
								new MenuChilden::World::Removals({ "Removals", 250, true }),
								new MenuChilden::World::Thirdperson({ "Thirdperson", 100, true }),
								new MenuChilden::World::AntiDemoRecorder({ "Anti demo recorder", 125, true }),
								new MenuChilden::World::AntiScreen({ "Anti screen", 90, true }),
								new MenuChilden::World::Automation({ "Automation", 120, true }),
								new MenuChilden::World::SpeedControl({ "Speed control", 215, true }),
								new MenuChilden::World::ViewmodelOffset({ "Viewmodel offset", 90, true }),
								new MenuChilden::World::Backtrack({ "Backtrack", 105, true }),
								new MenuChilden::World::SmokeVisuals({ "Smoke visuals", 180, true }),
								new MenuChilden::World::NonSteamFpsFix({ "Non-steam fps fix", 150, true }),
								new MenuChilden::World::WorldVisuals({ "World visuals", 225, true }),
								new MenuChilden::World::EnvironmentalEffects({ "Environmental effects", 300, true }),
							}
						}
					}
				}
			});

		m_tabgroup_Visuals.add_tab(
		UIMENU_Rendering,
			{ "Rendering", "Rendering-related cheats",
				{
					{
						"none",
						{
							"Description",
							{
								new MenuChilden::Rendering::FieldOfView({ "Field of view", 85, true }),
								new MenuChilden::Rendering::AspectRatio({ "Aspect ratio", 85, true }),
								new MenuChilden::Rendering::StudioRenderer({ "Studio renderer", 235, true }),
								new MenuChilden::Rendering::ModelChams({ "Model chams", 250, true }),
								new MenuChilden::Rendering::HUDRendering({ "HUD rendering", 210, true }),
							}
						}
					}
				}
			});

		m_tabgroup_Visuals.add_tab(
		UIMENU_Screen,
			{ "Screen", "Screen-related cheats",
				{
					{
						"none",
						{
							"Description",
							{
								new MenuChilden::Screen::FlashbangFade({ "Flashbang fade", 100, true }),
								new MenuChilden::Screen::ESP({ "ESP", 300, true }),
								new MenuChilden::Screen::CustomCrosshair({ "Custom crosshair", 270, true }),
								new MenuChilden::Screen::ClShowFPSEnhancement({ "cl_showfps enhancement", 140, true }),
							}
						}
					}
				}
			});
	}
	m_tab_groups.push_back(&m_tabgroup_Visuals);
	
	//
	// Miscellanous
	//
	m_tabgroup_Miscellaneous.set_label("Miscellaneous");
	{
		m_tabgroup_Miscellaneous.add_tab(
			UIMENU_Exploits,
			{ "Exploits", "Various game exploitations",
				{
					{
						"none",
						{
							"Description",
							{
								new MenuChilden::Exploits::ServerCommandFilter({ "Server command filter", 210, true, MENUCHILD_2x }),
								new MenuChilden::Exploits::CvarSandbox({ "Cvar sandbox", 400, true, MENUCHILD_2x }),
								new MenuChilden::Exploits::BypassGameConstrains({ "Bypass game constrains", 290, true }),
								new MenuChilden::Exploits::FramerateSimulation({ "Framerate simulation", 155, true }),
								new MenuChilden::Exploits::ConsistencyBypass({ "Consistency bypass", 135, true }),
								new MenuChilden::Exploits::FakeLatency({ "Fake latency", 85, true }),
							}
						}
					}
				}
			});

		m_tabgroup_Miscellaneous.add_tab(
			UIMENU_Movement,
			{ "Movement", "Kreedz/Movement cheats",
				{
					{
						"none",
						{
							"Description",
							{
								new MenuChilden::Movement::AirStuck({ "Air stuck", 150, true }),
								new MenuChilden::Movement::Visualization({ "Visualization", 170, true }),
								new MenuChilden::Movement::GroundStrafe({ "Ground strafe", 300, true }),
								new MenuChilden::Movement::Strafehack({ "Strafe hack", 225, true }),
								new MenuChilden::Movement::StrafeHelper({ "Strafe helper", 240, true }),
								new MenuChilden::Movement::Bunnyhop({ "Bunnyhop", 300, true }),
								new MenuChilden::Movement::Edgebug({ "Edge bug", 290, true }),
								new MenuChilden::Movement::Fastrun({ "Fast run", 200, true }),
								new MenuChilden::Movement::AutoJOF({ "Auto JOF", 200, true }),
							}
						}
					}
				}
			});
	}
	m_tab_groups.push_back(&m_tabgroup_Miscellaneous);
	
	//
	// Configuration
	//
	m_tabgroup_Configuration.set_label("Configuration");
	{
		m_tabgroup_Configuration.add_tab(
			UIMENU_Config,
			{ "Config", "Config list/manager",
				{
					{
						"none",
						{
							"Description",
							{
								new MenuChilden::Configuration::Config({ "Configuration", -1, false, MENUCHILD_2x }),
							}
						}
					}
				}
			});

		m_tabgroup_Configuration.add_tab(
			UIMENU_Theme,
			{ "Theme", "Theme editor",
				{
					{
						"none",
						{
							"Allows you to change UI theme",
							{
								new MenuChilden::Configuration::Theme({ "Theme", -1, false, MENUCHILD_2x }),
							}
						}
					}
				}
			});

		m_tabgroup_Configuration.add_tab(
			UIMENU_Binds,
			{ "Bind manager", "Cheat key-bind systems",
				{
					{ 
						"Bind list",
						{
							"Interactive UI for key binding",
							{
								new MenuChilden::Configuration::KeyBinding({ "Key binding", -1, false, MENUCHILD_2x }),
							}
						}
					},
					{
						"In-Commands",
						{
							"In-Command manager",
							{
								new MenuChilden::Configuration::InCommands({ "In-Commands", -1, false, MENUCHILD_2x }),
							}
						}
					}
				}
			});
	}
	m_tab_groups.push_back(&m_tabgroup_Configuration);
	
	m_tabgroup_Other.set_label("Other");
	{
		m_tabgroup_Other.add_tab(
			UIMENU_Console,
			{ "Console", "Console-related systems",
				{
					{ 
						"Variable list",
						{
							"Cheat variable list",
							{
								new MenuChilden::Other::VariableList({ "Variable list", -1, false, MENUCHILD_2x }),
							}
						}
					},
					{
						"Command list",
						{
							"Cheat command list",
							{
								new MenuChilden::Other::CommandList({ "Command list", -1, false, MENUCHILD_2x }),
							}
						}
					}
				}
			});
	}
	m_tab_groups.push_back(&m_tabgroup_Other);
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

	auto segoeui_extra = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_33px, FDC_Bold);

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
														  { child_pos.x + (child_size.x - label_size.x) / 2.0f, window_pos.y + (CMenuStyle::k_top_region_size_h - label_size.y) / 2.0f },
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
	auto window_pos = g_gui_widgets_i->get_current_window_pos();
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

	// true if there's <= 1 button for this tab
	bool buttons_disabled = false;

	// find the current menu tab
	MenuTab* current_menu_tab = nullptr;
	for (auto i = m_tab_groups.begin(); i != m_tab_groups.end() && !current_menu_tab; i++)
	{
		auto group = *i;

		for (auto k = group->m_menu_tabs.begin(); k != group->m_menu_tabs.end() && !current_menu_tab; k++)
		{
			auto& [group_id, tab] = *k;

			if (group_id != m_active_tab_section.first)
			{
				continue;
			}

			// if there's only one tab or none, don't process the rendering of buttons at all
			if (tab.m_sections.size() <= 1)
			{
				current_menu_tab = &tab;
				buttons_disabled = true;
				break;
			}

#if 0
			//
			// render background
			//

			g_gui_window_rendering_i->render_box(
				g_gui_window_rendering_i->get_current_drawlist(),
				window_pos,
				window_pos + Vector2D(window_size.x, CMenuStyle::k_tab_section_button_size.y + CMenuStyle::k_tab_section_button_padding.y * 2.0f), 
				g_gui_thememgr_i->get_current_theme()->get_color(GUICLR_ChildBackground), 
				5.0f);
#endif
			
			Vector2D button_size = CMenuStyle::k_tab_section_button_size;
			float button_width_w_padding = button_size.x + CMenuStyle::k_tab_section_button_padding.x;
			float button_total_width = tab.m_sections.size() * button_width_w_padding;
			float offset = /*(window_size.x - button_total_width) / 2.0f*/0.0f;

			for (auto j = tab.m_sections.begin(); j != tab.m_sections.end(); j++)
			{
				auto& [section_id, section] = *j;

				// always have at least one tab selected.
				if (m_active_tab_section.second.empty() && j == tab.m_sections.begin())
				{
					m_active_tab_section.second = section_id;
				}

				g_gui_widgets_i->set_cursor_pos(Vector2D(offset, 0.0f));

				// it's more convenient to render the tab section buttons here.
				section.render_button(button_size, section_id, m_active_tab_section);

				offset += button_width_w_padding;

				if (section_id == m_active_tab_section.second)
				{
					current_menu_tab = &tab;

					// don't leave yet, since we're rendering buttons.
				}
			}
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

	static std::string last_section_id;
	if (last_section_id != m_active_tab_section.second)
	{
		contents_changed = true;
		last_section_id = m_active_tab_section.second;
	}

	g_gui_widgets_i->add_child(
		"menu_contents_inner",
		{ -1.0f, -1.0f },
		false,
		ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_HorizontalScrollbar,
		[&]()
		{
			for (auto i = current_menu_tab->m_sections.begin(); i != current_menu_tab->m_sections.end(); i++)
			{
				auto& [id, section] = *i;

				// only perform the filter when buttons aren't disabled.
				if (!buttons_disabled)
				{
					if (id != m_active_tab_section.second)
					{
						continue;
					}
				}

				CMenuContentsGrid::the().precompute(window_size, section, contents_changed);

				g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { CMenuStyle::k_child_contents_padding.x, CMenuStyle::k_child_contents_padding.y });
				
				for (auto child : section.m_children)
				{
					child->render();
				}

				g_gui_widgets_i->pop_stylevar();

			}
		});
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
