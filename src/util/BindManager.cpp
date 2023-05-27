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

IBindManager* g_bindmgr_i = nullptr;

BaseCommand _bind(
	"bind", "<key> <command> <flags>", "Binds command to a key that is executed only once after the key was pressed.",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		if (args.count() == 1 || args.count() > 4)
		{
			cmd->print_usage();
			return;
		}

		if (args.count() == 2)
		{
			int vk = g_user_input_i->string_to_virtual_key(args.get(1));
			if (vk == NULL)
			{
				return;
			}
			auto bind = g_bindmgr_i->get_bind(vk);
			if (bind)
			{
				if (bind->type == BIND_Toggle)
				{
					CConsole::the().info("'{}' is bound to '{}' and '{}'", args.get(1), bind->cmd_sequence_0, bind->cmd_sequence_1);
				}
				else
				{
					CConsole::the().info("'{}' is bound to '{}'", args.get(1), bind->cmd_sequence_0);
				}
			}
		}
		else
		{
			std::string cmd = args.get(2);

			EBindFlags f = BINDFLAG_None;

			if (args.count() > 4)
			{
				f = g_bindmgr_i->parse_flags_out_of_string(args.get(4));
			}

			bool is_action_cmd = cmd[0] == '+';

			if (!is_action_cmd)
			{
				g_bindmgr_i->add_bind(args.get(1), cmd, f);
			}
			else
			{
				std::string cmd1 = '-' + cmd.substr(1);

				g_bindmgr_i->add_bind_toggle(args.get(1), cmd, cmd1, f);
			}
		}
	}
);

BaseCommand bind_toggle(
	"bind_toggle", "<key> <key down command> <key release command> <flags>", "Binds two commands to a key, each executing at push down and push up of the key.",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		if (args.count() == 1 || args.count() > 5)
		{
			cmd->print_usage();
			return;
		}
		
		if (args.count() == 2)
		{
			int vk = g_user_input_i->string_to_virtual_key(args.get(1));
			if (vk == NULL)
			{
				return;
			}
			auto bind = g_bindmgr_i->get_bind(vk);
			if (bind)
			{
				if (bind->type == BIND_Toggle)
				{
					CConsole::the().info("'{}' is bound to '{}' and '{}'", args.get(1), bind->cmd_sequence_0, bind->cmd_sequence_1);
				}
				else
				{
					CConsole::the().info("'{}' is bound to '{}'", args.get(1), bind->cmd_sequence_0);
				}
			}
		}
		else
		{
			EBindFlags f = BINDFLAG_None;

			if (args.count() > 4)
			{
				f = g_bindmgr_i->parse_flags_out_of_string(args.get(4));
			}

			g_bindmgr_i->add_bind_toggle(args.get(1), args.get(2), args.get(3), f);
		}
	}
);

BaseCommand list_binds(
	"list_binds", "Prints all currently bound binds",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		CConsole::the().info("{:<3}: {:<16} {:<32} {:<32} {:<16} {}", "id", "key", "command", "command 1", "type", "flags");
		int num = 0;
		g_bindmgr_i->for_each_bind(
			[&](int vk, const bind_t& bind)
			{
				num++;
				CConsole::the().info("{:<3}: {:<16} {:<32} {:<32} {:<16} {}", num, g_user_input_i->virtual_key_to_string(vk), 
									 bind.cmd_sequence_0, bind.cmd_sequence_1, bind.type, bind.flags);
			});

		CConsole::the().info("--------------------------------------------------------------------------------------------------------------------------------");
		CConsole::the().info("There is total of {} binds.", num);
	}
);

BaseCommand unbind(
	"unbind", "<key>", "Unbinds a key",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		if (args.count() != 2)
		{
			cmd->print_usage();
			return;
		}
		
		g_bindmgr_i->remove_bind(args.get(1));
	}
);

class CBindManager : public IBindManager
{
public:
	CBindManager();
	~CBindManager();

	void initialize();
	void shutdown();

