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

void MenuChilden::Configuration::Config::contents()
{
	static UIStatusWidget status_widget = UIStatusWidget(2500);

	static std::vector<FilePath_t> configs;
	static int num_configs = 0;
	auto search_for_configs = []()
	{
		configs.clear();

		g_config_mgr_i->for_each_cfg(
			[](const FilePath_t& cfg_path)
			{
				num_configs++;

				auto relative_cfg_path = g_filesystem_i->get_relative_to_appdata_ex("config\\", cfg_path);

				configs.push_back(relative_cfg_path);
			});
	};

	static uint32_t last_searched = GetTickCount();

	// search each # amount of ms
	static constexpr uint32_t interval_ms = 250;
	if (GetTickCount() - last_searched > interval_ms)
	{
		search_for_configs();
		last_searched = GetTickCount();
	}

	static FilePath_t selected_cfg = {};
	if (configs.empty() && !selected_cfg.empty())
	{
		selected_cfg.clear();
	}

	float status_bar_footer_height = 33.0f;

	if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
	{
		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text("Configuration files");

		g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, Vector2D(5, 5));

		g_gui_widgets_i->add_child(
			"configs", { -1.0f, -1.0f - status_bar_footer_height }, true, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[]()
			{
				if (configs.empty())
				{
					g_gui_widgets_i->add_window_centered_text_disabled("No configuration files available");
				}
				else
				{
					for (const auto& cfg_path : configs)
					{
						bool is_selected = selected_cfg == cfg_path;
						if (g_gui_widgets_i->add_toggle_button(cfg_path.string(), { -1.0f, 0.0f },
															   is_selected, false))
						{
							if (is_selected)
							{
								// clicking on currently selected entry, toggle it.
								selected_cfg.clear();
							}
							else
							{
								selected_cfg = cfg_path;
							}
						}
					}
				}
			});

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_text("Options on selected cfg");

		g_gui_widgets_i->add_child(
			"selection", { -1.0f, 55 }, true, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[&]()
			{
				if (!selected_cfg.empty())
				{
					if (g_gui_widgets_i->add_button("load", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
					{
						if (g_config_mgr_i->load_configuration(CFG_CheatSettings, selected_cfg.string()))
						{
							status_widget.update_status(std::format("Loaded from {}.", selected_cfg.string()), UIStatusWidget::Success);
						}
						else
						{
							status_widget.update_status(std::format("Failed to load {}!", selected_cfg.string()), UIStatusWidget::Error);
						}
					}

					if (g_gui_widgets_i->add_button("delete", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
					{
						if (g_filesystem_i->remove(g_appdata_mgr_i->get_known("config\\") / selected_cfg))
						{
							status_widget.update_status(std::format("Deleted {}.", selected_cfg), UIStatusWidget::Success);
						}
						else
						{
							status_widget.update_status(std::format("Failed to delete {}.", selected_cfg), UIStatusWidget::Error);
						}
					}
				}
				else
				{
					g_gui_widgets_i->add_window_centered_text_disabled("No cfg selected");
				}
			});

		g_gui_widgets_i->add_text("Other");

		g_gui_widgets_i->add_child(
			"information", { -1.0f, -1.0f - status_bar_footer_height }, true, ImGuiWindowFlags_AlwaysUseWindowPadding,
			[&]()
			{
				if (g_gui_widgets_i->add_button("create new", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
				{
					static char name_buffer[64];
					COxWareUI::the().schedule_popup(
						"", Vector2D(210, 170),
						[]()
						{
							if (g_gui_widgets_i->add_text_input("Config name", name_buffer, sizeof(name_buffer), ImGuiInputTextFlags_EnterReturnsTrue, true))
							{
								// enter returns true:
								COxWareUI::the().close_current_popup();
							}

							g_gui_widgets_i->add_text("Enter name of the config");

							g_gui_widgets_i->add_text("Will be saved as:");

							if (name_buffer[0])
							{
								g_gui_widgets_i->add_text(std::format("'{}.json'", name_buffer),
														  TEXTPROP_Wrapped, g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_16px, FDC_Regular));
							}
						},
						[&]() // on close
						{
							if (!name_buffer[0])
							{
								return;
							}

							auto path = g_config_mgr_i->get_config_directory(name_buffer);

							if (!path.has_extension() || path.extension() != ".json")
							{
								path.replace_extension("json");
							}

							if (g_config_mgr_i->write_configuration(CFG_CheatSettings, path.filename().string()))
							{
								status_widget.update_status(std::format("Saved '{}'.", path.filename().string()), UIStatusWidget::Success);
							}
							else
							{
								status_widget.update_status(std::format("Couldn't save '{}'.", path.filename().string()), UIStatusWidget::Error);
							}

							strcpy(name_buffer, "");
						},
							ImGuiWindowFlags_NoResize);
				}

				if (g_gui_widgets_i->add_button("open config directory", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
				{
					auto dir = g_config_mgr_i->get_config_directory().string();
					CGenericUtil::the().open_folder_inside_explorer(dir);
					status_widget.update_status("Opened config directory.", UIStatusWidget::Success);
				}

				if (g_gui_widgets_i->add_button("save current", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
				{
					if (g_config_mgr_i->write_configuration(CFG_CheatSettings, "saved.json"))
					{
						status_widget.update_status("Saved to saved.json.", UIStatusWidget::Success);
					}
					else
					{
						status_widget.update_status("Failed to save to saved.json!", UIStatusWidget::Error);
					}
				}

				if (g_gui_widgets_i->add_button("restore defaults", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
				{
					// this if-else tree is kinda dumb, but whatever xd
					if (g_config_mgr_i->load_configuration(CFG_CheatSettings, "default.json"))
					{
						status_widget.update_status("Restored default configuration.", UIStatusWidget::Success);
					}
					else
					{
						if (!g_config_mgr_i->write_configuration(CFG_CheatSettings, "default.json"))
						{
							status_widget.update_status("Failed to restore defaults!", UIStatusWidget::Error);
						}
						else
						{
							if (g_config_mgr_i->load_configuration(CFG_CheatSettings, "default.json"))
							{
								status_widget.update_status("Restored default configuration.", UIStatusWidget::Success);
							}
							else
							{
								status_widget.update_status("Failed to restore defaults!", UIStatusWidget::Error);
							}
						}

					}
				}

				g_gui_widgets_i->pop_stylevar();

				//
				// auto-save interval
				//

				g_gui_widgets_i->add_spacing();
				auto var = g_config_mgr_i->get_save_cfg_interval_var();

				g_gui_widgets_i->add_text("⏳ Auto-save interval");
				g_gui_widgets_i->add_spacing();
				CUIMenuWidgets::the().add_slider_nolabel("autosave_interval", "%0.0f seconds", var, "off");

				if (var->get_value() != var->get_min())
				{
					g_gui_widgets_i->add_progress_bar("Test", { -1.0f, 0.0f }, g_config_mgr_i->get_duration_last_saved_sec(), (float)g_config_mgr_i->get_save_cfg_interval_var()->get_value());
				}
			});

		g_gui_widgets_i->end_columns();
	}

	g_gui_widgets_i->add_child(
		"status", { -1.0f, -1.0f }, true, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[&]()
		{
			if (status_widget.is_alive())
			{
				g_gui_widgets_i->add_colored_text(status_widget.get_color(), status_widget.get_string());
			}
		});
}

void MenuChilden::Configuration::Theme::contents()
{
	g_gui_widgets_i->add_window_centered_text_disabled("To be done... Soon... 😉");
}

void MenuChilden::Configuration::Language::contents()
{
	g_gui_widgets_i->add_window_centered_text_disabled("To be done... Soon... 😉😁");
}

void MenuChilden::Configuration::KeyBinding::contents()
{
	CUIKeyBinding::the().render_interactible_bind_list();
}

void MenuChilden::Configuration::InCommands::contents()
{
	CUIInCommandKeyBinding::the().render_interactible_list();
}

