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
IRegistry* g_registry_i = nullptr;
IFileSystem* g_filesystem_i = nullptr;
IUserInput* g_user_input_i = nullptr;
IWindowMsgHandler* g_window_msg_handler_i = nullptr;
IConfigManager* g_config_mgr_i = nullptr;
IAppDataManager* g_appdata_mgr_i = nullptr;
IVariableManager* g_variablemgr_i = nullptr;
ICodePerfProfiler* g_code_perf_profiler_i = nullptr;
IBytePatternBank* g_bytepattern_bank_i = nullptr;
IBindManager* g_bindmgr_i = nullptr;

BaseCommand unload_cheat(
	"unload_cheat", "Unloads the cheat. Same as unloading the cheat through menu.",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		COxWare::the().end_cheat_execution();
	}
);

VarBoolean debug("debug", "Enables debug mode", false);

bool COxWare::run(injector_information_package_t* ifp)
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

	while (run_frame()) 
	{
		std::this_thread::sleep_for(10ms);
	}

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

bool COxWare::initialize_phase2()
{
	if (!initialize_hook_managers())
	{
		return false;
	}

	g_variablemgr_i->provide_hl_execute_cmd_pfn((m_hl_execute_cmd_pfn_t)CMemoryHookMgr::the().cl_enginefuncs()->pfnClientCmd);
	
	CFeatureManager::the().initialize();

#ifdef OX_ENABLE_CODE_PROFILE
	g_code_perf_profiler_i->register_profile_report(&g_module_profile_report);
#endif

	g_gui_thememgr_i->load_font_from_file_if_present();

	CConsole::the().info("Cheat module fully initialized.");
	m_fully_initialized = true;
	return true;
}

const wchar_t* COxWare::get_engine_fs_module_name() const
{
	uintptr_t module = g_libloader_i->get_target_loaded_dll_base_address(L"filesystem_stdio.dll");
	if (module != NULL)
	{
		return L"filesystem_stdio.dll";
	}

	return L"filesystem_steam.dll";
}

