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

//
// dllmain.cpp -- Module interface for oxWARE GUI module.
//

#include "precompiled.h"

// This
IBaseModule* g_thismodule_i = nullptr;

// Dependency loader
ILibraryLoader* g_libloader_i = nullptr;
IDeveloperConsole* g_devconsole_i = nullptr;

// Util
IVariableManager* g_variablemgr_i = nullptr;
IConfigManager* g_config_mgr_i = nullptr;
IRegistry* g_registry_i = nullptr;

class COxUIModule final : public IBaseModule
{
public:
	COxUIModule() { g_thismodule_i = this; }
	~COxUIModule() { g_thismodule_i = nullptr; }

	bool initialize(ModuleInitializationContext* context);
	void destroy();
	EUpdateStatus update();

	std::string get_module_name() { return MODULE_GUI; }

	void kill_module()
	{
		m_update_status = UPDATE_APP_EXIT;
	}

private:
	EUpdateStatus m_update_status = UPDATE_CONTINUE;
};

COxUIModule g_oxui_module;

//-------------------------------------------------------------------------------

bool COxUIModule::initialize(ModuleInitializationContext* context)
{
	g_libloader_i = context->libloader_i;
	g_devconsole_i = context->devconsole_i;

	CConsole::the().initialize(EOutputModule::GUI, g_devconsole_i);

	g_variablemgr_i = LocateExportedInterface<IVariableManager>(WMODULE_UTIL, IVARIABLEMANAGER_INTERFACEID);
	g_config_mgr_i = LocateExportedInterface<IConfigManager>(WMODULE_UTIL, ICONFIGMANAGER_INTERFACEID);
	g_registry_i = LocateExportedInterface<IRegistry>(WMODULE_UTIL, IREGISTRY_INTERFACEID);

	g_variablemgr_i->register_variables_and_commands_per_module(&g_static_variable_container, &g_static_command_container, MODULE_GUI);

	return true;
}

void COxUIModule::destroy()
{
	CConsole::the().destroy();

	// Dependency loader
	g_libloader_i = nullptr;
	g_devconsole_i = nullptr;

	// Util
	g_variablemgr_i = nullptr;
	g_config_mgr_i = nullptr;
	g_registry_i = nullptr;
}

EUpdateStatus COxUIModule::update()
{
	return m_update_status;
}

// Expose current module to the loader and initialize it
C_DLLEXPORT bool ExposeModule(IBaseModule** module_interface, ModuleInitializationContext* context)
{
	*module_interface = &g_oxui_module;
	if (!(*module_interface)->initialize(context))
	{
		return false;
	}

	CConsole::the().info("Module loaded and initialized.");
	
	return true;
}

//-------------------------------------------------------------------------------

BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved)
{
	return TRUE;
}
