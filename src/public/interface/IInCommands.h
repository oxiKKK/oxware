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

#ifndef IINCOMMANDS_H
#define IINCOMMANDS_H
#pragma once

#include "interface.h"

#include "BaseInCommand.h"

class IInCommands : public IBaseInterface
{
public:
	virtual void initialize() = 0;

	// call this in the beginning of engine frame, but not too early. CL_Move is enough (CL_CreateMove, too)
	virtual void update_activation_conditions() = 0;

	virtual void add_command(BaseInCommand* in_cmd) = 0;

	virtual void register_incommands_per_module(StaticInCommandContainer* incommand_container, const char* module_name) = 0;

	virtual void for_each_incommand(const std::function<void(BaseInCommand* in_cmd)>& callback) = 0;

	virtual void create_incommands_from_json(const nh::json& json) = 0;
	virtual void export_incommands_to_json(nh::json& json) = 0;

	virtual BaseInCommand* get_incommand(const std::string& id) = 0;

	virtual bool is_key_bound_and_active(int vk) = 0;

	// user can explicitly tell whenever we should execute the engine key as well or block it.
	virtual bool should_block_engine_key(int vk) = 0;

	virtual bool does_meet_activation_conditions(EActivationCondition act_cond) = 0;
};

extern IInCommands* g_in_commands_i;

#define IINCOMMANDS_INTERFACEID "IInCommands"

#endif // IINCOMMANDS_H