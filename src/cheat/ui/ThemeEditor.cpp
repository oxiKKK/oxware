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

UIStatusWidget CUIThemeEditor::m_status_widget = UIStatusWidget(k_StatusWidgetStandardLifeDur);

static float s_status_bar_footer_height = 33.0f;
static float s_current_theme_text_footer_height = 33.0f;

void CUIThemeEditor::render_ui()
{
	search_for_configs();

	if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
	{
		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text("Color palette");
		
		render_color_palette();

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text("Actions");

		render_actions();

		g_gui_widgets_i->begin_tab("themes", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll);

		g_gui_widgets_i->add_tab_item(
			std::format("📂 Files ({})", m_theme_cfgs.size()), true, { -1.0f, -1.0f - (s_status_bar_footer_height + s_current_theme_text_footer_height) },
			[this]()
			{
				render_file_list();
			});

		g_gui_widgets_i->add_tab_item(
			std::format("Built-in ({})", g_gui_thememgr_i->get_num_themes()), true, { -1.0f, -1.0f - (s_status_bar_footer_height + s_current_theme_text_footer_height) },
			[this]()
			{
				render_built_in_list();
			});

		g_gui_widgets_i->end_tab();

		g_gui_widgets_i->add_child(
			"current_theme", { -1.0f, -1.0f - s_status_bar_footer_height }, true, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[&]()
			{
				auto current_theme = g_gui_thememgr_i->get_current_theme_name();
				g_gui_widgets_i->add_text(std::format("Current theme: {}", current_theme));
			});

		g_gui_widgets_i->end_columns();
	}

	g_gui_widgets_i->add_child(
		"status", { -1.0f, -1.0f }, true, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[&]()
		{
			if (m_status_widget.is_alive())
			{
				g_gui_widgets_i->add_colored_text(m_status_widget.get_color(), m_status_widget.get_string());
			}
		});
}

void CUIThemeEditor::search_for_configs()
{
	static uint32_t last_searched = GetTickCount();

	// search each # amount of ms
	static constexpr uint32_t interval_ms = 250;
	if (GetTickCount() - last_searched > interval_ms)
	{
		m_theme_cfgs.clear();

		g_config_mgr_i->for_each_cfg(
			[this](const std::filesystem::path& relative_cfg_path)
			{
				auto full_cfg_path = g_filesystem_i->get_relative_to_appdata_ex("config\\theme\\", relative_cfg_path);
				m_theme_cfgs.push_back(full_cfg_path);
			}, "theme");

		last_searched = GetTickCount();
	}
}

void CUIThemeEditor::render_color_palette()
{
	g_gui_widgets_i->add_child(
		"color_palette", { -1.0f, -1.0f - s_status_bar_footer_height }, true, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[&]()
		{
			auto current_theme = g_gui_thememgr_i->get_current_theme();

			for (size_t i = GUICLR_TextLight; i < GUICLR_MAX; i++)
			{
				auto id = (EGUIColorId)i;
				auto& color = current_theme->immutable_color_list()[i];
				std::string color_name = g_gui_thememgr_i->color_id_to_string(id);

				if (g_gui_widgets_i->add_color_edit(
					color_name, color.get_base(), true,
					ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB))
				{
					g_gui_thememgr_i->sync_colors_with_imgui();
				}

				// re-set both of the colors
				current_theme->initialize_colors(id, color);
			}
		});
}

