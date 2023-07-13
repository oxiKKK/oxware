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

void CLoaderUI::render_contents()
{
	static bool inited = false;
	if (!inited)
	{
		initialize();
		inited = true;
	}

	g_gui_widgets_i->begin_tab("loader", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll);

	g_gui_widgets_i->add_tab_item("main", true, { 0, 0 }, [this]() { render_tab_main(); });
	g_gui_widgets_i->add_tab_item("console", true, { 0, 0 }, [this]() { render_tab_console(); }, { 8.0f, 2.0f });
#ifndef _RETAIL
	g_gui_widgets_i->add_tab_item("debug", true, { 0, 0 }, [this]() { render_tab_debug(); }, { 8.0f, 8.0f });
#endif

	g_gui_widgets_i->end_tab();
}

void CLoaderUI::initialize()
{
	auto on_injector_event_fn = [this](EInjectorEvent ev, IInjectableModuleObject* obj)
	{
		if (obj->get_dll_filepath().string() != MODULE_CHEAT)
		{
			// filter only the cheater module
			return;
		}

		switch (ev)
		{
			case INE_ModuleCommunicationHalt:
			case INE_ModuleFailedInitialization:
			case INE_ModuleUnloading:
			{
				m_allow_to_load_again = true;
				break;
			}
			case INE_ModuleRestarting:
			case INE_ModuleSuccessfulInitialization:
			{
				m_allow_to_load_again = false;
				break;
			}
		}
	};

	CMainLoader::the().get_injector()->provide_on_injector_event_callback(on_injector_event_fn);
}

void CLoaderUI::render_tab_main()
{
	if (g_gui_widgets_i->begin_columns(__FUNCTION__, 2))
	{
		g_gui_widgets_i->goto_next_column();

		auto very_large_font = g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_45px, FDC_Bold);
		g_gui_widgets_i->push_font(very_large_font);

		auto injected_object = CMainLoader::the().get_injector()->get_injected_dll(MODULE_CHEAT);

		auto window_size = g_gui_widgets_i->get_current_window_size();

		if (g_gui_widgets_i->add_button("Start cheat", { -1, window_size.y / 2 - 20 }, !m_allow_to_load_again, BUTTONFLAG_CenterLabel))
		{
			static const char* s_exe_names[] = { "cs.exe", "hl.exe", "cstrike.exe" };

			// try different names
			bool success = false;
			for (int i = 0; i < OX_ARRAYSIZE(s_exe_names); i++)
			{
				if (CMainLoader::the().get_injector()->inject_to_target_process(s_exe_names[i], MODULE_CHEAT))
				{
					success = true;
					break;
				}
			}

			if (!success)
			{
				CMessageBox::display_error("Failed to inject cheat into the game. For detailed information, see the console.");
			}
		}

		injected_object = CMainLoader::the().get_injector()->get_injected_dll(MODULE_CHEAT);
		if (!injected_object)
		{
			m_allow_to_load_again = true;
		}

		if (g_gui_widgets_i->add_button("Unload cheat", { -1, window_size.y / 2 - 20 }, m_allow_to_load_again, BUTTONFLAG_CenterLabel))
		{
			injected_object->unload(true);
		}

		g_gui_widgets_i->pop_font();

		g_gui_widgets_i->goto_next_column();

		g_gui_widgets_i->add_child(
			"Status", { -1.0f, -1.0f - 3 }, true, ImGuiWindowFlags_None,
			[&]()
			{
				auto injector = CMainLoader::the().get_injector();
				if (!injector)
					return;

				auto dll = injector->get_injected_dll(MODULE_CHEAT);
				std::pair<std::string, CColor> status = std::make_pair("offline", CColor(170, 170, 170, 170));

				if (dll)
				{
					static size_t clk_counter = 0;
					static size_t clk_clock = 0;
					
					if (dll->is_successfully_injected() && !dll->is_successfully_initialized())
					{
						if (GetTickCount() - clk_clock > 50)
						{
							if (++clk_counter > 12)
								clk_counter = 1;

							clk_clock = GetTickCount();
						}

						switch (clk_counter)
						{
							case 1:  status = std::make_pair("injecting 🕐", CColor(255, 165, 0, 200)); break;
							case 2:  status = std::make_pair("injecting 🕑", CColor(255, 165, 0, 200)); break;
							case 3:  status = std::make_pair("injecting 🕒", CColor(255, 165, 0, 200)); break;
							case 4:  status = std::make_pair("injecting 🕓", CColor(255, 165, 0, 200)); break;
							case 5:  status = std::make_pair("injecting 🕔", CColor(255, 165, 0, 200)); break;
							case 6:  status = std::make_pair("injecting 🕕", CColor(255, 165, 0, 200)); break;
							case 7:  status = std::make_pair("injecting 🕖", CColor(255, 165, 0, 200)); break;
							case 8:  status = std::make_pair("injecting 🕗", CColor(255, 165, 0, 200)); break;
							case 9:  status = std::make_pair("injecting 🕘", CColor(255, 165, 0, 200)); break;
							case 10: status = std::make_pair("injecting 🕙", CColor(255, 165, 0, 200)); break;
							case 11: status = std::make_pair("injecting 🕚", CColor(255, 165, 0, 200)); break;
							case 12: status = std::make_pair("injecting 🕛", CColor(255, 165, 0, 200)); break;
						}
											}
					else if (dll->is_successfully_injected() && dll->is_successfully_initialized())
					{
						status = std::make_pair("online 🔥", CColor(11, 218, 81, 200));

						clk_counter = 0;
						clk_clock = 0;
					}
				}

				g_gui_widgets_i->push_color(ImGuiCol_Text, status.second);
				g_gui_widgets_i->add_window_centered_text(status.first, very_large_font);
				g_gui_widgets_i->pop_color(1);
			});

		g_gui_widgets_i->end_columns();
	}
}

void CLoaderUI::render_tab_console()
{
	g_gui_widgets_i->add_console();
}

void CLoaderUI::render_tab_debug()
{
	if (g_gui_widgets_i->add_tree_node("Fonts"))
	{
		g_gui_fontmgr_i->render_debug_ui();
		
		g_gui_widgets_i->pop_tree_node();
	}
}
