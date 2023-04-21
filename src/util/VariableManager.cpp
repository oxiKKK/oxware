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

BaseCommand command_list(
	"command_list",
	[&]()
	{
		int num = 0;
		g_variablemgr_i->for_each_command(
			[&](BaseCommand* cmd)
			{
				CConsole::the().info("{:<3}: {}", ++num, cmd->get_name());
			});

		CConsole::the().info("------------------------------");
		CConsole::the().info("There is total of {} commands.", num);
	}
);

BaseCommand variable_list(
	"variable_list",
	[&]()
	{
		CConsole::the().info("{:<3}: {:<32} {:<10} {:<10} {:<10}", "id", "name", "value", "min", "max");
		int num = 0;
		g_variablemgr_i->for_each_variable(
			[&](BaseVariable* var)
			{
				bool bounds = var->has_bounds();
				CConsole::the().info("{:<3}: {:<32} {:<10} {:<10} {:<10}", 
									 ++num, var->get_name(), var->get_value_string(), bounds ? var->get_min_string() : " ", bounds ? var->get_max_string() : " ");
			});

		CConsole::the().info("------------------------------");
		CConsole::the().info("There is total of {} variables.", num);
	}
);

BaseCommand help(
	"help",
	[&]()
	{
		CConsole::the().info("Help:");
		CConsole::the().info("To see the list of available commands, type \"variable_list\" and press enter.");
		CConsole::the().info("");
		CConsole::the().info("You can also execute in-game commands such as, e.g. \"fps_max\" etc.");
		CConsole::the().info("Just type \"hl <command you want>\". So for example \"hl fps_max\"");
	}
);

IVariableManager* g_variablemgr_i = nullptr;

class CVariableManager : public IVariableManager
{
public:
	CVariableManager();
	~CVariableManager();
	
	bool initialize();

	// variable and command lookups
	BaseVariable* query_variable(const char* name);
	BaseCommand* query_command(const char* name);

	void register_variables_and_commands_per_module(StaticVariableContainer* variable_container, StaticCommandContainer* command_container, const char* module_name);
	void register_single_variable(BaseVariable* var);
	void register_single_command(BaseCommand* var);

	void for_each_variable(const std::function<void(BaseVariable*)>& callback);
	void for_each_command(const std::function<void(BaseCommand*)>& callback);

private:
	std::unordered_set<BaseVariable*> m_registered_variables;
	std::unordered_set<BaseCommand*> m_registered_commands;

	bool did_overflow_vars(BaseVariable* var);
	bool did_overflow_cmds(BaseCommand* cmd);
};

CVariableManager g_variablemgr;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CVariableManager, IVariableManager, IVARIABLEMANAGER_INTERFACEID, g_variablemgr);

//-------------------------------------------------------------------------------
//
// CVariableManager implementation
//
//-------------------------------------------------------------------------------

CVariableManager::CVariableManager()
{
	g_variablemgr_i = this;
}

CVariableManager::~CVariableManager()
{
	g_variablemgr_i = nullptr;
}

bool CVariableManager::initialize()
{
	register_variables_and_commands_per_module(&g_static_variable_container, &g_static_command_container, MODULE_UTIL);

	CConsole::the().info("VariableManager initialized.");
	CConsole::the().info("There are {} variables registered at initialization.", m_registered_variables.size());
	CConsole::the().info("There are {} commands registered at initialization.", m_registered_commands.size());

	return true;
}

BaseVariable* CVariableManager::query_variable(const char* name)
{
	for (BaseVariable* var : m_registered_variables)
	{
		if (!strcmp(var->get_name(), name))
			return reinterpret_cast<BaseVariable*>(var);
	}

	return nullptr;
}

BaseCommand* CVariableManager::query_command(const char* name)
{
	for (BaseCommand* cmd : m_registered_commands)
	{
		if (!strcmp(cmd->get_name(), name))
			return reinterpret_cast<BaseCommand*>(cmd);
	}

	return nullptr;
}

void CVariableManager::register_variables_and_commands_per_module(StaticVariableContainer* variable_container, StaticCommandContainer* command_container, const char* module_name)
{
	// variables
	{
		auto list = variable_container->get_variable_list();

		if (list.empty())
		{
			CConsole::the().info("Module {} has no variables to be registered.", module_name);
			return;
		}

		CConsole::the().info("Registering {} variables from module {}...", list.size(), module_name);

		// Merge vars from other modules into one continuous list.
		for (BaseVariable* var : list)
		{
			register_single_variable(var);
		}
	}

	// commands
	{
		auto list = command_container->get_command_list();

		if (list.empty())
		{
			CConsole::the().info("Module {} has no commands to be registered.", module_name);
			return;
		}

		CConsole::the().info("Registering {} commands from module {}...", list.size(), module_name);

		// Merge cmds from other modules into one continuous list.
		for (BaseCommand* cmd : list)
		{
			register_single_command(cmd);
		}
	}
}

void CVariableManager::register_single_variable(BaseVariable* var)
{
	if (did_overflow_vars(var))
	{
		return;
	}

	auto [iter, did_insert] = m_registered_variables.insert(var);

	// same message is already inside BaseVariable.cpp, hence the "(2)"
	assert(did_insert && "Failed to insert a variable because it's already in the list.. duplicated? (2)");

	CConsole::the().info("Registered variable '{}'.", var->get_name());
}

void CVariableManager::register_single_command(BaseCommand* cmd)
{
	if (did_overflow_cmds(cmd))
	{
		return;
	}

	auto [iter, did_insert] = m_registered_commands.insert(cmd);

	// same message is already inside BaseVariable.cpp, hence the "(2)"
	assert(did_insert && "Failed to insert a command because it's already in the list.. duplicated? (2)"); 

	CConsole::the().info("Registered command '{}'.", cmd->get_name());
}

void CVariableManager::for_each_variable(const std::function<void(BaseVariable*)>& callback)
{
	for (BaseVariable* var : m_registered_variables)
	{
		callback(var);
	}
}

void CVariableManager::for_each_command(const std::function<void(BaseCommand*)>& callback)
{
	for (BaseCommand* cmd : m_registered_commands)
	{
		callback(cmd);
	}
}

bool CVariableManager::did_overflow_vars(BaseVariable* var)
{
	if (m_registered_variables.size() >= k_max_variables_absolute)
	{
		CConsole::the().error("Variable {} couldn't be added, because the absolute limit has already been reached. ({})", 
							  var->get_name(), k_max_variables_absolute);
		return true;
	}

	return false;
}

bool CVariableManager::did_overflow_cmds(BaseCommand* cmd)
{
	if (m_registered_variables.size() >= k_max_commands_absolute)
	{
		CConsole::the().error("Command {} couldn't be added, because the absolute limit has already been reached. ({})", 
							  cmd->get_name(), k_max_commands_absolute);
		return true;
	}

	return false;
}