	void create_binds_from_json(const nlohmann::json& json);
	void export_binds_to_json(nlohmann::json& json);

	void add_bind(int virtual_key, const std::string& command_sequence, EBindFlags flags, bool silent);
	void add_bind_toggle(int virtual_key, const std::string& key_down_command_sequence, const std::string& key_up_command_sequence, EBindFlags flags, bool silent);

	void add_bind(const std::string& key_name, const std::string& command_sequence, EBindFlags flags, bool silent);
	void add_bind_toggle(const std::string& key_name, const std::string& key_down_command_sequence, const std::string& key_up_command_sequence, EBindFlags flags, bool silent);

	void remove_bind(const std::string& key_name, bool silen);
	void remove_bind(int virtual_key, bool silen);

	void remove_all_binds();

	bind_t* get_bind(int virtual_key);
	bind_t* get_bind(const std::string& key_name);

	void for_each_bind(const std::function<void(int, const bind_t&)>& callback);

	bool is_key_bindable(int vk);

	EBindFlags parse_flags_out_of_string(const std::string& flags_str);
	std::string create_string_out_of_flags(EBindFlags flags);

	void set_ui_running(bool is_running) { m_is_ui_running = is_running; }
	bool is_ui_running() { return m_is_ui_running; }

	bool is_key_bound(int vk);

private:
	void register_keypress_event_on_push(UserKey_t& key, int virtual_key, bind_t& new_bind);
	void register_keypress_event_toggle(UserKey_t& key, int virtual_key, bind_t& new_bind);

	void register_bind_internal(int virtual_key, UserKey_t& key, const std::string& command_seq, 
								const std::string& command_seq1, EBindType type, EBindFlags flags, bool silent);

private:
	// virtual key as key :) and command sequence as value
	std::map<int, bind_t> m_registerd_binds;

	bool m_is_ui_running = false;
};

CBindManager g_bindmgr;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CBindManager, IBindManager, IBINDMANAGER_INTERFACEID, g_bindmgr);

//-------------------------------------------------------------------------------
//
// CBindManager implementation
//
//-------------------------------------------------------------------------------

CBindManager::CBindManager()
{
	g_bindmgr_i = this;
}

CBindManager::~CBindManager()
{
	g_bindmgr_i = nullptr;
}

void CBindManager::initialize()
{
	CConsole::the().info("Bind Manager initialized.");
}

void CBindManager::shutdown()
{

}

void CBindManager::create_binds_from_json(const nlohmann::json& json)
{
	const nlohmann::json* binds = nullptr;

	try
	{
		binds = &json.at("binds");
	}
	catch (...)
	{
		// there aren't any binds
		return;
	}

	/*
		"binds": {
			"f4": {
				"command":	"ui_toggle_menu",
				"type":		"toggle",
				"flags":	"execute_over_ui, ..."
			}
		}
	*/

	for (auto& [key, bound_key] : binds->items())
	{
		std::string cmd_str, cmd1_str, type_str, flags_str;

		auto try_to_parse = [&](const std::string& what, std::string& output, bool doesnt_matter = false) {
			try
			{
				auto& key = bound_key.at(what);
				output = key.get<std::string>();
				return true;
			}
			catch (...)
			{
				if (!doesnt_matter)
				{
					CConsole::the().error("Error parsing '{}' field for key '{}'", what, key);
				}
				output = "";
				return false;
			}
		};

		// commnad
		if (!try_to_parse("command", cmd_str))
		{
			continue;
		}

		// type
		if (!try_to_parse("type", type_str))
		{
			continue;
		}

		// flags
		if (!try_to_parse("flags", flags_str, true))
		{
			// doesn't have to return, this is optional
		}

		// bind type
		EBindType type;
		if (type_str == bind_type_to_str[BIND_OnPush])
		{
			type = BIND_OnPush;
		}
		else if (type_str == bind_type_to_str[BIND_Toggle])
		{
			type = BIND_Toggle;

			if (!try_to_parse("command1", cmd1_str))
			{
				CConsole::the().error("You have to provide second command, too, when binding repeateable command");
				continue;
			}
		}
		else
		{
			if (type_str.empty())
			{
				CConsole::the().error("Bound key '{}' without any bind type! A key must be provided a bind type when bound!", key);
			}
			else
			{
				CConsole::the().error("Unrecognized bind type '{}' for key '{}'", key, type_str);
			}
			continue;
		}

		// bind flags
		EBindFlags flags = parse_flags_out_of_string(flags_str);

		switch (type)
		{
			case BIND_OnPush:
			{
				add_bind(key, cmd_str, flags, false);
				break;
			}
			case BIND_Toggle:
			{
				add_bind_toggle(key, cmd_str, cmd1_str, flags, false);
				break;
			}
		}
	}
}

