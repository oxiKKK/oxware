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

IInCommands* g_in_commands_i = nullptr;

class CInCommands : public IInCommands, IConfigIOOperations
{
public:
	CInCommands();
	~CInCommands();

	void initialize();

	void update_activation_conditions();

	void add_command(BaseInCommand* in_cmd);

	void register_incommands_per_module(StaticInCommandContainer* incommand_container, const char* module_name);

	void create_incommands_from_json(const nh::json& json);
	void export_incommands_to_json(nh::json& json);

	void for_each_incommand(const std::function<void(BaseInCommand* in_cmd)>& callback);

	BaseInCommand* get_incommand(const std::string& id);

	bool is_key_bound_and_active(int vk);

	bool should_block_engine_key(int vk);

	bool does_meet_activation_conditions(EActivationCondition act_cond);

private:
	// config I/O provider
	void provide_cfg_load_export_callbacks();

	EInCommandFlags parse_flags_out_of_string(const std::string& flags_str);
	std::string create_string_out_of_flags(EInCommandFlags flags);

private:
	std::unordered_map<std::string, BaseInCommand*> m_in_commands;

	EActivationCondition m_activation_conditions_for_this_frame = IN_ACTCOND_None;
};

CInCommands g_in_commands;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CInCommands, IInCommands, IINCOMMANDS_INTERFACEID, g_in_commands);

//-------------------------------------------------------------------------------
//
// CInCommands implementation
//
//-------------------------------------------------------------------------------

CInCommands::CInCommands()
{
	g_in_commands_i = this;
}

CInCommands::~CInCommands()
{
	g_in_commands_i = nullptr;
}

void CInCommands::initialize()
{
	register_incommands_per_module(&g_static_incommand_container, MODULE_CHEAT);

	for (const auto& [name, in_cmd] : m_in_commands)
	{
		in_cmd->register_cmd();
	}

	CConsole::the().info("Registered {} InCommands.", m_in_commands.size());

	provide_cfg_load_export_callbacks();
}

void CInCommands::update_activation_conditions()
{
	m_activation_conditions_for_this_frame = IN_ACTCOND_None;

	if (CGameUtil::the().is_fully_connected())
	{
		m_activation_conditions_for_this_frame |= IN_ACTCOND_Connected;

		auto local = CLocalState::the().local_player();
		if (local)
		{
			m_activation_conditions_for_this_frame |= IN_ACTCOND_Alive;
		}
	}

	if (!CLocalState::the().is_in_messagemode())
	{
		m_activation_conditions_for_this_frame |= IN_ACTCOND_NotInMessageMode;
	}
}

void CInCommands::add_command(BaseInCommand* in_cmd)
{
	m_in_commands.insert({ in_cmd->get_name(), in_cmd });

	CConsole::the().info("Added InCommand: '{}'.", in_cmd->get_name());
}

void CInCommands::register_incommands_per_module(StaticInCommandContainer* incommand_container, const char* module_name)
{
	auto& list = incommand_container->get_incommand_list();

	if (list.empty())
	{
		CConsole::the().info("Module {} has no incommands to be registered.", module_name);
		return;
	}

	CConsole::the().info("Registering {} incommands from module {}...", list.size(), module_name);

	// Merge vars from other modules into one continuous list.
	for (BaseInCommand* var : list)
	{
		add_command(var);
	}
}

void CInCommands::create_incommands_from_json(const nh::json& json)
{
	const nh::json* in_commands = nullptr;

	try
	{
		in_commands = &json.at("in_commands");
	}
	catch (...)
	{
		// there aren't any incommands
		return;
	}

	/*
		"in_commands": {
			"movement_bhop": {
				"key":		"space"
				"flags":	"none, ..."
			}, 
			...
		}
	*/

	for (auto& [incmd_name, in_cmd] : in_commands->items())
	{
		std::string key_str, flags_str;

		auto try_to_parse = [&](const std::string& what, std::string& output, bool doesnt_matter = false) {
			try
			{
				auto& key = in_cmd.at(what);
				output = key.get<std::string>();
				return true;
			}
			catch (...)
			{
				if (!doesnt_matter)
				{
					CConsole::the().error("Error parsing '{}' field for key '{}'", what, incmd_name);
				}
				output = "";
				return false;
			}
		};

		// key bound
		if (!try_to_parse("key", key_str))
		{
			continue;
		}

		// flags
		if (!try_to_parse("flags", flags_str, true))
		{
			// doesn't have to return, this is optional
		}

		auto in_cmd = get_incommand(incmd_name);
		if (!in_cmd)
		{
			continue;
		}

		int vk = NULL;
		if (key_str != "unbound")
		{
			vk = g_user_input_i->string_to_virtual_key(key_str);
			if (vk != NULL)
			{
				in_cmd->rebind_key_to(vk);
			}
		}

		// parse flags
		EInCommandFlags flags = parse_flags_out_of_string(flags_str);

		if (flags != IN_FLAG_None)
		{
			in_cmd->set_flags(flags);
		}
	}
}