void CUIThemeEditor::render_file_list()
{
	if (m_theme_cfgs.empty())
	{
		g_gui_widgets_i->add_window_centered_text_disabled("No theme configuration files available 😥");
		return;
	}

	auto window_size = g_gui_widgets_i->get_current_window_size();

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_CellPadding, { 1.0f, 1.0f });

	if (g_gui_widgets_i->begin_columns("theme_filelist", 2))
	{
		g_gui_widgets_i->setup_column_fixed_width(window_size.x - 60.0f);

		for (const auto& rel_cfg_path : m_theme_cfgs)
		{
			g_gui_widgets_i->push_id(rel_cfg_path.string());

			g_gui_widgets_i->goto_next_column();

			g_gui_widgets_i->add_text(rel_cfg_path.string());

			g_gui_widgets_i->goto_next_column();

			if (g_gui_widgets_i->add_button("Load", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
			{
				auto theme_rel_path = "theme" / rel_cfg_path;

				if (g_config_mgr_i->load_configuration(CFG_CheatTheme, theme_rel_path.string()))
				{
					m_status_widget.update_status(std::format("Loaded from {}.", theme_rel_path.string()), UIStatusWidget::Success);
				}
				else
				{
					m_status_widget.update_status(std::format("Failed to load {}!", theme_rel_path.string()), UIStatusWidget::Error);
				}
			}

			g_gui_widgets_i->pop_id();
		}

		g_gui_widgets_i->end_columns();
	}

	g_gui_widgets_i->pop_stylevar(); // cell padding
}

void CUIThemeEditor::render_built_in_list()
{
	auto window_size = g_gui_widgets_i->get_current_window_size();

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_CellPadding, { 1.0f, 1.0f });

	if (g_gui_widgets_i->begin_columns("theme_builtinlist", 2))
	{
		g_gui_widgets_i->setup_column_fixed_width(window_size.x - 60.0f);

		g_gui_thememgr_i->for_each_built_in(
			[](const std::string& id, const CGUITheme& theme)
			{
				g_gui_widgets_i->push_id(id);

				g_gui_widgets_i->goto_next_column();

				g_gui_widgets_i->add_text(id);

				g_gui_widgets_i->goto_next_column();

				if (g_gui_widgets_i->add_button("Use", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
				{
					if (g_gui_thememgr_i->set_new_theme(id))
					{
						m_status_widget.update_status(std::format("Now using '{}' theme.", id), UIStatusWidget::Success);
					}
					else
					{
						m_status_widget.update_status(std::format("Couldn't use theme '{}'!", id), UIStatusWidget::Error);
					}
				}

				g_gui_widgets_i->pop_id();
			});

		g_gui_widgets_i->end_columns();
	}

	g_gui_widgets_i->pop_stylevar(); // cell padding
}

void CUIThemeEditor::render_actions()
{
	g_gui_widgets_i->add_child(
		"actions", { -1.0f, 53.0f }, true, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[&]()
		{
			if (g_gui_widgets_i->add_button("Export as", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
			{
				actions_export_current_theme_as();
			}

			if (g_gui_widgets_i->add_button("Open theme directory", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
			{
				actions_open_config_dir();
			}
		});
}

void CUIThemeEditor::actions_open_config_dir()
{
	auto dir = g_config_mgr_i->get_config_directory().string();
	CGenericUtil::the().open_folder_inside_explorer(dir);
	m_status_widget.update_status("Opened config directory.", UIStatusWidget::Success);
}

void CUIThemeEditor::actions_export_current_theme_as()
{
	static char name_buffer[MAX_PATH];

	auto create_dialog = CUIWindowPopups::the().create_popup_context<UIDecoratedPopup>("export_theme_as");

	create_dialog->provide_window_size(Vector2D(220, 170));
	create_dialog->provide_window_flags(ImGuiWindowFlags_NoResize);

	create_dialog->provide_on_cancel_fn(
		[&]()
		{
			// don't do nothing at all, just close the popup. this is in order to make the "close button to render"
			strcpy(name_buffer, "");
		});

	create_dialog->provide_on_okay_fn(
		[&]()
		{
			if (!name_buffer[0])
			{
				return;
			}

			std::filesystem::path path = "theme\\" + std::string(name_buffer);
			g_config_mgr_i->fix_config_file_extension(path);

			g_gui_thememgr_i->set_current_theme_export_name(name_buffer);

			if (g_config_mgr_i->write_configuration(CFG_CheatTheme, path.string()))
			{
				m_status_widget.update_status(std::format("Exported as '{}'.", path.string()), UIStatusWidget::Success);
			}
			else
			{
				m_status_widget.update_status(std::format("Couldn't export '{}'.", path.string()), UIStatusWidget::Error);
			}

			g_gui_thememgr_i->reset_current_theme_export_name();

			strcpy(name_buffer, "");
		});

	create_dialog->provide_contents_fn(
		[]()
		{
			g_gui_widgets_i->add_text("Export as");

			g_gui_widgets_i->add_text_input("export_as_input", name_buffer, sizeof(name_buffer), ImGuiInputTextFlags_None, true);

			g_gui_widgets_i->add_text("Will be exported as:");

			if (name_buffer[0])
			{
				g_gui_widgets_i->add_text(std::format("'{}.json'", name_buffer),
										  TEXTPROP_Wrapped, g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular));
			}

			return false;
		});

	CUIWindowPopups::the().schedule_popup(create_dialog);
}
