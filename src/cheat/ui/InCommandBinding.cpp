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

void CUIInCommandKeyBinding::update()
{
	static bool once = true;
	if (once)
	{
		setup_interactible_list();
		once = false;
	}

	keep_ui_elements_up_to_date();

	// We can check this way if we started the key binding mode from our code.
	// Because it could be started elsewhere, e.g. bindmanager code.
	bool did_start_key_binding_mode = m_in_cmd_to_be_rebound != nullptr;

	if (did_start_key_binding_mode)
	{
		int vk_pressed = g_user_input_i->get_bound_key();
		if (vk_pressed != NULL)
		{
			// a key was just bound from the UserInput code

			end_key_binding_mode(vk_pressed);
		}
	}
}

void CUIInCommandKeyBinding::render_interactible_list()
{
	auto last_cursor_pos = g_gui_widgets_i->get_cursor_pos();

	g_gui_widgets_i->add_spacing();

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_CellPadding, { 2.0f, 1.0f });

	if (g_gui_widgets_i->begin_columns("incommands_column", 5))
	{
		g_gui_widgets_i->setup_column_fixed_width(100.0f);
		g_gui_widgets_i->setup_column_fixed_width(50.0f);
		g_gui_widgets_i->setup_column_fixed_width(50.0f);
		g_gui_widgets_i->setup_column_fixed_width(50.0f);
		g_gui_widgets_i->setup_column_fixed_width(283.0f);

		g_gui_widgets_i->goto_next_column();
		g_gui_widgets_i->add_text("Key name");
		g_gui_widgets_i->goto_next_column();
		g_gui_widgets_i->add_text("Enable");
		g_gui_widgets_i->goto_next_column();
		g_gui_widgets_i->add_text("Always");
		g_gui_widgets_i->goto_next_column();
		g_gui_widgets_i->add_text("Options");
		g_gui_widgets_i->goto_next_column();
		g_gui_widgets_i->add_text("Command");

		g_gui_widgets_i->end_columns();
	}

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

	g_gui_widgets_i->add_separator();

	g_gui_widgets_i->add_child(
		"incommands_list", Vector2D(-1.0f, -1.0f), false, ImGuiWindowFlags_None,
		[&]()
		{
			if (g_gui_widgets_i->begin_columns("binds_column_nested", 5))
			{
				g_gui_widgets_i->setup_column_fixed_width(100.0f);
				g_gui_widgets_i->setup_column_fixed_width(50.0f);
				g_gui_widgets_i->setup_column_fixed_width(50.0f);
				g_gui_widgets_i->setup_column_fixed_width(50.0f);
				g_gui_widgets_i->setup_column_fixed_width(285.0f);

				// for the '...' button
				static struct popup_state_t
				{
					BaseInCommand* in_cmd;
					ui_incmd_element_t* ui_element;
				} popup_st{};

				g_in_commands_i->for_each_incommand(
					[&](BaseInCommand* in_cmd)
					{
						auto& name = in_cmd->get_name();

						g_gui_widgets_i->goto_next_column();

						int vk = in_cmd->get_key_bound();

						add_keyscan_button(in_cmd, Vector2D(-1.0f, 25.0f));

						// unique insertion
						ui_incmd_element_t* ui_element = retreive_ui_element_or_insert_new(in_cmd);

						g_gui_widgets_i->goto_next_column();

						auto toggle_var = in_cmd->get_toggle_var();
						CUIMenuWidgets::the().add_checkbox(std::format("##{}_0", name), toggle_var);

						g_gui_widgets_i->goto_next_column();

						auto always_enabled_var = in_cmd->get_always_enabled_var();
						CUIMenuWidgets::the().add_checkbox(std::format("##{}_1", name), always_enabled_var);

						g_gui_widgets_i->goto_next_column();

						if (g_gui_widgets_i->add_button(std::format("...##{}", name), Vector2D(25.0f, 25.0f), false, BUTTONFLAG_CenterLabel))
						{
							popup_st = { in_cmd, ui_element };

							g_gui_widgets_i->schedule_simple_popup_dialog("options_popup");
						}

						g_gui_widgets_i->goto_next_column();

						g_gui_widgets_i->add_text(name);
					}
				);

				g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 8.0f, 8.0f });
				static bool was_popup_opened = false;
				bool popup_opened = g_gui_widgets_i->execute_simple_popup_popup(
					"options_popup", { 180, 40 }, ImGuiWindowFlags_NoMove,
					[&]()
					{
						auto ui_element = popup_st.ui_element;
						auto in_cmd = popup_st.in_cmd;

						if (g_gui_widgets_i->add_checkbox("Disable in-game key", &ui_element->f_disable_ingame_key))
						{
							in_cmd->add_flags(IN_FLAG_DisableInGameKey, ui_element->f_disable_ingame_key);
						}
					});
				g_gui_widgets_i->pop_stylevar();

				// on close
				if (was_popup_opened != popup_opened && !popup_opened)
				{
					popup_st = {};
					was_popup_opened = popup_opened;
				}

				g_gui_widgets_i->end_columns();
			}
		});

	g_gui_widgets_i->pop_stylevar();
	g_gui_widgets_i->pop_stylevar();
}

