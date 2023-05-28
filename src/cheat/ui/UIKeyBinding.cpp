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

void CUIKeyBinding::update()
{
	static bool once = true;
	if (once)
	{
		setup_interactible_bind_list();
		once = false;
	}

	keep_bound_keys_up_to_date();

	int vk_pressed = g_user_input_i->get_bound_key();
	if (vk_pressed != NULL)
	{
		// a key was just bound from the UserInput code

		end_key_binding_mode(vk_pressed);
	}
}

void CUIKeyBinding::render_interactible_bind_list()
{
	update();

	auto last_cursor_pos = g_gui_widgets_i->get_cursor_pos();

	CUIMenuWidgets::the().add_menu_child(
		"Binds", CMenuStyle::child_full_width(-1.0f), false, ImGuiWindowFlags_AlwaysUseWindowPadding,
		[&]()
		{
			g_gui_widgets_i->add_spacing();

			g_gui_widgets_i->push_stylevar(ImGuiStyleVar_CellPadding, { 2.0f, 1.0f });

			if (g_gui_widgets_i->begin_columns("binds_column", 2))
			{
				g_gui_widgets_i->setup_column_fixed_width(100.0f);
				g_gui_widgets_i->setup_column_fixed_width(283.0f);

				g_gui_widgets_i->goto_next_column();
				g_gui_widgets_i->add_text("Key name");
				g_gui_widgets_i->goto_next_column();
				g_gui_widgets_i->add_text("Command");

				g_gui_widgets_i->end_columns();
			}

			g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

			g_gui_widgets_i->add_separator();

			g_gui_widgets_i->add_child(
				"binds_list", Vector2D(-1.0f, -1.0f), false, ImGuiWindowFlags_None,
				[&]()
				{
					if (g_gui_widgets_i->begin_columns("binds_column_nested", 3))
					{
						g_gui_widgets_i->setup_column_fixed_width(100.0f);
						g_gui_widgets_i->setup_column_fixed_width(285.0f);

						// for the '...' button
						static struct popup_state_t
						{
							int vk;
							bound_key_t* key;
							std::string key_name;
						} popup_st{};

						int bind_to_be_removed = -1;
						std::pair<int, std::string> bind_to_be_rebound = { NULL, "" };
						g_bindmgr_i->for_each_bind(
							[&](int vk, const bind_t& bind)
							{
								g_gui_widgets_i->goto_next_column();

								std::string key_name = g_user_input_i->virtual_key_to_string(vk);
								if (key_name.empty())
								{
									bind_to_be_removed = vk;
									return;
								}

								add_keyscan_button(std::format("vk_{}", vk), vk, Vector2D(-1.0f, 25.0f));

								g_gui_widgets_i->goto_next_column();

								// unique insertion
								bound_key_t* bound_key = retreive_bound_key_or_insert_new(vk, bind);

								struct callback_data
								{
									size_t len;
									int vk;
									int n_buffer; // nth buffer
								};

								static auto textinput_callback = [](ImGuiInputTextCallbackData* data) -> int
								{
									switch (data->EventFlag)
									{
										case ImGuiInputTextFlags_CallbackAlways:
										{
											auto [last_length, vk, n_buffer] = *(callback_data*)data->UserData;
											if (last_length != data->BufTextLen)
											{
												if (data->BufTextLen != 0)
												{
													auto bind = g_bindmgr_i->get_bind(vk);

													if (bind)
													{
														if (n_buffer == 0)
														{
															// incommand
															if (data->Buf[0] == '+')
															{
																bind->cmd_sequence_0 = data->Buf;
																bind->cmd_sequence_1 = '-' + bind->cmd_sequence_0;
															}
															else
															{
																bind->cmd_sequence_0 = data->Buf;
															}
														}
														else // 1st buffer
														{
															bind->cmd_sequence_1 = data->Buf;
														}
													}
												}
											}
											break;
										}
									}
									return 1;
								};

								switch (bound_key->state)
								{
									case 0:
									{
										callback_data d = { strlen(bound_key->cmd_0.data()), vk, 0 };
										g_gui_widgets_i->add_text_input_ex(std::format("{}##0", key_name), bound_key->cmd_0.data(), buffer_len,
																		   Vector2D(-1.0f, 0.0f), ImGuiInputTextFlags_CallbackAlways,
																		   textinput_callback, (void*)&d);
										break;
									}
									case 1:
									{
										callback_data d = { strlen(bound_key->cmd_1.data()), vk, 1 };
										g_gui_widgets_i->add_text_input_ex(std::format("{}##1", key_name), bound_key->cmd_1.data(), buffer_len,
																		   Vector2D(-1.0f, 0.0f), ImGuiInputTextFlags_CallbackAlways,
																		   textinput_callback, (void*)&d);
										break;
									}
								}

								g_gui_widgets_i->goto_next_column();

								if (g_gui_widgets_i->add_button(std::format("...##{}", key_name), Vector2D(25.0f, 25.0f), false, BUTTONFLAG_CenterLabel))
								{
									popup_st = { vk, bound_key, key_name };

									g_gui_widgets_i->schedule_simple_popup_dialog("bind_popup");
								}
							});

						g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 8.0f, 8.0f });
						static bool was_popup_opened = false;
						bool popup_opened = g_gui_widgets_i->execute_simple_popup_popup(
							"bind_popup", { 140, 125 }, ImGuiWindowFlags_NoMove,
							[&]()
							{
								if (g_gui_widgets_i->add_button("Remove", { -1.0f, 0.0f }, false))
								{
									bind_to_be_removed = popup_st.vk; // remove after the iteration. avoids random iterator crashes
									g_gui_widgets_i->close_current_popup();
								}

								auto bound_key = popup_st.key;
								if (bound_key->has_two_cmds)
								{
									if (g_gui_widgets_i->add_button(std::format("{}##{}", bound_key->state == 1 ? "Switch to first" : "Switch to second", popup_st.key_name),
																	Vector2D(-1.0f, 0.0f), false))
									{
										bound_key->state ^= 1;
										g_gui_widgets_i->close_current_popup();
									}
								}

								g_gui_widgets_i->add_spacing();
								g_gui_widgets_i->add_separator();
								g_gui_widgets_i->add_spacing();

								if (g_gui_widgets_i->add_checkbox("Execute over UI", &bound_key->f_execute_over_ui))
								{
									auto bind = g_bindmgr_i->get_bind(popup_st.vk);
									if (bind)
									{
										if (bound_key->f_execute_over_ui)
										{
											bind->flags |= BINDFLAG_ExecuteOverUI;
										}
										else
										{
											bind->flags &= ~BINDFLAG_ExecuteOverUI;
										}
									}
								}

								if (g_gui_widgets_i->add_checkbox("Silent", &bound_key->f_silent))
								{
									auto bind = g_bindmgr_i->get_bind(popup_st.vk);
									if (bind)
									{
										if (bound_key->f_silent)
										{
											bind->flags |= BINDFLAG_Silent;
										}
										else
										{
											bind->flags &= ~BINDFLAG_Silent;
										}
									}
								}
							});
						g_gui_widgets_i->pop_stylevar();

						// on close
						if (was_popup_opened != popup_opened && !popup_opened)
						{
							popup_st = {};
							was_popup_opened = popup_opened;
						}

						if (bind_to_be_removed != -1)
						{
							m_bound_keys[bind_to_be_removed] = {};
							g_bindmgr_i->remove_bind(bind_to_be_removed);
						}

						g_gui_widgets_i->goto_next_column();

						if (g_gui_widgets_i->add_button("+##newbind", Vector2D(25.0f, 25.0f), false, BUTTONFLAG_CenterLabel))
						{
							g_gui_widgets_i->schedule_simple_popup_dialog("new_bind_popup");
						}

						g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 8.0f, 8.0f });

						g_gui_widgets_i->execute_simple_popup_popup(
							"new_bind_popup", { 120, 60 }, ImGuiWindowFlags_NoMove,
							[&]()
							{
								static bool close_current_popup = false;
								if (close_current_popup)
								{
									g_gui_widgets_i->close_current_popup();
									close_current_popup = false;
								}

								if (g_gui_widgets_i->add_button("On Push", { -1.0f, 0.0f }, false))
								{
									switch_to_key_binding_mode(&m_key_scan_button_info["new_key"], [&](key_scan_button_info_t* info, int vk)
										{
											// create new fresh bind
											if (!g_bindmgr_i->is_key_bound(vk))
											{
												g_bindmgr_i->add_bind(vk, "");
												update_keyscan_button_title(vk);
											}
											close_current_popup = true;
										});
								}
								
								if (g_gui_widgets_i->add_button("On Push and Release", { -1.0f, 0.0f }, false))
								{
									switch_to_key_binding_mode(&m_key_scan_button_info["new_key"], [&](key_scan_button_info_t* info, int vk)
										{
											// create new fresh bind
											if (!g_bindmgr_i->is_key_bound(vk))
											{
												g_bindmgr_i->add_bind_on_push_and_release(vk, "", "");
												update_keyscan_button_title(vk);
											}
											close_current_popup = true;
										});
								}
							});

						g_gui_widgets_i->pop_stylevar();

						g_gui_widgets_i->end_columns();
					}
				});

			g_gui_widgets_i->pop_stylevar();
			g_gui_widgets_i->pop_stylevar();
		});

	auto window_size = g_gui_widgets_i->get_current_window_size();
	auto button_size = Vector2D(25, 25);
	if (g_gui_widgets_i->add_floating_button(";", last_cursor_pos, { window_size.x - 23.0f - button_size.x, 30.0f },
											 button_size, false, BUTTONFLAG_CenterLabel))
	{
		CGenericUtil::the().copy_to_clipboard(";");
	}

	std::string label = "unbind all";
	auto label_size = g_gui_fontmgr_i->calc_font_text_size(g_gui_fontmgr_i->get_default_font(), label.c_str());
	auto button1_size = Vector2D(label_size.x + 5.0f * 2, 25);
	if (g_gui_widgets_i->add_floating_button(label, last_cursor_pos, { window_size.x - 23.0f - button_size.x - 3.0f - button1_size.x, 30.0f },
											 button1_size, false, BUTTONFLAG_CenterLabel))
	{
		m_bound_keys.clear();
		g_bindmgr_i->remove_all_binds();
	}

	if (g_gui_widgets_i->add_floating_button("?", last_cursor_pos, { window_size.x - 23.0f - button_size.x - 3.0f - button1_size.x - 3.0f - button_size.x, 30.0f },
											 button_size, false, BUTTONFLAG_CenterLabel))
	{
		COxWareUI::the().schedule_popup(
			"Binding help", { 450, 400 },
			[]()
			{
				g_gui_widgets_i->add_separtor_with_text("Types of binds");

				g_gui_widgets_i->add_text("There are currently two types of binds:", TEXTPROP_Wrapped);
				g_gui_widgets_i->add_bullet_text("On push - Activates the command when the key has been pressed, only once.", TEXTPROP_Wrapped);
				g_gui_widgets_i->add_bullet_text("On push and release - Activates the first command when the key has been pressed and then the second command when the key has been unpressed.", TEXTPROP_Wrapped);
				g_gui_widgets_i->add_bullet_text("InCommand - Involves binding commands with \"+\" prefix, just like in CS 1.6 (+attack, etc.)", TEXTPROP_Wrapped);

				g_gui_widgets_i->add_padding({ 0.0f, 10.0f });
				g_gui_widgets_i->add_separtor_with_text("Ways of binding a key");

				g_gui_widgets_i->add_text("You can bind a key using either the console or from the menu.", TEXTPROP_Wrapped);

				g_gui_widgets_i->add_padding({ 0.0f, 10.0f });
				g_gui_widgets_i->add_separtor_with_text("Binding through the console");

				g_gui_widgets_i->add_text("Use the \"bind\" or \"bind_on_push_and_release\" command to bind command to a key. Example:", TEXTPROP_Wrapped);
				g_gui_widgets_i->add_bullet_text("bind \"f4\" \"ui_toggle_menu\" - This command binds the UI toggle command to f4.", TEXTPROP_Wrapped);
				g_gui_widgets_i->add_bullet_text("bind_on_push_and_release\"f5\" \"print pushed!\" \"print unpushed!\" - As soon as f4 is pressed, \"pushed!\" is printed to the console. As soon as it is unpressed, \"unpushed!\" is printed.", TEXTPROP_Wrapped);

				g_gui_widgets_i->add_padding({ 0.0f, 10.0f });
				g_gui_widgets_i->add_separtor_with_text("Binding through the UI");

				g_gui_widgets_i->add_text("Use the UI to bind any command sequence to a key. Inside the \"bind\" tab, there are all of the current binds listed.", TEXTPROP_Wrapped);

				g_gui_widgets_i->add_padding({ 0.0f, 10.0f });
				g_gui_widgets_i->add_separtor_with_text("Binding InCommands");

				g_gui_widgets_i->add_text("In order to bind an InCommand, use the \"bind\" command and use commands with \"+\" prefix.", TEXTPROP_Wrapped);
				g_gui_widgets_i->add_text("For example: bind \"V\" \"+movement_bhop\".", TEXTPROP_Wrapped);
				g_gui_widgets_i->add_spacing();
				g_gui_widgets_i->add_text("You can also use the UI. Just create a new \"On Push\" bind, and add a command with \"+\" prefix", TEXTPROP_Wrapped);
			});
	}
}