void CBindManager::export_binds_to_json(nlohmann::json& json)
{
	for (const auto& [vk, bind] : m_registerd_binds)
	{
		auto key_name = g_user_input_i->virtual_key_to_string(vk);
		if (key_name.empty())
		{
			continue;
		}

		/*
			"binds": {
				"f4": {
					"command":	"ui_toggle_menu",
					"type":		"toggle",
					"flags":	"execute_over_ui, ..."
				}
			}
		*/

		auto write_json_bind = [&](const std::string name, const std::string value)
		{
			try
			{
				json["binds"][key_name][name] = value;
			}
			catch (const nlohmann::json::exception& e)
			{
				CConsole::the().error("JSON error while trying to write key '{}' and value '{}': {}", name, value, e.what());
			}
		};

		try
		{
			write_json_bind("command", bind.cmd_sequence_0);

			std::string type_str = bind_type_to_str[bind.type];

			write_json_bind("type", type_str);

			if (bind.type == BIND_Toggle)
			{
				write_json_bind("command1", bind.cmd_sequence_1);
			}

			std::string flags_str = create_string_out_of_flags(bind.flags);
			
			json["binds"][key_name]["flags"] = flags_str;
		}
		catch (const nlohmann::json::exception& e)
		{
			CConsole::the().error("JSON error: {}", e.what());
		}
	}
}

void CBindManager::add_bind(int virtual_key, const std::string& command_sequence, EBindFlags flags, bool silent)
{	
	if (!g_user_input_i->is_valid_key(virtual_key))
	{
		CConsole::the().error("Tried to bind invalid key: {}", virtual_key);
		return;
	}

	bool found;
	auto& key = g_user_input_i->get_key(virtual_key, &found);
	if (!found)
	{
		CConsole::the().info("Failed to bind command to a key '{}'. The key was not found", virtual_key);
		return;
	}

	std::string key_name = g_user_input_i->virtual_key_to_string(virtual_key);
	if (!is_key_bindable(virtual_key))
	{
		CConsole::the().error("Key '{}' isn't bindable", key_name);
		return;
	}

	register_bind_internal(virtual_key, key, command_sequence, "", BIND_OnPush, flags, silent);
}

void CBindManager::add_bind_toggle(int virtual_key, const std::string& key_down_command_sequence, const std::string& key_up_command_sequence, EBindFlags flags, bool silent)
{
	if (!g_user_input_i->is_valid_key(virtual_key))
	{
		CConsole::the().error("Tried to bind invalid key: {}", virtual_key);
		return;
	}

	bool found;
	auto& key = g_user_input_i->get_key(virtual_key, &found);
	if (!found)
	{
		CConsole::the().info("Failed to bind command to a key '{}'. The key was not found", virtual_key);
		return;
	}

	std::string key_name = g_user_input_i->virtual_key_to_string(virtual_key);
	if (!is_key_bindable(virtual_key))
	{
		CConsole::the().error("Key '{}' isn't bindable", key_name);
		return;
	}

	register_bind_internal(virtual_key, key, key_down_command_sequence, key_up_command_sequence, BIND_Toggle, flags, silent);
}