bool COxWare::initialize()
{
	CConsole::the().initialize(EOutputModule::CHEAT, g_devconsole_i);

	CInjectedDllIPCLayerClient::the().establish_communication(m_ifp->m_ipc_block_ptr);

	if (get_injection_technique() == INJECT_MANUALMAP)
	{
		// if we're manually mapped module, we need to provide the libraryloader interface our
		// exported functions / interfaces, so that other modules can load them.
		g_libloader_i->register_manualmapped_module(WMODULE_CHEAT, get_cheat_dll_base_address());
	}

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

	g_devconsole_i->enable_tooltip(true);

	// keyboard/mouse I/O
	g_user_input_i->initialize();

	g_bindmgr_i->initialize();
	g_in_commands_i->initialize();

	// call all interface initialization routines that provide load&export callbacks before config manager initializes, 
	// because otherwise those callbacks wouldn't be provided and the cfgmgr would load configs without these callbacks at 
	// startup.
	g_config_mgr_i->initialize();

	check_for_clientside_protectors();

	// check for encrypted modules, we need to encrypt them before we proceed. if we fail, we have to exit.
	if (!check_for_encrypted_modules())
	{
		return false;
	}

	// see for the renderer - before hooks! (because of hw.dll may missing, and we need it inside hook managers.)
	if (!is_hardware())
	{
		return false;
	}

	// see for engine build
	if (!validate_engine_build())
	{
		return false;
	}

	if (!g_bytepattern_bank_i->initialize(m_gs_build_number))
	{
		return false;
	}

	// see if we're acutally cstrike
	if (!is_valid_game())
	{
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

	auto& host_initialized = CMemoryHookMgr::the().host_initialized();
	if (!host_initialized.install())
	{
		CInjectedDllIPCLayerClient::the().report_error("Fatal error, couldn't find the 'host_initialized' crucial global variable.");
		return false;
	}

	int tries = 0;
	static int tries_max = 20;
	static const auto wait_time = 0.5s;
	while (!*host_initialized.get())
	{
		CConsole::the().warning("Waiting for the engine to initialize...{}", tries != 0 ? std::format(" ({}x)", tries) : "");

		std::this_thread::sleep_for(wait_time);
		if (++tries >= tries_max) // don't run this infinitely
		{
			CInjectedDllIPCLayerClient::the().report_error("Fatal error, tried {}x but the engine didn't initialize after launching.", tries_max);
			return false;
		}
	}

	if (!initialize_phase2())
	{
		return false;
	}

	return true;
}

void COxWare::shutdown()
{
	CConsole::the().info("Shutting down cheat module subsystems...");

	// save unload
	if (!m_dont_write_unloading_code)
	{
		CEngineSynchronization::the().put_engine_in_sleep();
	}

	if (g_config_mgr_i)
	{
		g_config_mgr_i->shutdown();
	}

	CFeatureManager::the().shutdown();

	CIngameScreenRendering::the().shutdown();

	CEnvironmentalEffects::the().shutdown();

	CEngineSoundPlayer::the().stop_all_ambient_sounds();

	COxWareUI::the().destroy();

	CGoldSrcCommandMgr::the().shutdown();

	if (get_injection_technique() == INJECT_MANUALMAP)
	{
		remove_inverted_function_table();
	}

	shutdown_hook_managers();

	unload_dependencies();

	// this isn't really needed, since _Host_Frame is already unloaded, but whatever..
	if (!m_dont_write_unloading_code)
	{
		CEngineSynchronization::the().resume_engine();
	}

	m_shutted_down = true;
}

bool COxWare::load_and_initialize_dependencies()
{
	assert(get_injection_technique() != INJECT_UNINITIALIZED); // just sanity checks
	ELoadType type = (get_injection_technique() == INJECT_MANUALMAP) ? LOADTYPE_MANUALMAP : LOADTYPE_NATIVE;

	auto loader_path = std::filesystem::path(m_ifp->m_loader_path);

	// Util
	if (!CDependencyLoader::the().load_and_initialize_module(type, loader_path, WMODULE_UTIL, [&](const auto& mod)
	{
		g_registry_i = mod.get_interface<IRegistry>(IREGISTRY_INTERFACEID);
		g_filesystem_i = mod.get_interface<IFileSystem>(IFILESYSTEM_INTERFACEID);
		g_user_input_i = mod.get_interface<IUserInput>(IUSERINPUT_INTERFACEID);
		g_window_msg_handler_i = mod.get_interface<IWindowMsgHandler>(IWINDOWMSGHANDLER_INTERFACEID);
		g_config_mgr_i = mod.get_interface<IConfigManager>(ICONFIGMANAGER_INTERFACEID);
		g_appdata_mgr_i = mod.get_interface<IAppDataManager>(IAPPDATAMANAGER_INTERFACEID);
		g_variablemgr_i = mod.get_interface<IVariableManager>(IVARIABLEMANAGER_INTERFACEID);
		g_code_perf_profiler_i = mod.get_interface<ICodePerfProfiler>(ICODEPERFPROFILER_INTERFACEID);
		g_bytepattern_bank_i = mod.get_interface<IBytePatternBank>(IBYTEPATTERNBANK_INTERFACEID);
		g_bindmgr_i = mod.get_interface<IBindManager>(IBINDMANAGER_INTERFACEID);

		return g_registry_i && g_filesystem_i && g_user_input_i && g_window_msg_handler_i && g_config_mgr_i &&
			g_appdata_mgr_i && g_variablemgr_i && g_code_perf_profiler_i && g_bytepattern_bank_i && g_bindmgr_i;
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

	g_gui_thememgr_i->initialize();

	g_variablemgr_i->register_variables_and_commands_per_module(&g_static_variable_container, &g_static_command_container, MODULE_CHEAT);

	// after all modules have been loaded
	if (!g_variablemgr_i->initialize())
	{
		return false;
	}

	return true;
}

bool COxWare::run_frame()
{
	// someone requested to close the cheat.
	if (is_exit_requested())
	{
		CConsole::the().info("Someone requested cheat exit. Exiting cheat main frame loop.");
		return false;
	}
	
	// keep communication with the injector on
	if (CInjectedDllIPCLayerClient::the().dispatch() != k_IPCLayerStatus_Ok)
	{
		return false;
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

	return true;
}

void COxWare::unload_dependencies()
{
	if (g_registry_i)
	{
		g_registry_i->shutdown();
	}

	CConsole::the().destroy();

	CDependencyLoader::the().unload_all();

	// At this point all of the interfaces we obtained are gone.
	g_gui_widgets_i = nullptr;
	g_gui_fontmgr_i = nullptr;
	g_imgui_platform_layer_i = nullptr;
	g_gui_window_rendering_i = nullptr;
	g_gui_thememgr_i = nullptr;

	// Util
	g_registry_i = nullptr;
	g_filesystem_i = nullptr;
	g_user_input_i = nullptr;
	g_window_msg_handler_i = nullptr;
	g_config_mgr_i = nullptr;
	g_appdata_mgr_i = nullptr;
	g_variablemgr_i = nullptr;
	g_code_perf_profiler_i = nullptr;
	g_bytepattern_bank_i = nullptr;
	g_bindmgr_i = nullptr;
}

bool COxWare::initialize_hook_managers()
{
	// security module hook
	if (!CSecurityModuleHook::the().install_hooks())
	{
		CConsole::the().error("Failed to install security module hooks.");
		return false;
	}

	// memory hook
	if (!CMemoryHookMgr::the().install_hooks())
	{
		CConsole::the().error("Failed to install hooks.");
		return false;
	}

	CVideoModeUtil::the().initialize();

	CGoldSrcCommandMgr::the().initialize();

	// HL interface hook
	if (!CHLInterfaceHook::the().install_hooks())
	{
		CConsole::the().error("Failed to install HL interface hooks.");
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

	// Half-Life commands hook
	if (!CHLCommandsDetourMgr::the().install_hooks())
	{
		CConsole::the().error("Failed to install HL commands hooks.");
		return false;
	}

	CConsole::the().info("Initialized all hook managers.");

#ifdef OX_ENABLE_HOOK_TESTING
	CHookTests::the().execute_tests();
#endif

	return true;
}

void COxWare::shutdown_hook_managers()
{
	// we uninstall hooks only that ch ange memory.
	CSecurityModuleHook::the().uninstall_hooks();
	CMemoryHookMgr::the().uninstall_hooks();
	CMemoryFnDetourMgr::the().uninstall_hooks();
	CSVCFuncDetourMgr::the().uninstall_hooks();
	CUserMSGDetourMgr::the().uninstall_hooks();
	CHLCommandsDetourMgr::the().uninstall_hooks();
}

void COxWare::remove_inverted_function_table()
{
	auto& p = m_ifp->m_RtlRIFT_pattern;

	CBytePatternRuntime RtlRIFT_pattern(p.bytepattern, p.length);

	auto [base, end] = g_libloader_i->get_loaded_dll_address_space(L"ntdll.dll", SPACE_CODE);

	auto pfnRtlRIFT = (pfnRtlRemoveInvertedFunctionTable_t)RtlRIFT_pattern.search_in_loaded_address_space(base, end);
	if (!pfnRtlRIFT)
	{
		CMessageBox::display_warning("Couldn't find RtlRemoveInvertedFunctionTable in ntdll.dll.\n"
									 "If you will experience random crashes after re-injecting the cheat, it's very likely that its due to this issue.\n"
									 "However, the cheat can in most situations operate just fine, even with this error.");
		return;
	}

	pfnRtlRIFT(g_cheat_dll_base);

	CConsole::the().info("Removed inverted function table.");
}

void COxWare::check_for_clientside_protectors()
{
	// https://github.com/2010kohtep/CSXGuard
	bool is_xguard = g_libloader_i->is_dll_loaded(L"xguard.dll");
	if (is_xguard)
	{
		CMessageBox::display_warning("You are using xguard. Be aware that this cheat isn't compatible with xguard or any similar software."
									 " This means that some parts of this cheat may not work properly or worse, can even crash your game.");
		return;
	}

	// Counter Strike Protector by 'quckly'
	bool is_cs_protector = g_libloader_i->is_dll_loaded(L"client_save.dll");
	if (is_cs_protector)
	{
		CMessageBox::display_warning("You are using CS protector. Be aware that this cheat isn't compatible with xguard or any similar software."
									 " This means that some parts of this cheat may not work properly or worse, can even crash your game.");
	}
}

bool COxWare::is_valid_game()
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

bool COxWare::is_hardware()
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

bool COxWare::validate_engine_build()
{
	m_gs_build_number = CGameUtil::the().get_build_number();

	if (!m_gs_build_number)
	{
		CInjectedDllIPCLayerClient::the().report_error("Couldn't get the engine build number at all! You must be using corrupted or ancient version of cs!");
		return false;
	}

	CConsole::the().info("Your build number is {}.", m_gs_build_number);
	return true;
}

bool COxWare::check_for_encrypted_modules()
{
	// modules that we need to decrypt, in order for the hooks to work..
	static const std::string s_possibly_encrypted_modules[] =
	{
		"hw.dll", "cstrike\\cl_dlls\\client.dll"
	};

	// see if these dlls are encrypted and add them into the list
	for (const auto& file : s_possibly_encrypted_modules)
	{
		std::filesystem::path full_path = g_filesystem_i->locate_halflife_dir() / file;
		std::string s_full_path = CStringTools::the().utf16_to_utf8(full_path.wstring());

		if (CGSDecrypt::the().is_dll_encrypted(s_full_path))
		{
			m_is_running_encrypted_game = true;

			CConsole::the().info("{} is encrypted.", file);

			std::wstring w_filename = full_path.filename().wstring();
			
			std::ostringstream os;

			reconstructed_blob_module_info_t info;
			if (CGSDecrypt::the().decrypt_dll(s_full_path, info))
			{
				g_libloader_i->register_encrypted_module(w_filename.c_str(), info);
			}
			else
			{
				CInjectedDllIPCLayerClient::the().report_error("Fatal error, couldn't decrypt encrypted DLL: {}. "
															   "See console for more information.", full_path);
				return false;
			}
		}
		else
		{
			CConsole::the().info("{} is not encrypted.", file);
		}
	}

	return true;
}

//---------------------------------------------------------------------

void CEngineSynchronization::put_engine_in_sleep()
{
	if (!CMemoryFnDetourMgr::the()._Host_Frame().is_installed())
	{
		CConsole::the().warning("Cannot put engine in sleep, because _Host_Frame detour isn't installed.");
		return;
	}

	m_engine_should_sleep = true;

	CConsole::the().info("Putting engine to sleep, waiting for it to ack...");

	// now hang till engine acks that it is sleeping.
	hang_till_engine_sleeps();

	// ok, engine is now sleeping, continue.
	// DONT FORGET TO RESUME ENGINE FROM SLEEPING!
}

void CEngineSynchronization::hang_till_engine_sleeps()
{
	int n = 0;
	
	//
	// time since engine starts a new frame, we need to handle a case where we're in some kind
	// of deadlock or something. in that case we'd hang forever.
	// let's say wait at max 20 seconds... 20 seconds long frame should be enough.. :^)
	//
	uint32_t start_time = GetTickCount();

	while (m_engine_is_sleeping == false)
	{
		// do nothing until engine acks

		std::this_thread::sleep_for(10ms);

		if (!m_engine_is_sleeping && (n % 20 == 0))
		{
			CConsole::the().info("Waiting for engine...{}x", (n++ / 20) + 1);
		}

		if (GetTickCount() - start_time > k_time_to_acknowledge_sleep)
		{
			CConsole::the().info("Failed to put engine in sleep! Waited {} seconds", k_time_to_acknowledge_sleep / 1000);
			return;
		}
	}

	// OK! engine ack'd that it is sleeping.
	CConsole::the().info("Engine went to sleep. Continuing...");
	CConsole::the().info("Took {} ms to put engine to sleep.", GetTickCount() - start_time);
}

void CEngineSynchronization::resume_engine()
{
	CConsole::the().info("Resuming engine from sleep...");
	m_engine_should_sleep = false;

	// wait for the engine to ack that it's not sleeping anymore
	hang_till_engine_resumes_from_sleep();
}

void CEngineSynchronization::hang_till_engine_resumes_from_sleep()
{
	int n = 0;

	//
	// at this point, the engine is inside an infinite loop hanging, waiting for us
	// to resume it from "sleeping". Tell it to stop.
	//
	uint32_t start_time = GetTickCount();

	while (m_engine_is_sleeping)
	{
		// do nothing until engine acks

		std::this_thread::sleep_for(10ms);

		if (m_engine_is_sleeping && (n % 20 == 0))
		{
			CConsole::the().info("Waiting for engine...{}x", (n++ / 20) + 1);
		}

		if (GetTickCount() - start_time > k_time_to_acknowledge_sleep)
		{
			CConsole::the().info("Failed to resume engine from sleep! Waited {} seconds", k_time_to_acknowledge_sleep / 1000);
			return;
		}
	}

	CConsole::the().info("Engine is not sleeping anymore, continuing...");
	CConsole::the().info("Took {} ms to resume engine from sleep.", GetTickCount() - start_time);
}

bool CEngineSynchronization::engine_frame()
{
	if (m_engine_should_sleep)
	{
		CConsole::the().info("Engine frame: Putting engine to sleep");
	}

	while (m_engine_should_sleep)
	{
		// hang till we let engine be free again

		m_engine_is_sleeping = true;

		// NOTE: there's a bug where when in retail mode + using manual mapping + having the game fullscreen + unloading the cheat from the menu :D
		//		 after calling "surfacefuncs->SetCursor" inside "CEngineInput::surface_set_cursor" that is called inside "COxWareUI::destroy", the
		//		 game hangs, presumably because the SetCursor engine function calls SDL_SetWindowGrab() however, since we hang inside the engine loop
		//		 SDL_PumpEvents() doesn't get ever called, and therefore the entire game freezes. So we call it here explicitly, and it will get fixed..
		//		 holy fuck... xD (clownbug)
		if (COxWare::the().get_build_number() >= FIRST_SDL_BUILD)
		{
			(void(__cdecl*)())GetProcAddress(GetModuleHandleA("SDL2.dll"), "SDL_PumpEvents")();
		}
		
		// don't flood the cpu scheduler with this thread only
		std::this_thread::sleep_for(10ms);
	}

	if (m_engine_is_sleeping)
	{
		CConsole::the().info("Engine frame: Engine exited from sleep.");
		m_engine_is_sleeping = false;
		return false;
	}

	m_engine_is_sleeping = false;
	return true;
}
