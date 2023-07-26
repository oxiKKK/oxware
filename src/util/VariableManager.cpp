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
	"command_list", "Prints out all registered commands",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		CConsole::the().info("{:<3}: {:<24} {}", "id", "name", "parameters");
		int num = 0;
		g_variablemgr_i->for_each_command(
			[&](BaseCommand* cmd)
			{
				CConsole::the().info("{:<3}: {:<24} {}", ++num, cmd->get_name(), cmd->has_usage() ? cmd->get_usage() : " ");
			});

		CConsole::the().info("------------------------------");
		CConsole::the().info("There is total of {} commands.", num);
	}
);

BaseCommand variable_list(
	"variable_list", "Prints out all registered variables",
	[&](BaseCommand* cmd, const CmdArgs& args)
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
	"help", "Prints out some information that may help you",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		CConsole::the().info("Help:");
		CConsole::the().info("To see the list of available commands, type \"variable_list\" and press enter.");
		CConsole::the().info("");
		CConsole::the().info("You can also execute in-game commands such as, e.g. \"fps_max\" etc.");
		CConsole::the().info("Just type \"hl <command you want>\". So for example \"hl fps_max\"");
	}
);

#if 0 // tokenizer test
BaseCommand tokenize(
	"tokenize", "<token1> <token2> <token3>", "Debugging command to test the command tokenizer",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		if (args.count() == 1 || args.count() > 3)
		{
			cmd->print_usage();
			return;
		}

		{
			std::string token = args.get(1);
			if (!token.empty())
			{
				CConsole::the().info("1st token: '{}'", token);
			}
		}
		{
			std::string token = args.get(2);
			if (!token.empty())
			{
				CConsole::the().info("2nd token: '{}'", token);
			}
		}
		{
			std::string token = args.get(3);
			if (!token.empty())
			{
				CConsole::the().info("3rd token: '{}'", token);
			}
		}
	}
);
#endif

IVariableManager* g_variablemgr_i = nullptr;

class CVariableManager : public IVariableManager, IConfigIOOperations
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

	void execute_command(const std::string& command_sequence, bool silent = false);

	void provide_hl_execute_cmd_pfn(m_hl_execute_cmd_pfn_t pfn)
	{
		m_hl_execute_cmd_pfn = pfn;
	}