void CUIInCommandKeyBinding::add_keyscan_button(BaseInCommand* in_cmd, const Vector2D& size)
{
	std::string key_name;

	int vk_bound = in_cmd->get_key_bound();
	if (vk_bound != NULL)
	{
		key_name = g_user_input_i->virtual_key_to_string(in_cmd->get_key_bound());
	}
	else
	{
		key_name = "unbound";
	}

	bool b = g_gui_widgets_i->add_button(std::format("{}##{}", key_name, in_cmd->get_name()), size, false, BUTTONFLAG_CenterLabel);
	if (b)
	{
		m_in_cmd_to_be_rebound = in_cmd;
		g_user_input_i->activate_key_binding_mode();
	}
}

void CUIInCommandKeyBinding::setup_interactible_list()
{
	// before we start the rendering, setup the initial list

	g_in_commands_i->for_each_incommand(
		[&](BaseInCommand* in_cmd)
		{
			insert_new_ui_element(in_cmd);
		});
}

void CUIInCommandKeyBinding::end_key_binding_mode(int vk_pressed)
{
	if (vk_pressed != VK_LBUTTON && vk_pressed != VK_ESCAPE)
	{
		m_in_cmd_to_be_rebound->rebind_key_to(vk_pressed);
	}

	m_in_cmd_to_be_rebound = nullptr;
	g_user_input_i->reset_bound_key();
}

ui_incmd_element_t* CUIInCommandKeyBinding::retreive_ui_element_or_insert_new(BaseInCommand* in_cmd)
{
	try
	{
		return &m_ui_elements.at(in_cmd->get_name());
	}
	catch (...)
	{
		return insert_new_ui_element(in_cmd);
	}
}

ui_incmd_element_t* CUIInCommandKeyBinding::insert_new_ui_element(BaseInCommand* in_cmd)
{
	auto [iter, did_insert] = m_ui_elements.insert(std::make_pair(in_cmd->get_name(), ui_incmd_element_t()));

	(*iter).second.f_disable_ingame_key = (in_cmd->get_flags() & IN_FLAG_DisableInGameKey);

	return &(*iter).second;
}

void CUIInCommandKeyBinding::keep_ui_elements_up_to_date()
{
	// refresh the list so that it keeps up to date with the actual list
	static uint32_t t = GetTickCount();
	if (GetTickCount() - t > 500)
	{
		g_in_commands_i->for_each_incommand(
			[&](BaseInCommand* in_cmd)
			{
				auto& ui_element = m_ui_elements[in_cmd->get_name()];

				ui_element.f_disable_ingame_key = (in_cmd->get_flags() & IN_FLAG_DisableInGameKey);
			});

		t = GetTickCount();
	}
}