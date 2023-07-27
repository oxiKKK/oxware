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

UIStatusWidget CUICheatSettings::m_status_widget = UIStatusWidget(k_StatusWidgetStandardLifeDur);

static float s_status_bar_footer_height = 33.0f;

void CUICheatSettings::render_ui()
{
	search_for_configs();

	if (m_configs.empty() && !m_selected_cfg.empty())
	{
		m_selected_cfg.clear();
	}

	if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
	{
		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text(std::format("Configuration files ({} 📂)", m_configs.size()));

		g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, Vector2D(5, 5));

		render_file_list();

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text("Options on selected cfg");

		render_options_on_selected();

		render_actions();

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

void CUICheatSettings::search_for_configs()
{
	static uint32_t last_searched = GetTickCount();

	// search each # amount of ms
	static constexpr uint32_t interval_ms = 250;
	if (GetTickCount() - last_searched > interval_ms)
	{
		m_configs.clear();

		g_config_mgr_i->for_each_cfg(
			[this](const std::filesystem::path& relative_cfg_path)
			{
				auto full_cfg_path = g_filesystem_i->get_relative_to_appdata_ex("config\\", relative_cfg_path);
				m_configs.push_back(full_cfg_path);
			});

		last_searched = GetTickCount();
	}
}

void CUICheatSettings::render_actions()
{
	g_gui_widgets_i->add_text("Actions");

	g_gui_widgets_i->add_child(
		"actions", { -1.0f, -1.0f - s_status_bar_footer_height }, true, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[&]()
		{
			if (g_gui_widgets_i->add_button("Export as", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
			{
				actions_export_as();
			}

			if (g_gui_widgets_i->add_button("Open config directory", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
			{
				actions_open_config_dir();
			}

			if (g_gui_widgets_i->add_button("Save current to saved.json", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
			{
				actions_save_current();
			}

			if (g_gui_widgets_i->add_button("Restore defaults", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
			{
				actions_restore_defaults();
			}

			g_gui_widgets_i->pop_stylevar(); // window padding

			autosave();
		});
}

void CUICheatSettings::render_options_on_selected()
{
	g_gui_widgets_i->add_child(
		"selection", { -1.0f, 60 }, true, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[&]()
		{
			if (!m_selected_cfg.empty())
			{
				if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
				{
					g_gui_widgets_i->goto_next_column();

					if (g_gui_widgets_i->add_button("Load", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
					{
						options_load();
					}

					g_gui_widgets_i->goto_next_column();

					if (g_gui_widgets_i->add_button("Rename", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
					{
						options_rename();
					}

					g_gui_widgets_i->end_columns();
				}

				if (g_gui_widgets_i->add_button("Delete file", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
				{
					options_delete();
				}
			}
			else
			{
				g_gui_widgets_i->add_window_centered_text_disabled("No cfg selected! 😶");
			}
		});
}

void CUICheatSettings::render_file_list()
{
	g_gui_widgets_i->add_child(
		"config_filelist", { -1.0f, -1.0f - s_status_bar_footer_height }, true, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[this]()
		{
			if (m_configs.empty())
			{
				g_gui_widgets_i->add_window_centered_text_disabled("No configuration files available 😥");
				return;
			}

			for (const auto& cfg_path : m_configs)
			{
				bool is_selected = (m_selected_cfg == cfg_path);
				if (g_gui_widgets_i->add_toggle_button(cfg_path.string(), { -1.0f, 0.0f }, is_selected, false))
				{
					if (is_selected)
					{
						// clicking on currently selected entry, toggle it.
						m_selected_cfg.clear();
					}
					else
					{
						m_selected_cfg = cfg_path;
					}
				}
			}
		});
}

void CUICheatSettings::options_load()
{
	if (g_config_mgr_i->load_configuration(CFG_CheatSettings, m_selected_cfg.string()))
	{
		m_status_widget.update_status(std::format("Loaded from {}.", m_selected_cfg.string()), UIStatusWidget::Success);
	}
	else
	{
		m_status_widget.update_status(std::format("Failed to load {}!", m_selected_cfg.string()), UIStatusWidget::Error);
	}
}

void CUICheatSettings::options_rename()
{
	static char rename_buffer[MAX_PATH];

	auto rename_dialog = CUIWindowPopups::the().create_popup_context<UIDecoratedPopup>("config_rename");

	rename_dialog->provide_window_size(Vector2D(220, 170));
	rename_dialog->provide_window_flags(ImGuiWindowFlags_NoResize);
	rename_dialog->provide_on_cancel_fn(
		[&]()
		{
			// don't do nothing at all, just close the popup. this is in order to make the "close button to render"
			strcpy(rename_buffer, "");
		});
	rename_dialog->provide_on_okay_fn(
		[&]()
		{
			if (!rename_buffer[0])
			{
				return;
			}

			auto path = g_config_mgr_i->get_config_directory(rename_buffer);
			g_config_mgr_i->fix_config_file_extension(path);

			auto from_full = g_appdata_mgr_i->get_known("config\\") / m_selected_cfg;
			auto to_full = g_appdata_mgr_i->get_known("config\\") / path.filename();

			if (g_filesystem_i->do_exist(to_full))
			{
				m_status_widget.update_status(std::format("'{}' cannot be renamed to '{}' because the file already exists.",
														  m_selected_cfg, path.filename().string()), UIStatusWidget::Error);
			}
			else
			{
				if (g_filesystem_i->rename(from_full, to_full))
				{
					m_status_widget.update_status(std::format("Renamed '{}' to '{}'.", m_selected_cfg, path.filename().string()), UIStatusWidget::Success);
				}
				else
				{
					m_status_widget.update_status(std::format("Couldn't rename '{}' to '{}'.", m_selected_cfg, path.filename().string()), UIStatusWidget::Error);
				}
			}

			strcpy(rename_buffer, "");
		});

	rename_dialog->provide_contents_fn(
		[]()
		{
			g_gui_widgets_i->add_text("Rename to");

			g_gui_widgets_i->add_text_input("rename_to_input", rename_buffer, sizeof(rename_buffer), ImGuiInputTextFlags_None, true);

			g_gui_widgets_i->add_text("Will be renamed to:");

			if (rename_buffer[0])
			{
				g_gui_widgets_i->add_text(std::format("'{}.json'", rename_buffer),
										  TEXTPROP_Wrapped, g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular));
			}

			return false;
		});

	CUIWindowPopups::the().schedule_popup(rename_dialog);
}

void CUICheatSettings::options_delete()
{
	if (g_filesystem_i->remove(g_appdata_mgr_i->get_known("config\\") / m_selected_cfg))
	{
		m_status_widget.update_status(std::format("Deleted {}.", m_selected_cfg), UIStatusWidget::Success);
	}
	else
	{
		m_status_widget.update_status(std::format("Failed to delete {}.", m_selected_cfg), UIStatusWidget::Error);
	}
}

void CUICheatSettings::actions_export_as()
{
	static char name_buffer[MAX_PATH];

	auto create_dialog = CUIWindowPopups::the().create_popup_context<UIDecoratedPopup>("config_export_as");

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

			if (g_config_mgr_i->write_configuration(CFG_CheatSettings, name_buffer))
			{
				m_status_widget.update_status(std::format("Exported as '{}'.", name_buffer), UIStatusWidget::Success);
			}
			else
			{
				m_status_widget.update_status(std::format("Couldn't export as '{}'.", name_buffer), UIStatusWidget::Error);
			}

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

void CUICheatSettings::actions_open_config_dir()
{
	auto dir = g_config_mgr_i->get_config_directory().string();
	CGenericUtil::the().open_folder_inside_explorer(dir);
	m_status_widget.update_status("Opened config directory.", UIStatusWidget::Success);
}

void CUICheatSettings::actions_save_current()
{
	if (g_config_mgr_i->write_configuration(CFG_CheatSettings, "saved.json"))
	{
		m_status_widget.update_status("Saved to saved.json.", UIStatusWidget::Success);
	}
	else
	{
		m_status_widget.update_status("Failed to save to saved.json!", UIStatusWidget::Error);
	}
}

void CUICheatSettings::actions_restore_defaults()
{
	// first load it
	if (g_config_mgr_i->load_configuration(CFG_CheatSettings, "default.json"))
	{
		m_status_widget.update_status("Restored default configuration.", UIStatusWidget::Success);
	}
	else
	{
		// if we fail, most likely the "default.json" file was deleted. created it back again and load that.
		g_config_mgr_i->write_configuration(CFG_CheatSettings, "default.json");

		if (!g_config_mgr_i->load_configuration(CFG_CheatSettings, "default.json"))
		{
			// idk... xd shouldn't happen
			m_status_widget.update_status("Failed to restore defaults!", UIStatusWidget::Error);
		}
	}
}

void CUICheatSettings::autosave()
{
	g_gui_widgets_i->add_spacing();
	auto var = g_config_mgr_i->get_save_cfg_interval_var();

	g_gui_widgets_i->add_text("⏳ Auto-save interval");
	g_gui_widgets_i->add_spacing();
	CUIMenuWidgets::the().add_slider_nolabel("autosave_interval", "%0.0f seconds", var, "off");

	if (var->get_value() != var->get_min())
	{
		g_gui_widgets_i->add_progress_bar("Test", { -1.0f, 0.0f }, 
										  g_config_mgr_i->get_duration_last_saved_sec(), 
										  (float)g_config_mgr_i->get_save_cfg_interval_var()->get_value());

		// HACKHACK
		if (g_config_mgr_i->get_duration_last_saved_sec() > (float)g_config_mgr_i->get_save_cfg_interval_var()->get_value() - 0.01f)
		{
			m_status_widget.update_status("Autosaved!", UIStatusWidget::Success);
		}
	}
}
