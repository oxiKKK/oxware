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

//--------------------------------------------------------------

void InCommandSimple::register_cmd()
{
	BaseCommand* cmd_dn = new BaseCommand("+" + m_command_name, "Toggles ON", 
										  [&](BaseCommand* cmd, const CmdArgs& args) { m_is_active = true; });
	BaseCommand* cmd_up = new BaseCommand("-" + m_command_name, "Toggles OFF", 
										  [&](BaseCommand* cmd, const CmdArgs& args) { m_is_active = false; });

	g_variablemgr_i->register_single_command(cmd_dn);
	g_variablemgr_i->register_single_command(cmd_up);
}

void InCommandSimple::add_to_global_list()
{
	CInCommands::the().add_command(this);
}

//--------------------------------------------------------------

void InCommandCustom::register_cmd()
{
	BaseCommand* cmd_dn = new BaseCommand("+" + m_command_name, "Toggles ON", 
										  [&](BaseCommand* cmd, const CmdArgs& args) { m_command_dn((InCommandCustom*)cmd_dn); m_is_active = true; });
	BaseCommand* cmd_up = new BaseCommand("-" + m_command_name, "Toggles OFF", 
										  [&](BaseCommand* cmd, const CmdArgs& args) { m_command_dn((InCommandCustom*)cmd_up); m_is_active = false; });

	g_variablemgr_i->register_single_command(cmd_dn);
	g_variablemgr_i->register_single_command(cmd_up);
}

void InCommandCustom::add_to_global_list()
{
	CInCommands::the().add_command(this);
}

//--------------------------------------------------------------

void CInCommands::add_command(BaseInCommand* in_cmd)
{
	m_in_commands.insert({ in_cmd->get_cmd_name(), in_cmd });

	CConsole::the().info("Added InCommand: '{}'.", in_cmd->get_cmd_name());
}

void CInCommands::initialize()
{
	for (const auto& [name, in_cmd] : m_in_commands)
	{
		in_cmd->register_cmd();
	}

	CConsole::the().info("Registered {} InCommands.", m_in_commands.size());
}
