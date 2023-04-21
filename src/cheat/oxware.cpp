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

// UI
IGUIWidgets* g_gui_widgets_i = nullptr;
IGUIFontManager* g_gui_fontmgr_i = nullptr;
IImGUIPlatformLayer* g_imgui_platform_layer_i = nullptr;
IGUIWindowRendering* g_gui_window_rendering_i = nullptr;
IGUIThemeManager* g_gui_thememgr_i = nullptr;

// Util
IImportBank* g_importbank_i = nullptr;
IRegistry* g_registry_i = nullptr;
IFileSystem* g_filesystem_i = nullptr;
IUserInput* g_user_input_i = nullptr;
IWindowMsgHandler* g_window_msg_handler_i = nullptr;
IConfigManager* g_config_mgr_i = nullptr;
IAppDataManager* g_appdata_mgr_i = nullptr;
IVariableManager* g_variablemgr_i = nullptr;
ICodePerfProfiler* g_code_perf_profiler_i = nullptr;

bool CoXWARE::run(injector_information_package_t* ifp)
{
	m_ifp = ifp;

	if (!initialize())
	{
		// if we have loaded some modules, we have to free all of them
		shutdown();
		return false;
	}

	// tell injector we've initialized.
	CInjectedDllIPCLayerClient::the().write_code(C2I_Init_OK);

	while (run_frame()) {}

	m_unloading_cheat = true;

	shutdown();

	// if this is true, some other thread will write this code nonetheless.
	if (!m_dont_write_unloading_code)
	{
		// tell injector we're done destroying and we can unload
		CInjectedDllIPCLayerClient::the().write_code(C2I_Unloading);
	}

	// don't call anything here. The injector is unloading us here.

	return true;
}

bool CoXWARE::initialize_phase2()
{
	if (!initialize_hook_managers())
	{
		return false;
	}
	
	CFeatureManager::the().initialize();

	CModelChams::the().initialize();

#ifdef OX_ENABLE_CODE_PROFILE
	g_code_perf_profiler_i->register_profile_report(&g_module_profile_report);
#endif

	CConsole::the().info("Cheat module fully initialized.");
	return true;
}

bool CoXWARE::initialize()
{
	CConsole::the().initialize(EOutputModule::CHEAT, g_devconsole_i);

	CInjectedDllIPCLayerClient::the().establish_communication(m_ifp->m_ipc_block_ptr);

	// load modules
	if (!load_and_initialize_dependencies())
	{
		CInjectedDllIPCLayerClient::the().report_error("Couldn't load and initialize dependencies.");
		return false;
	}

	if (!g_appdata_mgr_i->initialize_directory_hirearchy())
	{
		CInjectedDllIPCLayerClient::the().report_error("Couldn't initialize directory hirearchy.");
		return false;
	}

	if (!g_config_mgr_i->initialize())
	{
		CInjectedDllIPCLayerClient::the().report_error("Couldn't initialize config manager.");
		return false;
	}

	//
	// We, as a cheat dll, are reliant on the game. Hence, we need to wait till the game fully initializes.
	// We do this by loading our module first, then initialize game-independent subsystems for the module, and
	// then we hook the host_initialized globalvar and then we wait for the engine to execute it. After it is executed, 
	// the cheat initialization continues for subsystems that are reliant on the game. (the main cheat initialization
	// starts there..)
	// 
	// For example the engine dll loads up client.dll or vgui2.dll, but since our module is loaded before that, 
	// we cannot hook from those etc. So we need to wait for the engine to initialize.
	//

	// see for engine build
	if (!validate_engine_build())
	{
		return false;
	}

	// see if we're acutally cstrike
	if (!is_valid_game())
	{
		return false;
	}

	// see for the renderer - before hooks! (because of hw.dll may missing, and we need it inside hook managers.)
	if (!is_hardware())
	{
		return false;
	}

	if (!CMemoryHookMgr::the().host_initialized().install())
	{
		CInjectedDllIPCLayerClient::the().report_error("Fatal error, couldn't find the 'host_initialized' crucial global variable.");
		return false;
	}

	// see initialize_phase2() function.

	return true;
}

void CoXWARE::shutdown()
{
	CConsole::the().info("Shutting down cheat module subsystems...");

	if (g_config_mgr_i)
	{
		g_config_mgr_i->shutdown();
	}

	CFeatureManager::the().shutdown();

	COxWareUI::the().destroy();

	shutdown_hook_managers();

	unload_dependencies();

	m_shutted_down = true;
}