void CUIKeyBinding::add_keyscan_button(const std::string& id, int vk, const Vector2D& size, bool* force_binding_mode)
{
	// unique insertion
	key_scan_button_info_t* info;
	try
	{
		info = &m_key_scan_button_info.at(id);
	}
	catch (...)
	{
		m_key_scan_button_info.insert(std::make_pair(id, key_scan_button_info_t{}));
		return;
	}

	bool b = g_gui_widgets_i->add_button(std::format("{}##{}", info->label, id), size, false, BUTTONFLAG_CenterLabel);

	if (b || (force_binding_mode && *force_binding_mode))
	{
		if (info->prev_vk != vk)
		{
			info->prev_vk = vk;
		}

		switch_to_key_binding_mode(
			info, 
			[&](key_scan_button_info_t* info, int vk)
			{
				// rebind same command to different key
				if (info->prev_vk != vk)
				{
					auto bind = g_bindmgr_i->get_bind(info->prev_vk);
					if (bind)
					{
						if (!g_bindmgr_i->is_key_bound(vk))
						{
							// bind to a new key
							if (bind->type == BIND_OnPushAndRelease)
							{
								g_bindmgr_i->add_bind_on_push_and_release(vk, bind->cmd_sequence_0, bind->cmd_sequence_1);
							}
							else
							{
								g_bindmgr_i->add_bind(vk, bind->cmd_sequence_0);
							}

							g_bindmgr_i->remove_bind(info->prev_vk); // remove old bind
							insert_new_key_scan_button_info(vk);
						}
					}
				}

				info->prev_vk = 0;
			});

		if (force_binding_mode)
		{
			*force_binding_mode = false;
		}
	}
}

