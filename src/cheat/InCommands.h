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

#ifndef INCOMMANDS_H
#define INCOMMANDS_H
#pragma once

class BaseInCommand
{
public:
	BaseInCommand(const std::string& command_name) :
		m_command_name(command_name)
	{
	}

	inline bool is_active() const { return m_is_active; }
	inline auto get_cmd_name() const { return m_command_name; }

	virtual void register_cmd() = 0;

	virtual void add_to_global_list() = 0;

protected:
	bool m_is_active;

	std::string m_command_name;
};

// simple InCommand that activates when a key is down and deactivates when is up.
class InCommandSimple : public BaseInCommand
{
public:
	InCommandSimple(const std::string& command_name) :
		BaseInCommand(command_name)
	{
		add_to_global_list();
	}

	virtual void register_cmd();
	virtual void add_to_global_list();
};

class InCommandCustom;
using fnInCommandCustomCallback = void(*)(InCommandCustom* _this);

// InCommand providing custom callbacks on down-up events. 
class InCommandCustom : public BaseInCommand
{
public:
	InCommandCustom(const std::string& command_name, 
					const fnInCommandCustomCallback& dn, const fnInCommandCustomCallback& up) :
		BaseInCommand(command_name),
		m_command_dn(dn), 
		m_command_up(up)
	{
		add_to_global_list();
	}

	inline auto get_cmd_dn() const { return m_command_dn; }
	inline auto get_cmd_up() const { return m_command_up; }

	virtual void register_cmd();
	virtual void add_to_global_list();

protected:
	fnInCommandCustomCallback m_command_dn, m_command_up;
};

class CInCommands
{
public:
	DECL_BASIC_CLASS(CInCommands);

public:
	void initialize();

	void add_command(BaseInCommand* in_cmd);

private:
	std::unordered_map<std::string, BaseInCommand*> m_in_commands;
};

#endif // INCOMMANDS_H