void CBindManager::remove_bind(int virtual_key, bool silent)
{
	if (!g_user_input_i->is_valid_key(virtual_key))
	{
		CConsole::the().error("Tried to unbind invalid key: {}", virtual_key);
		return;
	}

	// if already bound, the key must exist.. or does it..? :D
	auto& key = g_user_input_i->get_key(virtual_key);

	std::string key_name = g_user_input_i->virtual_key_to_string(virtual_key);

	auto bind = get_bind(virtual_key);
	if (!bind)
	{
		return;
	}

	switch (bind->type)
	{
		case BIND_OnPush:
		{
			key.remove_on_pressed_callback(std::format("vk_{}", virtual_key));
			break;
		}
		case BIND_Toggle:
		{
			key.remove_on_pressed_callback(std::format("vk_{}", virtual_key));
			key.remove_on_unpressed_callback(std::format("vk_{}", virtual_key));
			break;
		}
		default:
		{
			assert(0 && "Cannot unbind bind with invalid bind type!");
			break;
		}
	}

	try
	{
		m_registerd_binds.erase(virtual_key);
	}
	catch (...)
	{
		CConsole::the().error("Can't unbind. Key '{}' not bound.", key_name);
		return;
	}

	if (!silent)
	{
		CConsole::the().info("Unbound key '{}'.", key_name);
	}
}

void CBindManager::add_bind(const std::string& key_name, const std::string& command_sequence, EBindFlags flags, bool silent)
{
	int vk = g_user_input_i->string_to_virtual_key(key_name);
	if (vk == NULL)
	{
		return;
	}

	add_bind(vk, command_sequence, flags, silent);
}

void CBindManager::add_bind_toggle(const std::string& key_name, const std::string& key_down_command_sequence, const std::string& key_up_command_sequence, EBindFlags flags, bool silent)
{
	int vk = g_user_input_i->string_to_virtual_key(key_name);
	if (vk == NULL)
	{
		return;
	}

	add_bind_toggle(vk, key_down_command_sequence, key_up_command_sequence, flags, silent);
}

void CBindManager::remove_bind(const std::string& key_name, bool silent)
{
	int vk = g_user_input_i->string_to_virtual_key(key_name);
	if (vk == NULL)
	{
		return;
	}

	remove_bind(vk, silent);
}

void CBindManager::remove_all_binds()
{
	// we cannot do this at once, otherwise we would crash at the interation
	std::vector<int> keys_to_be_removed;
	for (auto& [vk, cmd] : m_registerd_binds)
	{
		keys_to_be_removed.push_back(vk);
	}

	for (int vk : keys_to_be_removed)
	{
		remove_bind(vk, false);
	}
}

bind_t* CBindManager::get_bind(int virtual_key)
{
	try
	{
		return &m_registerd_binds.at(virtual_key);
	}
	catch (...)
	{
		std::string key_name = g_user_input_i->virtual_key_to_string(virtual_key);
		CConsole::the().error("Virtual key '{}' not bound!", key_name);
		return nullptr;
	}
}

bind_t* CBindManager::get_bind(const std::string& key_name)
{
	int vk = g_user_input_i->string_to_virtual_key(key_name);
	if (vk == NULL)
	{
		return nullptr;
	}

	return get_bind(vk);
}

void CBindManager::for_each_bind(const std::function<void(int, const bind_t&)>& callback)
{
	for (const auto [vk, bind] : m_registerd_binds)
	{
		callback(vk, bind);
	}
}

bool CBindManager::is_key_bindable(int vk)
{
	return vk != VK_ESCAPE && vk != VK_INSERT && vk != VK_OEM_3;
}