bool CoXWARE::load_and_initialize_dependencies()
{
	assert(m_ifp->m_ipc_block_ptr->m_technique != INJECT_UNINITIALIZED); // just sanity checks
	ELoadType type = (m_ifp->m_ipc_block_ptr->m_technique == INJECT_MANUALMAP) ? LOADTYPE_MANUALMAP : LOADTYPE_NATIVE;

	auto loader_path = FilePath_t(m_ifp->m_loader_path);

	// Util
	if (!CDependencyLoader::the().load_and_initialize_module(type, loader_path, WMODULE_UTIL, [&](const auto& mod)
	{
		g_importbank_i = mod.get_interface<IImportBank>(IIMPORTBANK_INTERFACEID);
		g_registry_i = mod.get_interface<IRegistry>(IREGISTRY_INTERFACEID);
		g_filesystem_i = mod.get_interface<IFileSystem>(IFILESYSTEM_INTERFACEID);
		g_user_input_i = mod.get_interface<IUserInput>(IUSERINPUT_INTERFACEID);
		g_window_msg_handler_i = mod.get_interface<IWindowMsgHandler>(IWINDOWMSGHANDLER_INTERFACEID);
		g_config_mgr_i = mod.get_interface<IConfigManager>(ICONFIGMANAGER_INTERFACEID);
		g_appdata_mgr_i = mod.get_interface<IAppDataManager>(IAPPDATAMANAGER_INTERFACEID);
		g_variablemgr_i = mod.get_interface<IVariableManager>(IVARIABLEMANAGER_INTERFACEID);
		g_code_perf_profiler_i = mod.get_interface<ICodePerfProfiler>(ICODEPERFPROFILER_INTERFACEID);

		return g_importbank_i && g_registry_i && g_filesystem_i && g_user_input_i && g_window_msg_handler_i && g_config_mgr_i &&
			g_appdata_mgr_i && g_variablemgr_i && g_code_perf_profiler_i;
	}))
	{
		return false;
	}

	// UI
	if (!CDependencyLoader::the().load_and_initialize_module(type, loader_path, WMODULE_GUI, [&](const auto& mod)
	{
		g_gui_widgets_i = mod.get_interface<IGUIWidgets>(IGUIWIDGETS_INTERFACEID);
		g_gui_fontmgr_i = mod.get_interface<IGUIFontManager>(IGUIFONTMANAGER_INTERFACEID);
		g_imgui_platform_layer_i = mod.get_interface<IImGUIPlatformLayer>(IIMGUIPLATFORMLAYER_INTERFACEID);
		g_gui_window_rendering_i = mod.get_interface<IGUIWindowRendering>(IGUIWINDOWRENDERING_INTERFACEID);
		g_gui_thememgr_i = mod.get_interface<IGUIThemeManager>(IGUITHEMEMANAGER_INTERFACEID);

		return g_gui_widgets_i && g_gui_fontmgr_i && g_imgui_platform_layer_i && g_gui_window_rendering_i && g_gui_thememgr_i;
	}))
	{
		return false;
	}

	g_variablemgr_i->register_variables_per_module(&g_static_variable_container, MODULE_CHEAT);

	// after all modules have been loaded
	if (!g_variablemgr_i->initialize())
	{
		return false;
	}

	return true;
}

bool CoXWARE::run_frame()
{
	if (!can_update_frame())
	{
		return true;
	}
	else
	{
		m_main_frame_update = std::chrono::high_resolution_clock::now();
	}

	// keep communication with the injector on
	if (CInjectedDllIPCLayerClient::the().dispatch() != k_IPCLayerStatus_Ok)
	{
		return false;
	}

	// someone requested to close the cheat.
	if (is_exit_requested())
	{
		return false;
	}

	// initialize after the engine has initialized.
	static bool initialize_once = true;
	if (initialize_once)
	{
		// watch till this gets triggered.
		if (CMemoryHookMgr::the().host_initialized().is_installed()
			&& CMemoryHookMgr::the().host_initialized().get() != FALSE)
		{
			if (!initialize_phase2())
			{
				return false;
			}
			initialize_once = false;
		}
	}

	//-------------------------------------------------------------------------
	// Update code

#ifdef OX_ENABLE_CODE_PROFILE
	g_code_perf_profiler_i->update();
#endif

	g_config_mgr_i->update();
	
	if (!CDependencyLoader::the().update())
	{
		return false;
	}

#if 0
	if (CMemoryFnHookMgr::the().VGuiWrap2_ConPrintf().is_installed())
	{
		CMemoryFnHookMgr::the().VGuiWrap2_ConPrintf().call("tick\n");
	}
#endif

	return true;
}