void CUIKeyBinding::setup_interactible_bind_list()
{
	// before we start the rendering, setup the initial list

	g_bindmgr_i->for_each_bind(
		[&](int vk, const bind_t& bind)
		{
			insert_new_key_bind(vk, bind);

			// and update the data.
			strncpy(m_bound_keys[vk].cmd_0.data(), bind.cmd_sequence_0.c_str(), buffer_len);
			strncpy(m_bound_keys[vk].cmd_1.data(), bind.cmd_sequence_1.c_str(), buffer_len);

			if (bind.type == BIND_OnPushAndRelease)
			{
				m_bound_keys[vk].has_two_cmds = true;
			}

			insert_new_key_scan_button_info(vk);
		});
}

void CUIKeyBinding::update_keyscan_button_title(int new_vk)
{
	std::string key_name = g_user_input_i->virtual_key_to_string(new_vk);
	if (key_name.empty())
	{
		key_name = "<invalid key>";
	}
	m_key_scan_button_info[std::format("vk_{}", new_vk)].label = key_name;
}

void CUIKeyBinding::switch_to_key_binding_mode(key_scan_button_info_t* info,
											   const fnOnKeyBoundCallback& on_key_bound_callback)
{
	assert(!m_current_key_bound && "Tried to bind new key while the old one was still in the process of binding!");

	m_current_key_bound = info;

	if (!m_on_key_bound_callback)
	{
		m_on_key_bound_callback = on_key_bound_callback;
	}
	g_user_input_i->activate_key_binding_mode();
}

