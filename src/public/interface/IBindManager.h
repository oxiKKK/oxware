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

#ifndef IBINDMANAGER_H
#define IBINDMANAGER_H
#pragma once

#include <nlohmann/json.hpp>

enum EBindType
{
	BIND_OnPush,			// executes the command only once, after the key has been pushed

	BIND_OnPushAndRelease,	// executes the first command as soon as the key is pressed, then executes the second
							// command after the key has been released.
};

static const char* bind_type_to_str[] =
{
	"on_push",				// BIND_OnPush
	"on_push_and_release",	// BIND_OnPushAndRelease
};

template <>
struct std::formatter<EBindType> : std::formatter<std::string> {
	auto format(EBindType type, std::format_context& ctx) {

		switch (type)
		{
			case BIND_OnPush:
			case BIND_OnPushAndRelease:
			{
				return std::formatter<string>::format(bind_type_to_str[type], ctx);
			}
			default:
			{
				return std::formatter<string>::format("unknown", ctx);
			}
		}
	}
};

enum EBindFlags
{
	BINDFLAG_None = 0,

	BINDFLAG_ExecuteOverUI = BIT(0),		// execute the bind still if the Cheat UI is up.
	BINDFLAG_ExecuteOverGameUI = BIT(1),	// execute the bind still if the Game UI is up.
	BINDFLAG_Silent = BIT(2),				// don't log anything to console when executing the bind.
	BINDFLAG_DisableInGameKey = BIT(3),		// don't run the engine command bound to same key when it is pressed
};
DEFINE_ENUM_BITWISE_OPERATORS(EBindFlags);

static std::unordered_map<EBindFlags, std::string> bind_flags_to_str
{
	{
		{ BINDFLAG_None, "none" },
		{ BINDFLAG_ExecuteOverUI, "execute_over_ui" },
		{ BINDFLAG_ExecuteOverGameUI, "execute_over_game_ui" },
		{ BINDFLAG_Silent, "silent" },
		{ BINDFLAG_DisableInGameKey, "disable_ingame_key" },
	}
};

template <>
struct std::formatter<EBindFlags> : std::formatter<std::string> {
	auto format(EBindFlags flags, std::format_context& ctx) {
		if (flags == BINDFLAG_None)
		{
			return std::formatter<string>::format("none", ctx);
		}

		std::string flags_str;
		bool add_colon = false;
		if (flags & BINDFLAG_ExecuteOverUI)
		{
			flags_str = bind_flags_to_str[BINDFLAG_ExecuteOverUI];
			add_colon = true;
		}
		if (flags & BINDFLAG_ExecuteOverGameUI)
		{
			flags_str += (add_colon ? ", " : "") + bind_flags_to_str[BINDFLAG_ExecuteOverGameUI];
			add_colon = true;
		}
		if (flags & BINDFLAG_Silent)
		{
			flags_str += (add_colon ? ", " : "") + bind_flags_to_str[BINDFLAG_Silent];
			add_colon = true;
		}
		if (flags & BINDFLAG_DisableInGameKey)
		{
			flags_str += (add_colon ? ", " : "") + bind_flags_to_str[BINDFLAG_DisableInGameKey];
			add_colon = true;
		}

		// NOTE: Don't forget to add separator ', ' after the first command

		return std::formatter<string>::format(flags_str, ctx);
	}
};

enum EBindInternalState
{
	BINDSTATE_None = 0,

	BINDSTATE_ExecutedWhenCheatUIWasOff = BIT(0),	// the bind was executed when UI was off. Handy for on push and release binds.
	BINDSTATE_ExecutedWhenGameUIWasOff = BIT(1),	// 
};
DEFINE_ENUM_BITWISE_OPERATORS(EBindInternalState);

struct bind_t
{
	std::string cmd_sequence_0, cmd_sequence_1;
	EBindType type;
	EBindFlags flags;

	// internal data
	EBindInternalState internal_state;
};

class IBindManager 
{
public:
	virtual void initialize() = 0;

	virtual void create_binds_from_json(const nh::json& json) = 0;
	virtual void export_binds_to_json(nh::json& json) = 0;

	virtual void add_bind(int virtual_key, const std::string& command_sequence, EBindFlags flags = BINDFLAG_None, bool silent = false) = 0;
	virtual void add_bind(const std::string& key_name, const std::string& command_sequence, EBindFlags flags = BINDFLAG_None, bool silent = false) = 0;

	virtual void add_bind_on_push_and_release(int virtual_key, const std::string& key_down_command_sequence, const std::string& key_up_command_sequence, EBindFlags flags = BINDFLAG_None, bool silent = false) = 0;
	virtual void add_bind_on_push_and_release(const std::string& key_name, const std::string& key_down_command_sequence, const std::string& key_up_command_sequence, EBindFlags flags = BINDFLAG_None, bool silent = false) = 0;

	virtual void remove_bind(int virtual_key, bool silent = false) = 0;
	virtual void remove_bind(const std::string& key_name, bool silent = false) = 0;

	virtual void remove_all_binds() = 0;

	virtual bind_t* get_bind(int virtual_key) = 0;
	virtual bind_t* get_bind(const std::string& key_name) = 0;

	virtual void for_each_bind(const std::function<void(int, const bind_t&)>& callback) = 0;

	virtual bool is_key_bindable(int vk) = 0;

	virtual EBindFlags parse_flags_out_of_string(const std::string& flags_str) = 0;
	virtual std::string create_string_out_of_flags(EBindFlags flags) = 0;

	virtual bool should_execute_bind(EBindFlags flags) = 0;

	virtual bool is_key_bound(int vk) = 0;

	// based on flags, tell whenever to block the engine key and execute our bind instead of engine bind.
	virtual bool should_block_engine_key(int vk) = 0;
};

extern IBindManager* g_bindmgr_i;

#define IBINDMANAGER_INTERFACEID "IBindManager"

#endif // IBINDMANAGER_H