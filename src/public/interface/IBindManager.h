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

class IBindManager 
{
public:
	virtual void initialize() = 0;
	virtual void shutdown() = 0;

	virtual void create_binds_from_json(const nlohmann::json& json) = 0;
	virtual void export_binds_to_json(nlohmann::json& json) = 0;

	virtual void add_bind(int virtual_key, const std::string& command_sequence, bool silent = false) = 0;
	virtual void remove_bind(int virtual_key, bool silent = false) = 0;

	virtual void add_bind(const std::string& key_name, const std::string& command_sequence, bool silent = false) = 0;
	virtual void remove_bind(const std::string& key_name, bool silent = false) = 0;

	virtual void remove_all_binds() = 0;

	virtual std::string get_command_sequence_bound_to_key(int virtual_key) = 0;
	virtual std::string get_command_sequence_bound_to_key(const std::string& key_name) = 0;

	virtual void for_each_bind(const std::function<void(int, const std::string&)>& callback) = 0;

	virtual void set_can_execute_binds(bool can) = 0;
	virtual bool can_execute_binds() const = 0;
};

extern IBindManager* g_bindmgr_i;

#define IBINDMANAGER_INTERFACEID "IBindManager"

#endif // IBINDMANAGER_H