void CoXWARE::unload_dependencies()
{
	if (g_registry_i)
	{
		g_registry_i->shutdown();
	}

	CConsole::the().destroy();

	CDependencyLoader::the().unload_all();

	// At this point all of the interfaces we obtained are gone.
	g_devconsole_i = nullptr;
	g_gui_widgets_i = nullptr;
	g_gui_fontmgr_i = nullptr;
	g_imgui_platform_layer_i = nullptr;
	g_gui_window_rendering_i = nullptr;
	g_gui_thememgr_i = nullptr;

	// Util
	g_importbank_i = nullptr;
	g_registry_i = nullptr;
	g_filesystem_i = nullptr;
	g_user_input_i = nullptr;
	g_window_msg_handler_i = nullptr;
	g_config_mgr_i = nullptr;
	g_appdata_mgr_i = nullptr;
	g_variablemgr_i = nullptr;
	g_code_perf_profiler_i = nullptr;
}

bool CoXWARE::initialize_hook_managers()
{
	// memory hook
	if (!CMemoryHookMgr::the().install_hooks())
	{
		CConsole::the().error("Failed to install hooks.");
		return false;
	}

	// memory function hook
	if (!CMemoryFnHookMgr::the().install_hooks())
	{
		CConsole::the().error("Failed to install function hooks.");
		return false;
	}

	// memory function detour
	if (!CMemoryFnDetourMgr::the().install_hooks())
	{
		CConsole::the().error("Failed to install detour hooks.");
		return false;
	}

	// HL interface hook
	if (!CHLInterfaceHook::the().install_hooks())
	{
		CConsole::the().error("Failed to install HL interface hooks.");
		return false;
	}

	// CBase clientside class hook
	if (!CMemoryHookCBaseStuff::the().install_hooks())
	{
		CConsole::the().error("Failed to install CBase stuff hooks.");
		return false;
	}

	// HLNetMessageIO hooks
	if (!CHLNetMessageIO::the().install_hooks())
	{
		CConsole::the().error("Failed to install NetMessage hooks.");
		return false;
	}

	// SVC hook
	if (!CSVCFuncDetourMgr::the().install_hooks())
	{
		CConsole::the().error("Failed to install SVC hooks.");
		return false;
	}

	// UserMSG hook
	if (!CUserMSGDetourMgr::the().install_hooks())
	{
		CConsole::the().error("Failed to install UserMSG hooks.");
		return false;
	}

	CConsole::the().info("Initialized all hook managers.");

	return true;
}

void CoXWARE::shutdown_hook_managers()
{
	CMemoryHookMgr::the().uninstall_hooks();
	// CMemoryFnHookMgr doesn't uninstall since it doesn't modify memory, just hooks it.
	CMemoryFnDetourMgr::the().uninstall_hooks();
	CMemoryHookCBaseStuff::the().uninstall_hooks();
	CSVCFuncDetourMgr::the().uninstall_hooks();
	CUserMSGDetourMgr::the().uninstall_hooks();
}

bool CoXWARE::is_valid_game()
{
	auto app_id = CGameUtil::the().get_current_app_id();
	if (app_id != 10) // cs
	{
		CInjectedDllIPCLayerClient::the().report_error("You are not running Counter-Strike game. Your appid={}", app_id);
		return false;
	}

	CConsole::the().info("Running game Counter-Strike");

	return true;
}

bool CoXWARE::is_hardware()
{
	// we check if we're running the hardware engine by looking whenever the hw.dll is loaded in our process.
	// 
	// we have to check before we isntall any hooks, because we're targeting hw.dll inside these, and hence
	// if we're sw, there will be no hwdll...

	bool is_hw = g_libloader_i->is_dll_loaded(L"hw.dll");

	if (!is_hw) // software
	{
		CInjectedDllIPCLayerClient::the().report_error("You must run the game using the OpenGL renderer (-gl), not the software renderer.");
		return false;
	}

	CConsole::the().info("Running on the OpenGL/D3D renderer of the game.");

	return is_hw;
}

bool CoXWARE::validate_engine_build()
{
	int build_num = CGameUtil::the().get_build_number();

	if (!build_num)
	{
		CInjectedDllIPCLayerClient::the().report_error("Couldn't get the engine build number at all! You must be using corrupted or ancient version of cs!");
		return false;
	}

	if (build_num != 8684)
	{
		CInjectedDllIPCLayerClient::the().report_error("Your build number isn't 8684! This cheat is currently only available for steam. (your build num: {})", build_num);
		return false;
	}

	CConsole::the().info("Your build number is {}.", build_num);
	return true;
}

void CoXWARE::on_render()
{
	static constexpr auto window_flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoNav;

	g_gui_widgets_i->create_new_window(
		"main_window", window_flags, [this]()
		{
			g_gui_widgets_i->add_text("Hello");
		});
}
