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

UIStatusWidget CUIThemeEditor::m_status_widget = UIStatusWidget(2500);

static float s_status_bar_footer_height = 33.0f;

void CUIThemeEditor::render_ui()
{
	search_for_configs();

	if (m_theme_cfgs.empty() && !m_selected_cfg.empty())
	{
		m_selected_cfg.clear();
	}

	if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
	{
		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text(std::format("Theme files ({} 📂)", m_theme_cfgs.size()));

		g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, Vector2D(5, 5));

		g_gui_widgets_i->add_child(
			"theme_filelist", { -1.0f, -1.0f - s_status_bar_footer_height }, true, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[this]()
			{
				render_file_list();
			});

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text("Color palette");

		render_color_palette();

		g_gui_widgets_i->pop_stylevar(); // window padding

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
}

void CUIThemeEditor::render_file_list()
{
	if (m_theme_cfgs.empty())
	{
		g_gui_widgets_i->add_window_centered_text_disabled("No theme configuration files available 😥");
		return;
	}

	for (const auto& rel_cfg_path : m_theme_cfgs)
	{
		bool is_selected = (m_selected_cfg == rel_cfg_path);
		if (g_gui_widgets_i->add_toggle_button(rel_cfg_path.string(), { -1.0f, 0.0f }, is_selected, false))
		{
			if (is_selected)
			{
				// clicking on currently selected entry, toggle it.
				m_selected_cfg.clear();
			}
			else
			{
				m_selected_cfg = rel_cfg_path;
			}
		}
	}
}

