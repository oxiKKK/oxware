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

class BaseInCommand
{
public:
	explicit BaseInCommand(const std::string& command_name, int key_default, VarBoolean* var_can_be_activated,
						   fnInCommandCallback in, fnInCommandCallback out) :
		m_command_name(command_name),
		m_vk(key_default),
		m_var_can_be_activated(var_can_be_activated),
		m_in_fn(in), m_out_fn(out)
	{
	}

	inline bool is_active() const { return m_is_active && (m_var_can_be_activated && m_var_can_be_activated->get_value()); }
	inline auto get_cmd_name() const { return m_command_name; }
	inline int get_key_bound() const { return m_vk; } // 0 if unbound

	inline auto get_in_fn() const { return m_in_fn; }
	inline auto get_out_fn() const { return m_out_fn; }

	void rebind_key_to(int new_vk);

	virtual void register_cmd() = 0;

	virtual void add_to_global_list() = 0;

	void set_is_active(bool is_active) { m_is_active = is_active; }

protected:
	bool m_is_active = false;
	int m_vk = NULL; // 0 if unbound

	std::string m_command_name;

	// down/up callbacks
	fnInCommandCallback m_in_fn, m_out_fn;

	bool m_bound_initially = false;

	VarBoolean* m_var_can_be_activated = nullptr;
};

class InCommand : public BaseInCommand
{
public:
	explicit InCommand(const std::string& command_name, int key_default, VarBoolean* var_can_be_activated) :
		BaseInCommand(command_name, key_default, var_can_be_activated,
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