private:
	std::set<BaseVariable*, BaseVariable::less> m_registered_variables;
	std::set<BaseCommand*, BaseCommand::less> m_registered_commands;

	bool did_overflow_vars(BaseVariable* var);
	bool did_overflow_cmds(BaseCommand* cmd);

	void tokenize_command(const std::string& command_buffer);
	std::vector<std::string> m_current_cmd_tokens;

	void execute_internal(const std::string& command, bool silent);
	void execute_halflife_command(const std::string& full_hl_command, bool silent);

	int execute_hl_command(const char* cmd)
	{
		if (m_hl_execute_cmd_pfn)
		{
			return m_hl_execute_cmd_pfn(cmd);
		}
		else
		{
			return 0;
		}
	}

	m_hl_execute_cmd_pfn_t m_hl_execute_cmd_pfn;

	// config I/O provider
	void provide_cfg_load_export_callbacks();
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

	provide_cfg_load_export_callbacks();

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
		auto& list = variable_container->get_variable_list();

		if (list.empty())
		{
			CConsole::the().info("Module {} has no variables to be registered.", module_name);
		}
		else
		{
			CConsole::the().info("Registering {} variables from module {}...", list.size(), module_name);

			// Merge vars from other modules into one continuous list.
			for (BaseVariable* var : list)
			{
				register_single_variable(var);
			}
		}
	}

	// commands
	{
		auto& list = command_container->get_command_list();

		if (list.empty())
		{
			CConsole::the().info("Module {} has no commands to be registered.", module_name);
		}
		else
		{
			CConsole::the().info("Registering {} commands from module {}...", list.size(), module_name);

			// Merge cmds from other modules into one continuous list.
			for (BaseCommand* cmd : list)
			{
				register_single_command(cmd);
			}
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

void CVariableManager::execute_command(const std::string& command_sequence, bool silent)
{
	if (command_sequence == " ")
	{
		return;
	}

	// collect individual commands separated by ';'
	std::string current_command;
	bool in_quotes = false;
	for (size_t i = 0; i < command_sequence.length(); i++)
	{
		const char c = command_sequence[i];

		bool last = (i == command_sequence.length() - 1);

		if (c == '"' && !in_quotes)
		{
			in_quotes = true;
		}
		else if (in_quotes && c == '"')
		{
			if (!last)
			{
				in_quotes = false;
			}
		}

		if ((c == ';' && !in_quotes) || last) // the last case, too
		{
			if (last)
			{
				current_command.push_back(c);
			}

			// trim leading spaces, if any
			current_command = CStringTools::the().ltrim(current_command);

			// execute the command in place
			execute_internal(current_command, silent);
			
			current_command.clear();
		}
		else
		{
			current_command.push_back(c);
		}
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

void CVariableManager::execute_internal(const std::string& command, bool silent)
{
	if (command.substr(0, 3) == "hl ")
	{
		execute_halflife_command(command.substr(3), silent); // pass in full command
		return;
	}

	tokenize_command(command);

	if (m_current_cmd_tokens.empty())
	{
		return;
	}

	auto& cmd_name = m_current_cmd_tokens.front();
	auto cmd = query_command(cmd_name.c_str());
	if (!cmd)
	{
		// try variable
		auto var = query_variable(cmd_name.c_str());
		if (!var)
		{
			if (!silent)
			{
				CConsole::the().info("> Unknown command '{}'", command);
			}
			return;
		}
		else
		{
			if (m_current_cmd_tokens.size() == 1)
			{
				if (!silent)
				{
					CConsole::the().info("> '{}' is '{}'", cmd_name, var->get_value_string());
				}
			}
			else
			{
				std::string value_raw = command.substr(cmd_name.length() + 1);

				// erase quotes, they're useless here.
				value_raw.erase(std::remove(value_raw.begin(), value_raw.end(), '"'), value_raw.end());
				bool ok;
				var->set_from_string(value_raw, ok);

				if (!silent)
				{
					if (ok)
					{
						CConsole::the().info("> Set '{}' to '{}'", cmd_name, value_raw);
					}
					else
					{
						CConsole::the().error("> Invalid variable value: '{}'", value_raw);
					}
				}
			}
		}
	}
	else
	{
		if (!silent )
		{
			CConsole::the().info("> {}", command);
		}

		cmd->execute(cmd, m_current_cmd_tokens);
	}
}

void CVariableManager::execute_halflife_command(const std::string& full_hl_command, bool silent)
{
	const char* s = full_hl_command.c_str();
	// hl command
	if (!execute_hl_command(s))
	{
		if (!silent)
		{
			CConsole::the().info("> Bad HL command: {}", full_hl_command);
		}
	}
	else
	{
		if (!silent)
		{
			CConsole::the().info("> HL command: {}", full_hl_command);
		}
	}
}

void CVariableManager::provide_cfg_load_export_callbacks()
{
	auto cheat_settings = g_config_mgr_i->query_config_file_type("cheat_settings");
	if (!cheat_settings)
	{
		return;
	}

	//
	// provide config load function.
	//

	auto load_fn = [](nh::json& json)
	{
		g_variablemgr_i->for_each_variable(
			[&](BaseVariable* var)
			{
				try
				{
					var->set_from_string(json[var->get_current_module_name()][var->get_name()].get<std::string>());
				}
				catch (const nh::json::exception& e)
				{
					CConsole::the().error("JSON error: {}", e.what());
				}
			});
	};

	(*cheat_settings)->provide_load_fn(load_fn);

	//
	// provide config export function.
	//

	auto export_fn = [](nh::json& json)
	{
		g_variablemgr_i->for_each_variable(
			[&](BaseVariable* var)
			{
				try
				{
					json[var->get_current_module_name()][var->get_name()] = var->get_value_string();
				}
				catch (const nh::json::exception& e)
				{
					CConsole::the().error("JSON error: {}", e.what());
				}
			});
	};

	(*cheat_settings)->provide_export_fn(export_fn);
}

//--------------------------------------------------------------------------------------------------------------------
// COMMAND PARSING
// 

void CVariableManager::tokenize_command(const std::string& command_buffer)
{
	assert(!command_buffer.empty() && "passed empty command buffer to " __FUNCTION__ ". This shouldn't happen.");

	m_current_cmd_tokens.clear();

	std::string current_token;
	bool in_quotes = false; // ignore spaces and such
	for (size_t i = 0; i < command_buffer.length(); i++)
	{
		const char c = command_buffer[i];

		bool last = (i == command_buffer.length() - 1);

		if (c == '"' && !in_quotes)
		{
			in_quotes = true;
			continue;
		}
		else if (in_quotes && c == '"')
		{
			if (!last)
			{
				in_quotes = false;
				continue; // look for ending quote
			}
		}

		if ((c == ' ' && !in_quotes) || last) // the last case, too
		{
			if (last && !in_quotes)
			{
				current_token.push_back(c);
			}

			m_current_cmd_tokens.push_back(current_token);
			current_token.clear();
		}
		else
		{
			current_token.push_back(c);
		}
	}

#if 0
	CConsole::the().info("Tokenized last command:");
	for (const auto& token : m_cmd_tokens)
	{
		CConsole::the().info("{}", token);
	}
#endif
}
