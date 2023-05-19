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
	BIND_OnPush,	// executes the command only once, after the key has been pushed
	BIND_Toggle,	// executes the first command as soon as the key is pressed, then executes the second
					// command after the key has been released.
};

template <>
struct std::formatter<EBindType> : std::formatter<std::string> {
	auto format(EBindType type, std::format_context& ctx) {

		switch (type)
		{
			case BIND_OnPush:		return std::formatter<string>::format("on_push", ctx);
			case BIND_Toggle:		return std::formatter<string>::format("toggle", ctx);
			default:				return std::formatter<string>::format("unknown", ctx);
		}
	}
};

enum EBindFlags
{
	BINDFLAG_None = 0,
	BINDFLAG_ForceExecution = BIT(0), // executes even when execution is normally disabled (e.g. when UI is up)
};

DEFINE_ENUM_BITWISE_OPERATORS(EBindFlags);

template <>
struct std::formatter<EBindFlags> : std::formatter<std::string> {
	auto format(EBindFlags flags, std::format_context& ctx) {

		std::string flags_str;
		if (flags & BINDFLAG_ForceExecution)
		{
			flags_str = "force_execution";
		}

		// NOTE: Don't forget to add separator ', ' after the first command

		return std::formatter<string>::format(flags_str, ctx);
	}
};

struct bind_t
{
	std::string cmd_sequence_0, cmd_sequence_1;
	EBindType type;
	EBindFlags flags;
};

class IBindManager 
{
public:
	virtual void initialize() = 0;
	virtual void shutdown() = 0;

	virtual void create_binds_from_json(const nlohmann::json& json) = 0;
	virtual void export_binds_to_json(nlohmann::json& json) = 0;

	virtual void add_bind(int virtual_key, const std::string& command_sequence, EBindFlags flags = BINDFLAG_None, bool silent = false) = 0;
	virtual void add_bind(const std::string& key_name, const std::string& command_sequence, EBindFlags flags = BINDFLAG_None, bool silent = false) = 0;

	virtual void add_bind_toggle(int virtual_key, const std::string& key_down_command_sequence, const std::string& key_up_command_sequence, EBindFlags flags = BINDFLAG_None, bool silent = false) = 0;
	virtual void add_bind_toggle(const std::string& key_name, const std::string& key_down_command_sequence, const std::string& key_up_command_sequence, EBindFlags flags = BINDFLAG_None, bool silent = false) = 0;

	virtual void remove_bind(int virtual_key, bool silent = false) = 0;
	virtual void remove_bind(const std::string& key_name, bool silent = false) = 0;

	virtual void remove_all_binds() = 0;

	virtual bind_t* get_bind(int virtual_key) = 0;
	virtual bind_t* get_bind(const std::string& key_name) = 0;

	virtual void for_each_bind(const std::function<void(int, const bind_t&)>& callback) = 0;

	virtual void set_can_execute_binds(bool can) = 0;
	virtual bool can_execute_binds() const = 0;

	virtual EBindFlags parse_flags_out_of_string(const std::string& flags_str) = 0;
};

extern IBindManager* g_bindmgr_i;

#define IBINDMANAGER_INTERFACEID "IBindManager"

#endif // IBINDMANAGER_H