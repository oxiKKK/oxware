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

	tokenize_user_cmd_filter(filtered_cmds);

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

bool CStuffCmdFilter::approve_server_cmdlist(const std::string& server_cmd)
{
	m_block_victim.clear();
	
	for (const auto& token : m_filtered_cmds)
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

void CStuffCmdFilter::tokenize_user_cmd_filter(const std::string& filtered_cmds)
{
	m_filtered_cmds.clear();

	// tokenize a buffer separated by commas ";".
	std::string current_token;
	for (size_t i = 0; i < filtered_cmds.length(); i++)
	{
		const char c = filtered_cmds[i];

		bool last = (i == filtered_cmds.length() - 1);

		if (c == ';' || last) // the last case, too
		{
			if (last)
			{
				current_token.push_back(c);
			}

			m_filtered_cmds.push_back(current_token);
			current_token.clear();
		}
		else
		{
			current_token.push_back(c);
		}
	}
}