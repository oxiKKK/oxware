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

#include "assets/fonts/compressed_png_data.h"

#include "resource/resource.h"

#if !defined(_DEBUG)
#define INJECTION_METHOD_MANUALMAP
#endif

// UI
IGLFWApp* g_glfw_app_i = nullptr;
IGUIWidgets* g_gui_widgets_i = nullptr;
IGUIFontManager* g_gui_fontmgr_i = nullptr;
IGUIThemeManager* g_gui_thememgr_i = nullptr;
IGUIWindowRendering* g_gui_window_rendering_i = nullptr;

// Util
IRegistry* g_registry_i = nullptr;
IFileSystem* g_filesystem_i = nullptr;
IVariableManager* g_variablemgr_i = nullptr;
IAppDataManager* g_appdata_mgr_i = nullptr;

enum
{
	RET_SUCCESS = 0, 
	RET_FAILURE = 1, 
};

#define DEFAULT_APP_TITLE	"oxWARE Loader"
#define DEFAULT_APP_W		720
#define DEFAULT_APP_H		480

int CMainLoader::run(HINSTANCE hinst)
{
	if (!check_supported_os_version())
	{
		CMessageBox::display_error("You are not running Windows 10 or newer. This cheat is not supported on your OS." 
								   "\n\nYour Windows version is: {}", CGenericUtil::the().get_os_version_str());
		return RET_FAILURE;
	}

	if (is_already_launched())
	{
		CMessageBox::display_error("Already running. Cannot not run multiple instances of the loader.");
		return RET_FAILURE;
	}

	m_instance_handle = hinst;

	CConsole::the().initialize(EOutputModule::LOADER, g_devconsole_i);

	// load modules
	if (!load_and_initialize_dependencies())
	{
		release_handles();
		return RET_FAILURE;
	}

	g_devconsole_i->enable_tooltip(false);

	decide_injection_type();

	on_window_invoke();

	while (run_frame()) {}

	m_selected_injector->destroy();
	delete m_selected_injector;
	m_selected_injector = nullptr;

	on_window_destroy();

	unload_dependencies();

	release_handles();

	return RET_SUCCESS;
}

bool CMainLoader::load_and_initialize_dependencies()
{
	// Util
	if (!CDependencyLoader::the().load_and_initialize_module(LOADTYPE_NATIVE, {}, WMODULE_UTIL, [&](const auto& mod)
	{
		 g_registry_i = mod.get_interface<IRegistry>(IREGISTRY_INTERFACEID);
		 g_filesystem_i = mod.get_interface<IFileSystem>(IFILESYSTEM_INTERFACEID);
		 g_variablemgr_i = mod.get_interface<IVariableManager>(IVARIABLEMANAGER_INTERFACEID);
		 g_appdata_mgr_i = mod.get_interface<IAppDataManager>(IAPPDATAMANAGER_INTERFACEID);

		 return g_registry_i && g_filesystem_i && g_variablemgr_i && g_appdata_mgr_i;
	}))
	{
		return false;
	}

	if (!g_appdata_mgr_i->initialize_directory_hirearchy())
	{
		return false;
	}

	// UI
	if (!CDependencyLoader::the().load_and_initialize_module(LOADTYPE_NATIVE, {}, WMODULE_GUI, [&](const auto& mod)
	{
		g_glfw_app_i = mod.get_interface<IGLFWApp>(IGLFWAPP_INTERFACEID);
		g_gui_widgets_i = mod.get_interface<IGUIWidgets>(IGUIWIDGETS_INTERFACEID);
		g_gui_fontmgr_i = mod.get_interface<IGUIFontManager>(IGUIFONTMANAGER_INTERFACEID);
		g_gui_thememgr_i = mod.get_interface<IGUIThemeManager>(IGUITHEMEMANAGER_INTERFACEID);
		g_gui_window_rendering_i = mod.get_interface<IGUIWindowRendering>(IGUIWINDOWRENDERING_INTERFACEID);

		return g_glfw_app_i && g_gui_widgets_i && g_gui_fontmgr_i && g_gui_thememgr_i && g_gui_window_rendering_i;
	}))
	{
		return false;
	}

	g_gui_thememgr_i->initialize();

	g_variablemgr_i->register_variables_and_commands_per_module(&g_static_variable_container, &g_static_command_container, MODULE_LOADER);

	// after all modules have been loaded
	if (!g_variablemgr_i->initialize())
	{
		return RET_FAILURE;
	}

	return true;
}

bool CMainLoader::on_window_invoke()
{
	if (!g_glfw_app_i->create_window(DEFAULT_APP_W, DEFAULT_APP_H, DEFAULT_APP_TITLE, [this]() { on_render(); }))
	{
		return false;
	}

	set_window_icon();

	CConsole::the().info("Created loader window");

	return true;
}

