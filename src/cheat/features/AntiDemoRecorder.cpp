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

VarBoolean anti_demo_recorder_enable("anti_demo_recorder_enable", "Blocks when server starts to record demos", false);

bool CAntiDemoRecorder::allow_demo_to_be_recorded()
{
	// called inside svc_stufftext

	if (!anti_demo_recorder_enable.get_value())
	{
		return false;
	}

	CHLNetMessageIO::the().begin_silent_reading();

	std::string cmd_buffer = CHLNetMessageIO::the().read_string();

	size_t record_command = cmd_buffer.find("record");
	if (record_command != std::string::npos)
	{
		// parse out e.g.: record "PPC-ZONE HNS"
		std::string record_command_str_begin = cmd_buffer.substr(record_command);
		auto tokens = CGameUtil::the().tokenize_goldsrc_command(record_command_str_begin.c_str());

		std::string demo_name;
		if (tokens.size() > 1)
		{
			demo_name = tokens[1];
		}

		if (!demo_name.empty())
		{
			CConsole::the().info("Server tried to record demo '{}'. Blocked...", demo_name);
			CHLNetMessageIO::the().end_silent_reading_no_restore();
			return true;
		}
		else
		{
			CConsole::the().error("Server tried to send invalid record command?");
			CConsole::the().error("Full: '{}'", cmd_buffer);
		}
	}

	CHLNetMessageIO::the().end_silent_reading();
	return false;
}
