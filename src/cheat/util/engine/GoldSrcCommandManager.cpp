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

void CGoldSrcCommandMgr::initialize()
{
	// cvars and commands in GS are registered one and never unregistered, so this is a one-way ticked here.

	int cvars = 0;
	for_each_cvar(
		[&](hl::cvar_t* cvar)
		{
			cvars++;
		});

	int cmds = 0;
	for_each_cmd(
		[&](hl::cmd_function_t* cmd)
		{
			cmds++;
		});

	m_cvars.reserve(cvars);
	m_commands.reserve(cmds);

	for_each_cvar(
		[&](hl::cvar_t* cvar)
		{
			register_cvar(cvar->name, cvar);
		});

	for_each_cmd(
		[&](hl::cmd_function_t* cmd)
		{
			register_cmd(cmd->name, cmd);
		});

	CConsole::the().info("Registered {} cvars and {} commands.", m_cvars.size(), m_commands.size());

#if 0 // measurements (MEASURED IN DEBUG MODE, MAY BE MUCH FASTER IN RETAIL OR RELEASE)
	auto t1 = std::chrono::high_resolution_clock::now();
	auto _cvar = get_cvar("fps_max");//
	CConsole::the().info("Took {} seconds (ours)", std::chrono::duration<float, std::ratio<1, 1>>(std::chrono::high_resolution_clock::now() - t1).count());

	t1 = std::chrono::high_resolution_clock::now();
	_cvar = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetCvarPointer((char*)"fps_max");
	CConsole::the().info("Took {} seconds (vanilla)", std::chrono::duration<float, std::ratio<1, 1>>(std::chrono::high_resolution_clock::now() - t1).count());

	//
	// cvar query results:
	// ours:	7.4e-06 seconds		-> 10x faster
	// vanilla:	1.18e-05 seconds
	//

	t1 = std::chrono::high_resolution_clock::now();
	for_each_cvar(
		 [](hl::cvar_t* cvar)
		{
			// stuff
		});
	CConsole::the().info("Took {} seconds (ours iteration)", std::chrono::duration<float, std::ratio<1, 1>>(std::chrono::high_resolution_clock::now() - t1).count());

	t1 = std::chrono::high_resolution_clock::now();
	auto cvar = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetFirstCvarPtr();
	while (cvar)
	{
		// stuff
		cvar = cvar->next;
	}
	CConsole::the().info("Took {} seconds (vanilla iteration)", std::chrono::duration<float, std::ratio<1, 1>>(std::chrono::high_resolution_clock::now() - t1).count());

	//
	// cvar iteration results:
	// ours:	0.0001631 seconds (iterating through our map)		-> 10x slower than linked list
	// vanilla:	1.17e-05 seconds (iterating through a linked list)	
	//
#endif

	CConsole::the().info("GoldSrc Command Manager initialized.");
}

void CGoldSrcCommandMgr::shutdown()
{
	CConsole::the().info("Shutting down GoldSrc Command Manager...");

	m_cvars.clear();
	m_commands.clear();
}

hl::cvar_t* CGoldSrcCommandMgr::get_cvar(const std::string& name)
{
	try
	{
		return m_cvars.at(name);
	}
	catch (...)
	{
		CConsole::the().info("Unregistered cvar '{}'.", name);
		return nullptr;
	}
}

hl::cmd_function_t* CGoldSrcCommandMgr::get_cmd(const std::string& name)
{
	try
	{
		return m_commands.at(name);
	}
	catch (...)
	{
		CConsole::the().info("Unregistered command '{}'.", name);
		return nullptr;
	}
}

void CGoldSrcCommandMgr::register_cvar(const std::string& name, hl::cvar_t* cvar)
{
	m_cvars.insert({ cvar->name, cvar });
}

void CGoldSrcCommandMgr::register_cmd(const std::string& name, hl::cmd_function_t* cmd)
{
	m_commands.insert({ cmd->name, cmd });
}

void CGoldSrcCommandMgr::for_each_cvar(const std::function<void(hl::cvar_t*cvar)>& callback)
{
	// in terms of iteration, going through a linked list is faster than through our map 
	// (I have measured it and its about 10x faster)
	auto cvar = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetFirstCvarPtr();
	while (cvar)
	{
		callback(cvar);
		cvar = cvar->next;
	}
}

void CGoldSrcCommandMgr::for_each_cmd(const std::function<void(hl::cmd_function_t*cmd)>& callback)
{
	// in terms of iteration, going through a linked list is faster than through our map 
	// (I have measured it and its about 10x faster)
	auto cmd = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetFirstCmdFunctionHandle();
	while (cmd)
	{
		callback(cmd);
		cmd = cmd->next;
	}
}

std::vector<std::string> CGoldSrcCommandMgr::find_invalid_tokens_in_tokenized_buffer(const std::vector<std::string>& tokenized_buffer)
{
	std::vector<std::string> invalid_tokens;

	for (auto& token : tokenized_buffer)
	{
		if (token.empty())
		{
			continue;
		}

		bool found = false;
		// try cvars
		for_each_cvar(
			[&](hl::cvar_t*cvar)
			{
				if (token == cvar->name)
				{
					found = true;
					return;
				}
			});

		// try commands
		for_each_cmd(
			[&](hl::cmd_function_t*cmd)
			{
				if (token == cmd->name)
				{
					found = true;
					return;
				}
			});

		// didn't find both in cvars&cmds, invalid token
		if (!found)
		{
			invalid_tokens.push_back(token);
		}
	}

	return invalid_tokens;
}