void CUIKeyBinding::end_key_binding_mode(int vk_pressed)
{
	if (vk_pressed != VK_LBUTTON && vk_pressed != VK_ESCAPE)
	{
		if (m_on_key_bound_callback)
		{
			m_on_key_bound_callback(m_current_key_bound, vk_pressed);
			m_on_key_bound_callback = nullptr;
		}
	}

	// reset all states for the future
	m_current_key_bound = nullptr;
	g_user_input_i->reset_bound_key();
}

void CUIKeyBinding::keep_bound_keys_up_to_date()
{
	// refresh the list so that it keeps up to date
	static uint32_t t = GetTickCount();
	if (GetTickCount() - t > 500)
	{
		g_bindmgr_i->for_each_bind(
			[&](int vk, const bind_t& bind)
			{
				strncpy(m_bound_keys[vk].cmd_0.data(), bind.cmd_sequence_0.c_str(), buffer_len);
				strncpy(m_bound_keys[vk].cmd_1.data(), bind.cmd_sequence_1.c_str(), buffer_len);

				if (bind.type == BIND_OnPushAndRelease)
				{
					m_bound_keys[vk].has_two_cmds = true;
				}

				m_bound_keys[vk].f_execute_over_ui = (float)(bind.flags & BINDFLAG_ExecuteOverUI);
				m_bound_keys[vk].f_silent = (float)(bind.flags & BINDFLAG_Silent);

				update_keyscan_button_title(vk);
			});

		t = GetTickCount();
	}
}

