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

#ifndef IVARIABLEMANAGER_H
#define IVARIABLEMANAGER_H
#pragma once

#include "IBaseModule.h"

#include <map>
#include <unordered_set>
#include <functional>

class BaseCommand;
class BaseVariable;

typedef void(*OnVariableChangeFn)(BaseVariable* var);
class VariableChangeCallbackContainer
{
public:
	inline void run_callbacks(BaseVariable* var) const
	{
		for (auto& c : m_callbacks)
		{
			c(var);
		}
	}

	inline void add(OnVariableChangeFn callback)
	{
		m_callbacks.insert(callback);
	}

	inline void remove(OnVariableChangeFn callback)
	{
		m_callbacks.erase(callback);
	}

public:
	std::unordered_set<OnVariableChangeFn> m_callbacks;
};

class LoadableModuleObject;
class StaticVariableContainer;
class StaticCommandContainer;

typedef int(__cdecl*m_hl_execute_cmd_pfn_t)(const char* cmd);

class IVariableManager : public IBaseInterface
{
public:
	virtual bool initialize() = 0;

	// variable and command lookups
	virtual BaseVariable* query_variable(const char* name) = 0;
	virtual BaseCommand* query_command(const char* name) = 0;

	virtual void register_variables_and_commands_per_module(StaticVariableContainer* variable_container, StaticCommandContainer* command_container, const char* module_name) = 0;
	virtual void register_single_variable(BaseVariable* var) = 0;
	virtual void register_single_command(BaseCommand* var) = 0;

	virtual void for_each_variable(const std::function<void(BaseVariable*)>& callback) = 0;
	virtual void for_each_command(const std::function<void(BaseCommand*)>& callback) = 0;

	virtual void execute_command(const std::string& command_sequence, bool silent = false) = 0;

	virtual void provide_hl_execute_cmd_pfn(m_hl_execute_cmd_pfn_t pfn) = 0;
};

extern IVariableManager* g_variablemgr_i;

#define IVARIABLEMANAGER_INTERFACEID "IVariableManager"

#endif // IVARIABLEMANAGER_H