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

VarBoolean cmdfilter_enable("cmdfilter_enable", "Enable server command filter", false);
VarBoolean cmdfilter_print_blocked("cmdfilter_print_blocked", "Allows blocked commands to be printed to the console", false);
VarBoolean cmdfilter_print_every("cmdfilter_print_every", "Prints every command send by the server to be executed", false);
VarBoolean cmdfilter_filter_all("cmdfilter_filter_all", "Block all commands send to you by the server", false);
VarString cmdfilter_filtered_commands("cmdfilter_filtered_commands", "List of commands to be filtered separated by a \";\".", "fps_max; developer; fps_override");

// enable this to measure in ms how long does it take to query through cvars and cmds when searching for the command
//#define DEBUG_MSR_CMDCVAR_QUERY

UIStatusWidget CStuffCmdFilter::m_status_widget = UIStatusWidget(k_StatusWidgetStandardLifeDur);

bool CStuffCmdFilter::allow_command_to_be_executed()
{
	if (!cmdfilter_enable.get_value())
	{
		return true;
	}

	// see if we have some cmds to be filtered
	std::string filtered_cmds = cmdfilter_filtered_commands.get_value_string();
	if (filtered_cmds.empty())
	{
		return true;
	}

	m_filtered_cmds_tokenized = CGameUtil::the().tokenize_goldsrc_command(filtered_cmds.c_str());

	CHLNetMessageIO::the().begin_silent_reading();

	std::string server_cmd_buffer = CHLNetMessageIO::the().read_string();

	// erase newlines from the buffer, they make mess in our console
	std::string server_cmd_buffer_formatted = server_cmd_buffer;
	std::erase(server_cmd_buffer_formatted, '\n');

	bool found_cvar_or_cmd = false;
	bool nonexistent_cmd_or_cvar_from_server = false;
	
#ifdef DEBUG_MSR_CMDCVAR_QUERY
	auto t1 = std::chrono::high_resolution_clock::now();
#endif

	//
	// Note #1: We need to filter out only ingame cvars & commands, because the server can send other things
	//			such as connection validation tokens, etc. This obviously won't get executed on the client, 
	//			because it isn't a command nor cvar however, it is being forwarded back to the server using
	//			Cmd_ForwardToServer() and some servers use this as a form of "validation". This is just an example
	//			of such no-cvar/cmd stufftext we can get.
	// 
	// Note #2: This usually takes <1ms to query through even in debug mode. Fast enough..
	//

	// see for all cvars
	auto cvar = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetFirstCvarPtr();
	while (cvar)
	{
		if (server_cmd_buffer_formatted.find(cvar->name) != std::string::npos)
		{
			found_cvar_or_cmd = true;
			break;
		}

		cvar = cvar->next;
	}

	// see for all commands
	if (!found_cvar_or_cmd)
	{
		auto cmd = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetFirstCmdFunctionHandle();
		while (cmd)
		{
			if (server_cmd_buffer_formatted.find(cmd->name) != std::string::npos)
			{
				found_cvar_or_cmd = true;
				break;
			}

			cmd = cmd->next;
		}
	}

	if (!found_cvar_or_cmd)
	{
		// the server can send nonexistent commands/cvars that are then sent back to it using Cmd_ForwardToServer().
		nonexistent_cmd_or_cvar_from_server = true;
	}

#ifdef DEBUG_MSR_CMDCVAR_QUERY
	auto t2 = std::chrono::high_resolution_clock::now();

	CConsole::the().info("Took {} ms to query through cvars&cmds.", 
						 std::chrono::duration<float, std::milli>(t2 - t1).count());
#endif

	// if we force to block some cmds, block only those that are actually valid.
	bool forced_to_block = cmdfilter_filter_all.get_value() && found_cvar_or_cmd;

	if (cmdfilter_print_every.get_value())
	{
		if (forced_to_block)
		{
			CConsole::the().info("Blocked command sequence: '{}'", server_cmd_buffer_formatted);
		}
		else
		{
			if (nonexistent_cmd_or_cvar_from_server)
			{
				CConsole::the().info("Server executing non-existent command sequence: '{}'", server_cmd_buffer_formatted);
			}
			else
			{
				CConsole::the().info("Server executing command sequence: '{}'", server_cmd_buffer_formatted);
			}
		}
	}

	if (forced_to_block)
	{
		CHLNetMessageIO::the().end_silent_reading_no_restore();
		return false;
	}

	if (!approve_server_cmdlist(server_cmd_buffer))
	{
		if (cmdfilter_print_blocked.get_value())
		{
			if (cmdfilter_print_every.get_value())
			{
				CConsole::the().info("Blocked because of: '{}'", m_block_victim);
			}
			else
			{
				CConsole::the().info("Blocked command sequence '{}'", server_cmd_buffer_formatted);
			}
		}
		// didn't allow something, don't restore the reading state, because we won't call the original function.
		CHLNetMessageIO::the().end_silent_reading_no_restore();
		return false;
	}

	// didn't find anything, pass the buffer to the engine.
	CHLNetMessageIO::the().end_silent_reading();
	return true;
}

void CStuffCmdFilter::render_ui()
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

	auto window_size = g_gui_widgets_i->get_current_window_size();

	g_gui_widgets_i->add_table(
		"cmdfilter_table", 2,
		ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_HeaderTextOnly,
		[&]()
		{
			static auto column_flags = ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize;
			g_gui_widgets_i->table_setup_column_fixed_width("Commands to be filtered", window_size.x - 100.0f, column_flags);

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
					if (is_buffer_valid(cmd_buffer))
					{
						m_status_widget.update_status("Updated!", UIStatusWidget::Success);
					}

					cmdfilter_filtered_commands.set_value(cmd_buffer);
				}
			}
		});

	float footer_height = 50.0f;
	g_gui_widgets_i->set_cursor_pos({ 0.0f, window_size.y - footer_height });

	g_gui_widgets_i->add_text("Status");
	g_gui_widgets_i->add_child(
		"Status", 
		{ -1.0f, -1.0f }, 
		true, ImGuiWindowFlags_None, 
		[&]()
		{
			if (m_status_widget.is_alive())
			{
				g_gui_widgets_i->add_colored_text(m_status_widget.get_color() , m_status_widget.get_string());
			}
		});
}

bool CStuffCmdFilter::is_buffer_valid(const char* buffer)
{
	auto tokens = CGameUtil::the().tokenize_goldsrc_command(buffer);

	if (tokens.empty())
	{
		m_status_widget.update_status("No commands!", UIStatusWidget::Warning);
		return false;
	}

	auto invalid_tokens = CGoldSrcCommandMgr::the().find_invalid_tokens_in_tokenized_buffer(tokens);
	if (invalid_tokens.empty())
	{
		return true; // if it's empty, there aren't any invalid tokens.
	}

	// got some invalid tokens, add them to the message
	std::string message = "Invalid commands: ";
	uint32_t n = 0;
	for (const auto& invalid : invalid_tokens)
	{
		message += invalid;
		if (n != invalid_tokens.size() - 1)
		{
			message += ", ";
		}

		n++;
	}

	m_status_widget.update_status(message, UIStatusWidget::Error);
	return false;
}

bool CStuffCmdFilter::approve_server_cmdlist(const std::string& server_cmd)
{
	m_block_victim.clear();
	
	for (const auto& token : m_filtered_cmds_tokenized)
	{
		if (server_cmd.find(token) != std::string::npos)
		{
			// if one of the blacklisted commands were found inside the buffer, don't allow the whole buffer.
			// this is also the way engine does it.
			m_block_victim = token;
			return false;
		}
	}

	return true;
}
