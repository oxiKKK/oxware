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

IVariableManager* g_variablemgr_i = nullptr;

class CVariableManager : public IVariableManager
{
public:
	CVariableManager();
	~CVariableManager();
	
	bool initialize();

	// variable lookups
	BaseVariable* get(const char* name);

	void register_variables_per_module(StaticVariableContainer* container, const char* module_name);
	void register_single_variable(BaseVariable* var);

	void for_each_variable(const std::function<void(BaseVariable*)>& callback);

private:
	std::unordered_set<BaseVariable*> m_registered_variables;

	bool did_overflow(BaseVariable* var);
};

CVariableManager g_variablemgr;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CVariableManager, IVariableManager, IVARIABLEMANAGER_INTERFACEID, g_variablemgr);

//-------------------------------------------------------------------------------
//
// CVariableManager implementation
//
//-------------------------------------------------------------------------------

CVariableManager::CVariableManager()
{
	g_variablemgr_i = this;
}

CVariableManager::~CVariableManager()
{
	g_variablemgr_i = nullptr;
}

bool CVariableManager::initialize()
{
	register_variables_per_module(&g_static_variable_container, MODULE_UTIL);

	CConsole::the().info("VariableManager initialized.");
	CConsole::the().info("There are {} variables registered at initialization.", m_registered_variables.size());

	return true;
}

BaseVariable* CVariableManager::get(const char* name)
{
	for (BaseVariable* var : m_registered_variables)
	{
		if (!strcmp(var->get_name(), name))
			return reinterpret_cast<BaseVariable*>(var);
	}

	assert(0);
	return nullptr;
}

void CVariableManager::register_variables_per_module(StaticVariableContainer* container, const char* module_name)
{
	auto list = container->get_variable_list();

	if (list.empty())
	{
		CConsole::the().info("Module {} has no variables to be registered.", module_name);
		return;
	}

	CConsole::the().info("Registering {} variables from module {}...", list.size(), module_name);

	// Merge cvars from other modules into one continuous list.
	for (BaseVariable* var : list)
	{
		register_single_variable(var);
	}
}

void CVariableManager::register_single_variable(BaseVariable* var)
{
	if (did_overflow(var))
	{
		return;
	}

	auto [iter, did_insert] = m_registered_variables.insert(var);

	// same message is already inside BaseVariable.cpp, hence the "(2)"
	assert(did_insert && "Failed to insert a variable because it's already in the list.. duplicated? (2)"); 

	CConsole::the().info("Registered variable '{}'.", var->get_name());
}

void CVariableManager::for_each_variable(const std::function<void(BaseVariable*)>& callback)
{
	for (BaseVariable* var : m_registered_variables)
	{
		callback(var);
	}
}

bool CVariableManager::did_overflow(BaseVariable* var)
{
	if (m_registered_variables.size() >= k_max_variables_absolute)
	{
		CConsole::the().error("Variable {} couldn't be added, because the absolute limit has already been reached. ({})", 
							  var->get_name(), k_max_variables_absolute);
		return true;
	}

	return false;
}