void CInCommands::export_incommands_to_json(nh::json& json)
{
	for (const auto& [name, in_cmd] : m_in_commands)
	{
		int vk = in_cmd->get_key_bound();
		if (vk == NULL) // unbound
		{
			json["in_commands"][name]["key"] = "unbound";
			continue;
		}

		json["in_commands"][name]["key"] = g_user_input_i->virtual_key_to_string(vk);

		std::string flags_str = create_string_out_of_flags(in_cmd->get_flags());

		json["in_commands"][name]["flags"] = flags_str;
	}
}

void CInCommands::for_each_incommand(const std::function<void(BaseInCommand*in_cmd)>& callback)
{
	for (const auto& [name, in_cmd] : m_in_commands)
	{
		callback(in_cmd);
	}
}

BaseInCommand* CInCommands::get_incommand(const std::string& id)
{
	try
	{
		return m_in_commands.at(id);
	}
	catch (...)
	{
		CConsole::the().error("Unknown InCommand: '{}'", id);
		return nullptr;
	}
}

bool CInCommands::is_key_bound_and_active(int vk)
{
	for (const auto& [name, in_cmd] : m_in_commands)
	{
		bool will_exec_incmd =
			in_cmd->is_active() &&
			does_meet_activation_conditions(in_cmd->get_activation_condition());
		
		if (in_cmd->get_key_bound() == vk && will_exec_incmd)
		{
			return true;
		}
	}

	return false;
}

bool CInCommands::should_block_engine_key(int vk)
{
	for (const auto& [name, in_cmd] : m_in_commands)
	{
		bool will_exec_incmd =
			in_cmd->is_active() &&
			does_meet_activation_conditions(in_cmd->get_activation_condition());

		if (in_cmd->get_key_bound() == vk && will_exec_incmd)
		{
			return in_cmd->get_flags() & IN_FLAG_DisableInGameKey;
		}
	}

	return false;
}

void CInCommands::provide_cfg_load_export_callbacks()
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
		g_in_commands_i->create_incommands_from_json(json);
	};

	(*cheat_settings)->provide_load_fn(load_fn);

	//
	// provide config export function.
	//

	auto export_fn = [](nh::json& json)
	{
		g_in_commands_i->export_incommands_to_json(json);
	};

	(*cheat_settings)->provide_export_fn(export_fn);
}

EInCommandFlags CInCommands::parse_flags_out_of_string(const std::string & flags_str)
{
	EInCommandFlags flags = IN_FLAG_None;
	std::string current_token;
	for (size_t i = 0; i < flags_str.length(); i++)
	{
		char c = flags_str[i];
		bool last = (i == flags_str.length() - 1);
		if (c == ',' || last)
		{
			if (last)
			{
				current_token.push_back(c);
			}

			// trim space
			current_token.erase(current_token.find_last_not_of(' ') + 1); // suffixing spaces
			current_token.erase(0, current_token.find_first_not_of(' ')); // prefixing spaces

			if (current_token == incommand_flags_to_str[IN_FLAG_DisableInGameKey])
			{
				flags |= IN_FLAG_DisableInGameKey;
			}

			current_token.clear(); // throw out after used

		}
		else
		{
			current_token.push_back(c);
		}
	}
	return flags;
}

std::string CInCommands::create_string_out_of_flags(EInCommandFlags flags)
{
	// we have a custom formatter for flags, so use that
	return std::format("{}", flags);
}

bool CInCommands::does_meet_activation_conditions(EActivationCondition act_cond)
{
	if (act_cond == IN_ACTCOND_None)
	{
		return true;
	}

	return (m_activation_conditions_for_this_frame & act_cond) == act_cond;
}