void CMainLoader::on_window_destroy()
{
	g_glfw_app_i->destroy_window();
}

bool CMainLoader::check_supported_os_version()
{
	auto ver = CGenericUtil::the().get_os_version();

	//
	// https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-osversioninfoexa#remarks
	//

	// windows 10/11:
	if (ver.dwMajorVersion >= 10)
	{
		return true;
	}

	// windows 8/8.1: TODO
	//if (ver.dwMajorVersion >= 6 && ver.dwMinorVersion >= 2)
	//{
	//	return true;
	//}

	return false;
}

void CMainLoader::decide_injection_type()
{	
#ifdef INJECTION_METHOD_MANUALMAP
	CConsole::the().info("The injection type is manual mapping");
	m_selected_injector = new CBaseInjector(INJECT_MANUALMAP);
#else
	CConsole::the().info("The injection type is LoadLibrary");
	m_selected_injector = new CBaseInjector(INJECT_LOADLIBRARY);
#endif
}

bool CMainLoader::run_frame()
{
	frame_start = std::chrono::high_resolution_clock::now();

	if (!g_glfw_app_i->on_tick())
	{
		return false;
	}

	m_selected_injector->update();

	if (!CDependencyLoader::the().update())
	{
		return false;
	}

	frame_end = std::chrono::high_resolution_clock::now();

	std::this_thread::sleep_for(1ms);

	return true;
}

void CMainLoader::set_window_icon()
{
	// we set the icon through winapi, because glfw is just fucking stupid. I have wasted like 4
	// hours trying to resolve this, only because glfw doesn't allow the user (AFAIK) to set both
	// the smaller and bigger icon (for window title and taskbar). I don't know why the fuck not, but
	// whatever. This does it just alright.

	auto load_icon_image = [this](DWORD idi, int w, int h) -> HANDLE
	{
		HANDLE handle = LoadImageA(m_instance_handle, MAKEINTRESOURCE(idi), IMAGE_ICON, w, h, LR_COLOR);
		if (!handle)
		{
			CConsole::the().error("Couldn't set window small icon..({}x{}) (last err: {})", w, h, CGenericUtil::the().get_last_win_error());
			return NULL;
		}

		return handle;
	};

	HANDLE ico_big = load_icon_image(IDI_ICON_BIG, 256, 256);
	HANDLE ico_small = load_icon_image(IDI_ICON_SMALL, 32, 32);

	if (!ico_big || !ico_small)
	{
		return;
	}

	SendMessageA(g_glfw_app_i->get_window_handle(), (UINT)WM_SETICON, ICON_BIG, (LPARAM)ico_big);
	SendMessageA(g_glfw_app_i->get_window_handle(), (UINT)WM_SETICON, ICON_SMALL, (LPARAM)ico_small);
}

bool CMainLoader::is_already_launched()
{
	m_loader_mutex = CreateMutexA(NULL, TRUE, "oxware_launcher_mutex");
	if (CGenericUtil::the().get_last_win_error() == ERROR_ALREADY_EXISTS)
	{
		// already running somewhere
		return true;
	}

	unsigned int waitResult = WaitForSingleObject(m_loader_mutex, NULL);
	if (waitResult != WAIT_OBJECT_0 && waitResult != WAIT_ABANDONED)
	{
		// already running somewhere
		return true;
	}

	return false;
}

void CMainLoader::release_handles()
{
	if (m_loader_mutex != NULL)
	{
		ReleaseMutex(m_loader_mutex); // Explicitly release mutex
		CloseHandle(m_loader_mutex); // close handle before terminating
	}
}

void CMainLoader::unload_dependencies()
{
	g_registry_i->shutdown();

	CConsole::the().destroy();

	CDependencyLoader::the().unload_all();

	// At this point all of the interfaces we obtained are gone.
	g_devconsole_i = nullptr;
	g_glfw_app_i = nullptr;
	g_gui_widgets_i = nullptr;	
	g_gui_fontmgr_i = nullptr;
	g_gui_thememgr_i = nullptr;
	g_gui_window_rendering_i = nullptr;

	// Util
	g_registry_i = nullptr;
	g_filesystem_i = nullptr;
	g_variablemgr_i = nullptr;
	g_appdata_mgr_i = nullptr;
}

void CMainLoader::on_render()
{
	g_gui_widgets_i->set_next_window_pos({}, ImGuiCond_Always);
	g_gui_widgets_i->set_next_window_size(g_glfw_app_i->get_app_resolution(), ImGuiCond_Always);

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
			CLoaderUI::the().render_contents();
		});
}
