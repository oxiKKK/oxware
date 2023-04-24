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

#include "Console.h"

#include "BaseVariable.h"

//--------------------------------------------------------------------------------------------------------------------
// VARIABLES
//

StaticVariableContainer g_static_variable_container;

void StaticVariableContainer::add_variable(BaseVariable* var)
{
	static bool reserved = false;
	if (!reserved)
	{
		reserve_limit();
		reserved = true;
	}

	if (!see_for_overflow(var))
	{
		return;
	}

	auto [iter, did_insert] = m_variables.insert(var);

	assert(did_insert && "Failed to insert a variable because it's already in the list.. duplicated?");
}

void StaticVariableContainer::reserve_limit()
{
	m_variables.reserve(k_max_variables_per_module);

	CConsole::the().info("Reserved {} slots for variables for current modules.", k_max_variables_per_module);
}

bool StaticVariableContainer::see_for_overflow(BaseVariable* current_var)
{
	if (m_variables.size() >= k_max_variables_per_module)
	{
		CConsole::the().error("Variable {} couldn't be added, because the limit per this module has already been reached. ({})", 
							  current_var->get_name(), k_max_variables_per_module);
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------
// COMMANDS
//

StaticCommandContainer g_static_command_container;

void StaticCommandContainer::add_command(BaseCommand* var)
{
	static bool reserved = false;
	if (!reserved)
	{
		reserve_limit();
		reserved = true;
	}

	if (!see_for_overflow(var))
	{
		return;
	}

	auto [iter, did_insert] = m_commands.insert(var);

	assert(did_insert && "Failed to insert a command because it's already in the list.. duplicated?");
}

void StaticCommandContainer::reserve_limit()
{
	m_commands.reserve(k_max_commands_per_module);

	CConsole::the().info("Reserved {} slots for variables for current modules.", k_max_commands_per_module);
}

bool StaticCommandContainer::see_for_overflow(BaseCommand* current_var)
{
	if (m_commands.size() >= k_max_commands_per_module)
	{
		CConsole::the().error("Command {} couldn't be added, because the limit per this module has already been reached. ({})", 
							  current_var->get_name(), k_max_commands_per_module);
		return false;
	}

	return true;
}

