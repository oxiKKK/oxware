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
	"bind", "<key> <command>", "Binds command to a key",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		if (args.count() == 1 || args.count() > 3)
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
			auto cmd_seq = g_bindmgr_i->get_command_sequence_bound_to_key(vk);
			if (!cmd_seq.empty())
			{
				CConsole::the().info("'{}' is bound to '{}'", args.get(1), cmd_seq);
			}
		}
		else
		{
			g_bindmgr_i->add_bind(args.get(1), args.get(2));
		}
	}
);

BaseCommand list_binds(
	"list_binds", "Prints all currently bound binds",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		CConsole::the().info("{:<3}: {:<16} {}", "id", "key", "command");
		int num = 0;
		g_bindmgr_i->for_each_bind(
			[&](int vk, const std::string& cmd)
			{
				num++;
				CConsole::the().info("{:<3}: {:<16} \"{}\"", num, g_user_input_i->virtual_key_to_string(vk), cmd);
			});

		CConsole::the().info("------------------------------");
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

	void add_bind(int virtual_key, const std::string& command_sequence, bool silen);
	void remove_bind(int virtual_key, bool silen);

	void add_bind(const std::string& key_name, const std::string& command_sequence, bool silen);
	void remove_bind(const std::string& key_name, bool silen);

	void remove_all_binds();

	std::string get_command_sequence_bound_to_key(int virtual_key);
	std::string get_command_sequence_bound_to_key(const std::string& key_name);

	void for_each_bind(const std::function<void(int, const std::string&)>& callback);

	bool is_there_a_command_bound_to_some_key(const std::string& cmd);

	void set_can_execute_binds(bool can) { m_can_execute_binds = can; }
	bool can_execute_binds() const { return m_can_execute_binds; }

private:
	// virtual key as key :) and command sequence as value
	std::map<int, std::string> m_registerd_binds;

	bool m_can_execute_binds = false;
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
	try
	{
		auto& binds = json.at("binds");
		if (binds.empty())
		{
			return;
		}

		for (auto& [key, bind] : binds.items())
		{
			auto key_command = bind.get<std::string>();
			add_bind(key, key_command, false);
		}
	}
	catch (const nlohmann::json::exception& e)
	{
		CConsole::the().error("JSON error: {}", e.what());
	}
}

void CBindManager::export_binds_to_json(nlohmann::json& json)
{
	for (const auto& [vk, cmd] : m_registerd_binds)
	{
		auto key_name = g_user_input_i->virtual_key_to_string(vk);
		if (key_name.empty())
		{
			continue;
		}

		try
		{
			json["binds"][key_name] = cmd;
		}
		catch (const nlohmann::json::exception& e)
		{
			CConsole::the().error("JSON error: {}", e.what());
		}
	}
}

void CBindManager::add_bind(int virtual_key, const std::string& command_sequence, bool silent)
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
	
	try
	{
		auto [it, did_insert] = m_registerd_binds.insert({ virtual_key, command_sequence });
		if (!did_insert)
		{
			// re-bind
			(*it).second = command_sequence;
		}
	}
	catch (...)
	{
		CConsole::the().error("Failed to create new bind for key '{}'! (duplicated id?)", key_name);
		return;
	}

	key.add_on_pressed_callback(
		std::format("vk_{}", virtual_key),
		[&](const std::string& id, UserKey_t* key)
		{
			int vk = std::stoi(id.substr(3));
			std::string key_name = g_user_input_i->virtual_key_to_string(vk);
			std::string cmd_seq = g_bindmgr_i->get_command_sequence_bound_to_key(vk);

			if (!g_bindmgr_i->can_execute_binds() && 
				// kinda dumb, but whatever, maybe TODO in the future? Improve?
				cmd_seq != "ui_toggle_menu" && cmd_seq != "ui_toggle_console")
			{
				return;
			}

			CConsole::the().info("Executing bind for key '{}': '{}'", key_name, cmd_seq);
			g_variablemgr_i->execute_command(cmd_seq);
		});

	if (!silent)
	{
		CConsole::the().info("Bound '{}' to key '{}'.", command_sequence, key_name);
	}
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

	try
	{
		m_registerd_binds.erase(virtual_key);
	}
	catch (...)
	{
		CConsole::the().error("Can't unbind. Key '{}' not bound.", key_name);
		return;
	}

	key.remove_on_pressed_callback(std::format("vk_{}", virtual_key));

	if (!silent)
	{
		CConsole::the().info("Unbound key '{}'.", key_name);
	}
}

void CBindManager::add_bind(const std::string& key_name, const std::string& command_sequence, bool silent)
{
	int vk = g_user_input_i->string_to_virtual_key(key_name);
	if (vk == NULL)
	{
		return;
	}

	add_bind(vk, command_sequence, silent);
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

std::string CBindManager::get_command_sequence_bound_to_key(int virtual_key)
{
	try
	{
		return m_registerd_binds.at(virtual_key);
	}
	catch (...)
	{
		std::string key_name = g_user_input_i->virtual_key_to_string(virtual_key);
		CConsole::the().error("Virtual key '{}' not bound!", key_name);
		return "";
	}
}

std::string CBindManager::get_command_sequence_bound_to_key(const std::string& key_name)
{
	int vk = g_user_input_i->string_to_virtual_key(key_name);
	if (vk == NULL)
	{
		return "";
	}

	return get_command_sequence_bound_to_key(vk);
}

void CBindManager::for_each_bind(const std::function<void(int, const std::string&)>& callback)
{
	for (const auto [vk, cmd] : m_registerd_binds)
	{
		callback(vk, cmd);
	}
}

bool CBindManager::is_there_a_command_bound_to_some_key(const std::string& cmd)
{
	for (const auto& [vk, _cmd] : m_registerd_binds)
	{
		if (_cmd.find(cmd) != std::string::npos)
			return true;
	}

	return false;
}