EBindFlags CBindManager::parse_flags_out_of_string(const std::string & flags_str)
{
	EBindFlags flags = BINDFLAG_None;
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

			if (current_token == bind_flags_to_str[BINDFLAG_ExecuteOverUI])
			{
				flags |= BINDFLAG_ExecuteOverUI;
			}
			if (current_token == bind_flags_to_str[BINDFLAG_Silent])
			{
				flags |= BINDFLAG_Silent;
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

std::string CBindManager::create_string_out_of_flags(EBindFlags flags)
{
	bool more_than_one_flag = false;
	std::string flags_str;
	if (flags & BINDFLAG_ExecuteOverUI)
	{
		flags_str += bind_flags_to_str[BINDFLAG_ExecuteOverUI];
		more_than_one_flag = true;
	}
	if (flags & BINDFLAG_Silent)
	{
		if (more_than_one_flag) flags_str += ", ";
		flags_str += bind_flags_to_str[BINDFLAG_Silent];
		more_than_one_flag = true;
	}

	return flags_str;
}

bool CBindManager::is_key_bound(int vk)
{
	for (auto& [_vk, bind] : m_registerd_binds)
	{
		if (_vk == vk)
			return true;
	}
	return false;
}

void CBindManager::register_keypress_event_on_push(UserKey_t& key, int virtual_key, bind_t& new_bind)
{
	// on pressed, once
	key.add_on_pressed_callback(
		std::format("vk_{}", virtual_key),
		[](const std::string& id, UserKey_t* key)
		{
			int vk = std::stoi(id.substr(3));
			auto bind = g_bindmgr_i->get_bind(vk);

			if (bind)
			{
				if (g_bindmgr_i->is_ui_running() && !(bind->flags & BINDFLAG_ExecuteOverUI))
				{
					return;
				}

				g_variablemgr_i->execute_command(bind->cmd_sequence_0, bind->flags & BINDFLAG_Silent);
			}
		});
}

void CBindManager::register_keypress_event_toggle(UserKey_t& key, int virtual_key, bind_t& new_bind)
{
	// on pressed, once
	key.add_on_pressed_callback(
		std::format("vk_{}", virtual_key),
		[](const std::string& id, UserKey_t* key)
		{
			int vk = std::stoi(id.substr(3));
			auto bind = g_bindmgr_i->get_bind(vk);

			if (bind)
			{
				if (g_bindmgr_i->is_ui_running() && !(bind->flags & BINDFLAG_ExecuteOverUI))
				{
					return;
				}

				g_variablemgr_i->execute_command(bind->cmd_sequence_0, bind->flags & BINDFLAG_Silent);
			}
		});

	// on released, once
	key.add_on_unpressed_callback(
		std::format("vk_{}", virtual_key),
		[](const std::string& id, UserKey_t* key)
		{
			int vk = std::stoi(id.substr(3));
			auto bind = g_bindmgr_i->get_bind(vk);

			if (bind)
			{
				// always execute release callbacks, even we're on top of UI

				g_variablemgr_i->execute_command(bind->cmd_sequence_1, bind->flags & BINDFLAG_Silent);
			}
		});
}

void CBindManager::register_bind_internal(int virtual_key, UserKey_t& key, const std::string& command_seq, 
										  const std::string& command_seq1, EBindType type, EBindFlags flags, bool silent)
{
	std::string key_name = g_user_input_i->virtual_key_to_string(virtual_key);
	try
	{
		bind_t bind = { command_seq, command_seq1, type, flags };
		auto [it, did_insert] = m_registerd_binds.insert({ virtual_key, bind });
		if (!did_insert)
		{
			// re-bind
			(*it).second = bind;
		}

		switch (type)
		{
			case BIND_OnPush:
			{
				register_keypress_event_on_push(key, virtual_key, bind);
				break;
			}
			case BIND_Toggle:
			{
				register_keypress_event_toggle(key, virtual_key, bind);
				break;
			}
		}
	}
	catch (...)
	{
		CConsole::the().error("Failed to create new bind for key '{}'! (duplicated id?)", key_name);
		return;
	}

	if (!silent)
	{
		CConsole::the().info("Bound '{}' to key '{}'.", command_seq, key_name);
	}
}
