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

#ifndef BASEINCOMMAND_H
#define BASEINCOMMAND_H
#pragma once

#include "BaseVariable.h"

class BaseInCommand;
using fnInCommandCallback = void(*)(BaseInCommand* _this);

// Conditions whenever to activate the incommand or not.
// these are hardcoded to each incmd and cannot be changed by the user.
enum EActivationCondition
{
	IN_ACTCOND_None = BIT(-1),
	IN_ACTCOND_Connected = BIT(0),			// executes only if connected to a server
	IN_ACTCOND_Alive = BIT(1),				// executes only if alive
	IN_ACTCOND_NotInMessageMode = BIT(2),	// executes only if not in messagemode (chat opened)
};
DEFINE_ENUM_BITWISE_OPERATORS(EActivationCondition);

enum EInCommandFlags
{
	IN_FLAG_None = BIT(-1),
	IN_FLAG_DisableInGameKey = BIT(0),	// don't run the engine command bound to same key when it is pressed
};
DEFINE_ENUM_BITWISE_OPERATORS(EInCommandFlags);

static std::unordered_map<EInCommandFlags, std::string> incommand_flags_to_str = 
{
	{
		{ IN_FLAG_None, "none" },
		{ IN_FLAG_DisableInGameKey, "disable_ingame_key" },
	}
};

template <>
struct std::formatter<EInCommandFlags> : std::formatter<std::string> {
	auto format(EInCommandFlags flags, std::format_context& ctx) {
		if (flags == IN_FLAG_None)
		{
			return std::formatter<string>::format("none", ctx);
		}

		std::string flags_str;
		bool add_colon = false;
		if (flags & IN_FLAG_DisableInGameKey)
		{
			flags_str = incommand_flags_to_str[IN_FLAG_DisableInGameKey];
			add_colon = true;
		}
		//if (flags & XXX)
		//{
		//	flags_str += (add_colon ? ", " : "") + incommand_flags_to_str[XXX];
		//	add_colon = true;
		//}

		// NOTE: Don't forget to add separator ', ' after the first command

		return std::formatter<string>::format(flags_str, ctx);
	}
};

class BaseInCommand
{
public:
	explicit BaseInCommand(const std::string& name, int key_default, bool enable_by_default,
						   EActivationCondition act_cond,
						   fnInCommandCallback in, fnInCommandCallback out) :
		m_name(name),
		m_vk(key_default),
		m_enable_variable_by_default(enable_by_default),
		m_activation_condition(act_cond),
		m_in_fn(in), m_out_fn(out)
	{
	}

	// tell whenever the InCommand is activated or not
	bool is_active() const;

	inline auto get_activation_condition() const { return m_activation_condition; }
	inline const auto& get_name() const { return m_name; }
	inline int get_key_bound() const { return m_vk; } // 0 if unbound

	inline auto get_in_fn() const { return m_in_fn; }
	inline auto get_out_fn() const { return m_out_fn; }
	inline auto get_toggle_var() const { return m_var_toggle; }
	inline auto get_always_enabled_var() const { return m_var_always_enabled; }
	inline auto get_flags() const { return m_flags; }

	void rebind_key_to(int new_vk);

	virtual void register_cmd() = 0;

	virtual void add_to_global_list() = 0;

	// setters
	inline void set_is_active(bool is_active) { m_is_active = is_active; }
	inline void set_flags(EInCommandFlags f) { m_flags = f; }
	inline void add_flags(EInCommandFlags f, bool set)
	{
		if (set)
		{
			m_flags |= f;
		}
		else
		{
			m_flags &= ~f;
		}
	}

protected:
	bool m_is_active = false;
	int m_vk = NULL; // 0 if unbound

	std::string m_name;

	// down/up callbacks
	fnInCommandCallback m_in_fn, m_out_fn;

	bool m_bound_initially = false;

	// variable to enable/disable this incommand completely (optional)
	VarBoolean* m_var_toggle = nullptr;
	VarBoolean* m_var_always_enabled = nullptr;
	bool m_enable_variable_by_default = false;

	EActivationCondition m_activation_condition = IN_ACTCOND_None;

	EInCommandFlags m_flags = IN_FLAG_None;
};

class InCommand : public BaseInCommand
{
public:
	explicit InCommand(const std::string& name, int key_default, bool enable_by_default, EActivationCondition act_cond = IN_ACTCOND_None) :
		BaseInCommand(name, key_default, enable_by_default, act_cond,
					  [](BaseInCommand* _this) { _this->set_is_active(true); },
					  [](BaseInCommand* _this) { _this->set_is_active(false); })
	{
		add_to_global_list();
	}

	virtual void register_cmd();
	virtual void add_to_global_list();
};

//--------------------------------------------------------------------------------------------------------------------

#include <unordered_set>

// maximum amount of incommands that our list can hold
static constexpr size_t k_max_incommands = 64; // per_module limit
static constexpr size_t k_max_incommands_absolute = k_max_incommands * 4; // used in the interface code

// temporary storage to collect all globally declared incommands. The collected data will be then transefered
// to IInCommand interface that will collect all incommands at runtime.
class StaticInCommandContainer
{
public:
	void add_incommand(BaseInCommand* in_cmd);

	inline auto& get_incommand_list() const { return m_incommands; }

private:
	// call only once
	void reserve_limit();

	bool see_for_overflow(BaseInCommand* current_incommand);

private:
	std::unordered_set<BaseInCommand*> m_incommands;
};

extern StaticInCommandContainer g_static_incommand_container;

inline void InCommand::add_to_global_list()
{
	g_static_incommand_container.add_incommand(this);
}

#endif // BASEINCOMMAND_H