bound_key_t* CUIKeyBinding::retreive_bound_key_or_insert_new(int vk, const bind_t& bind)
{
	try
	{
		// this should be O(1), but hehe, whatever..
		return &m_bound_keys.at(vk);
	}
	catch (...)
	{
		return insert_new_key_bind(vk, bind);
	}
}

bound_key_t* CUIKeyBinding::insert_new_key_bind(int vk, const bind_t& bind)
{
	auto [iter, did_insert] = m_bound_keys.insert(std::make_pair(vk, bound_key_t()));

	if (bind.type == BIND_OnPushAndRelease)
	{
		(*iter).second.has_two_cmds = true;
	}

	(*iter).second.f_execute_over_ui = (float)(bind.flags & BINDFLAG_ExecuteOverUI);
	(*iter).second.f_silent = (float)(bind.flags & BINDFLAG_Silent);

	strncpy((*iter).second.cmd_0.data(), bind.cmd_sequence_0.c_str(), buffer_len);
	strncpy((*iter).second.cmd_1.data(), bind.cmd_sequence_1.c_str(), buffer_len);
	return &(*iter).second;
}

void CUIKeyBinding::insert_new_key_scan_button_info(int vk)
{
	m_key_scan_button_info.insert(std::make_pair(std::format("vk_{}", vk), g_user_input_i->virtual_key_to_string(vk)));
}
