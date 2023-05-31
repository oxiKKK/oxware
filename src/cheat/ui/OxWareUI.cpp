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

VarBoolean ui_background_rain("ui_background_rain", "Toggles on/off the background rain when UI is up", true);
VarBoolean ui_background_fade("ui_background_fade", "Toggles on/off the background fade animation when UI is up", true);
VarBoolean ui_render_feature_list("ui_render_feature_list", "Toggles on/off rendering of the feature list", true);

BaseCommand ui_toggle_menu(
	"ui_toggle_menu", "Toggles menu on/off",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		COxWareUI::the().find_context("menu")->toggle_rendering();
	}
);

BaseCommand ui_toggle_console(
	"ui_toggle_console", "Toggles console on/off",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		COxWareUI::the().find_context("console")->toggle_rendering();
	}
);

#ifdef OX_ENABLE_CODE_PROFILE
BaseCommand ui_toggle_perf_profiler_visualization(
	"ui_toggle_perf_profiler_visualization", "Toggles performance profiler visualization on/off",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		COxWareUI::the().find_context("performace_profiler_visualization")->toggle_rendering();
	}
);
#endif

void COxWareUI::swapbuffers_detour(HDC hdc)
{
	if (CoXWARE::the().is_cheat_exiting())
	{
		return; // this is an external func, so we have to handle it like this.
	}

	m_hdc = hdc;

	if (!m_initialized)
	{
		initialize(WindowFromDC(hdc));

		m_initialized = true;
	}

	run_ui();
}

void COxWareUI::destroy()
{
	if (!m_cheat_context || !m_original_context)
	{
		return; // haven't initialized yet
	}

	CConsole::the().info("Destroying cheat UI...");

	// if we're exiting/restarting from the game, we don't have to worry about this at all.
	// NOTE: This is in order to fix a occasional crash that would happen where the gameUI 
	//		 interface would become dangling... (non-nullptr, but invalid memory) so this fixes that.
	if (!CoXWARE::the().is_game_exiting_or_restarting())
	{
		CEngineInput::the().toggle_ingame_clientdll_mouse(true); // call this before the restoration, so that we eliminate bugs when not in relative mode.
		CEngineInput::the().toggle_ingame_input(true);
	}

	destroy_rendering_contexts();

	g_window_msg_handler_i->destroy();
	g_user_input_i->destroy();

	g_imgui_platform_layer_i->destroy_layer();

	m_initialized = false;
}

IRenderingContext* COxWareUI::find_context(const std::string& id) const
{
	for (auto ctx : m_rendering_contexts)
	{
		if (id == ctx->get_id())
			return ctx;
	}
	
	assert(0);
	return nullptr;
}

void COxWareUI::schedule_popup(const std::string& window_title, const Vector2D& window_size, 
							   const std::function<void()>& contents, const std::function<void()>& on_close_callback, 
							   ImGuiWindowFlags window_flags)
{
	m_popup_callback = contents;
	m_on_close_callback = on_close_callback;
	m_popup_window_size = window_size;
	m_popup_window_title = window_title;
	m_popup_window_flags = window_flags;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void COxWareUI::initialize(HWND wnd)
{
	m_hwnd = wnd;

	// window proc message handler
	g_window_msg_handler_i->initialize(wnd);

	if (!g_imgui_platform_layer_i->create_new_layer(IMPLATFORM_win32, wnd, [this]() { render_imgui(); }, ImGuiConfigFlags_NoMouseCursorChange))
	{
		CoXWARE::the().end_cheat_execution();
		return;
	}

	// add imgui wndproc
	g_window_msg_handler_i->add_msg_return_callback(
		[this](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* lpReturnVal)
		{
			if (m_is_any_interactible_rendering_context_active)
			{
				if (msg == WM_SETCURSOR)
				{
					// this message is fired when the mouse is moved across the window (unless another window 
					// has captured the mouse). In order to properly set the mouse cursor when the UI is up, we 
					// need to set it each time the mouse is moved (i.e. when this event is invoked), because 
					// otherwise the mouse would be reseted.
					//
					// for more information, refer to following:
					// https://learn.microsoft.com/en-us/windows/win32/learnwin32/setting-the-cursor-image

					// HTCLIENT informs that the cursor is over client area of the window.
					if (LOWORD(lParam) == HTCLIENT)
					{
						// if we don't return true here, the event will be passed further along to the default window procedure.
						::SetCursor(nullptr);
						*lpReturnVal = TRUE;
					}
					else
					{
						// the cursor is outisde of the window area, let windows handle it.
						*lpReturnVal = FALSE;
					}

					return WNDPROCRET_Custom; // return *lpReturnVal
				}

				if (g_imgui_platform_layer_i->imgui_wndproc_handler(hWnd, msg, wParam, lParam))
				{
					*lpReturnVal = TRUE;
					return WNDPROCRET_Custom; // return *lpReturnVal
				}
			}

			return WNDPROCRET_Generic; // return CallWindowProcA
		});

	instantiate_rendering_contexts();

	// add default keys
	g_user_input_i->add_key_press_callback(
		"toggle_menu", VK_INSERT, 
		[](const std::string& id, UserKey_t* key)
		{
			COxWareUI::the().find_context("menu")->toggle_rendering();
		});
	g_user_input_i->add_key_press_callback(
		"toggle_console", VK_OEM_3, 
		[](const std::string& id, UserKey_t* key)
		{
			COxWareUI::the().find_context("console")->toggle_rendering();
		});
#ifdef OX_ENABLE_CODE_PROFILE
	g_user_input_i->add_key_press_callback(
		"toggle_perf_profiler_visualization", VK_F11, 
		[](const std::string& id, UserKey_t* key)
		{
			COxWareUI::the().find_context("performace_profiler_visualization")->toggle_rendering();
		});
#endif

	m_original_context = wglGetCurrentContext();
	m_cheat_context = wglCreateContext(m_hdc);

	CEngineInput::the().initialize();

	CESP::the().initialize_gui();
	CFlashbangFadeModifier::the().initialize_gui();
	CBulletTrace::the().initialize_gui();

	CConsole::the().info("Cheat UI fully initialized.");
}

void COxWareUI::run_ui()
{
	m_is_any_interactible_rendering_context_active = false;
	m_contexts_to_be_rendered.clear();

	if (is_in_popup_dialog())
	{
		lock_interaction_on_all_contexts();
	}

	for (IRenderingContext* ctx : m_rendering_contexts)
	{
		if (ctx->should_render())
		{
			m_contexts_to_be_rendered.push_back(ctx);

			// we need at least one interactible context to affect ingame mouse events
			if (ctx->is_interactible() && !m_is_any_interactible_rendering_context_active)
			{
				m_is_any_interactible_rendering_context_active = true;
			}
		}
	}

	// welcoming popup dialog
	static bool once = false;
	if (!once && (CoXWARE::the().at_least_once_focused() || GetFocus() == m_hwnd))
	{
		bool show_welcome_popup = g_registry_i->read_int(REG_OXWARE, "already_shown_welcome_popup") == 0;
		if (show_welcome_popup)
		{
			create_welcome_popup();
		}
	
		once = true;
	}

	// TODO: Create some kind of unique interface for this...
	g_bindmgr_i->set_ui_running(m_is_any_interactible_rendering_context_active);
	g_bindmgr_i->set_game_ui_running(CEngineInput::the().is_gameui_rendering());
	g_in_commands_i->set_ui_running(m_is_any_interactible_rendering_context_active || CEngineInput::the().is_gameui_rendering());

	if (!m_contexts_to_be_rendered.empty() || (m_popup_callback != nullptr))
	{
		// push new
		wglMakeCurrent(m_hdc, m_cheat_context);

		if (g_imgui_platform_layer_i)
		{
			g_imgui_platform_layer_i->render();
		}

		// restore old
		wglMakeCurrent(m_hdc, m_original_context);
	}

	post_render();
}

void COxWareUI::render_imgui()
{
	g_gui_widgets_i->block_input_on_all_except_popup(is_in_popup_dialog() || g_user_input_i->is_in_key_binding_mode());

	for (IRenderingContext* ctx : m_contexts_to_be_rendered)
	{
		ctx->on_render();
	}

	if (is_in_popup_dialog())
	{
		g_gui_widgets_i->push_executing_popup_code();

		render_popup();

		g_gui_widgets_i->pop_executing_popup_code();
	}

	if (g_user_input_i->is_in_key_binding_mode())
	{
		// if in key scanning mode, we don't want to render the cursor
		g_imgui_platform_layer_i->override_cursor(GUICURSOR_None);
	}

	g_imgui_platform_layer_i->should_render_imgui_cursor(m_is_any_interactible_rendering_context_active);
}

void COxWareUI::post_render()
{
	handle_ingame_mouseevents();
}

void COxWareUI::create_welcome_popup()
{
	schedule_popup(
		"About", Vector2D(350, 420),
		[this]()
		{
			g_gui_widgets_i->add_separtor_with_text("Introduction");
			g_gui_widgets_i->add_text("Welcome to oxWARE " OXVER_STRING "! A public open-source cheat for CS 1.6.", TEXTPROP_Wrapped);
			g_gui_widgets_i->add_spacing();
			g_gui_widgets_i->add_text("Please, note that this cheat is still in development! It's nowhere near finished!", TEXTPROP_Wrapped);

			g_gui_widgets_i->add_spacing();

			g_gui_widgets_i->add_text("Press INSERT to begin the cheating experience! :) Or simply re-bind the key to whatever you like!", TEXTPROP_Wrapped);

			float sections_padding = 10.0f;

			g_gui_widgets_i->add_padding({ 0.0f, sections_padding });
			g_gui_widgets_i->add_separtor_with_text("Project GitHub repository");

			g_gui_widgets_i->add_text("Please, don't forget to visit the project repository on GitHub! You can find there information such as:", TEXTPROP_Wrapped);
			g_gui_widgets_i->add_bullet_text("TODO list");
			g_gui_widgets_i->add_bullet_text("ChangeLog for each version");
			g_gui_widgets_i->add_bullet_text("Feature list");
			g_gui_widgets_i->add_bullet_text("Non-steam support");
			g_gui_widgets_i->add_bullet_text("And more!");

			g_gui_widgets_i->add_spacing();
			if (g_gui_widgets_i->add_hypertext_link("github.com/oxiKKK/oxware"))
			{
				CGenericUtil::the().open_link_inside_browser("https://github.com/oxiKKK/oxware");
			}

			g_gui_widgets_i->add_padding({ 0.0f, sections_padding });
			g_gui_widgets_i->add_separtor_with_text("Availability");

			g_gui_widgets_i->add_text(std::format("The cheat is currently available on following builds: {}", g_bytepattern_bank_i->supported_builds_as_str()), TEXTPROP_Wrapped);

			g_gui_widgets_i->add_padding({ 0.0f, sections_padding });
			g_gui_widgets_i->add_separtor_with_text("Bugs / feature requests");
			g_gui_widgets_i->add_text("If you find any bugs or you want something to be added, please, create an issue here:", TEXTPROP_Wrapped);
			if (g_gui_widgets_i->add_hypertext_link("github.com/oxiKKK/oxware/issues"))
			{
				CGenericUtil::the().open_link_inside_browser("https://github.com/oxiKKK/oxware/issues");
			}

			g_gui_widgets_i->add_spacing();
			g_gui_widgets_i->add_text("Or just simply contact me anywhere! ;)", TEXTPROP_Wrapped);

			g_gui_widgets_i->add_padding({ 0.0f, sections_padding });
			g_gui_widgets_i->add_separtor_with_text("Contact on the author");

			if (g_gui_widgets_i->begin_columns("contact", 2))
			{
				g_gui_widgets_i->setup_column_fixed_width(90);
				g_gui_widgets_i->goto_next_column();

				g_gui_widgets_i->add_text("Discord");
				g_gui_widgets_i->goto_next_column();
				g_gui_widgets_i->add_text("__oxiKKK#5015");
				g_gui_widgets_i->goto_next_column();

				g_gui_widgets_i->add_text("Steam");
				g_gui_widgets_i->goto_next_column();
				if (g_gui_widgets_i->add_hypertext_link("steamcommunity.com/id/oxiKKK"))
				{
					CGenericUtil::the().open_link_inside_browser("https://steamcommunity.com/id/oxiKKK");
				}
				g_gui_widgets_i->goto_next_column();

				g_gui_widgets_i->add_text("YouTube");
				g_gui_widgets_i->goto_next_column();
				if (g_gui_widgets_i->add_hypertext_link("www.youtube.com/@OX666"))
				{
					CGenericUtil::the().open_link_inside_browser("https://steamcommunity.com/id/oxiKKK");
				}
				g_gui_widgets_i->goto_next_column();

				g_gui_widgets_i->add_text("GitHub");
				g_gui_widgets_i->goto_next_column();
				if (g_gui_widgets_i->add_hypertext_link("github.com/oxiKKK"))
				{
					CGenericUtil::the().open_link_inside_browser("https://github.com/oxiKKK");
				}

				g_gui_widgets_i->end_columns();
			}

			g_gui_widgets_i->add_padding({ 0.0f, 20.0f });

			m_is_any_interactible_rendering_context_active = true;
		},
		[]() // on close
		{
			g_registry_i->write_int(REG_OXWARE, "already_shown_welcome_popup", 1);
		});
}

void COxWareUI::instantiate_rendering_contexts()
{
	// just little bit of speedup, since we're using a vector of pointers, allocate them in one memory region.
	m_rendering_contexts.reserve(64);

	instantiate_single_rendering_context(&ctx_Menu, new CUIMenu("menu"));
	instantiate_single_rendering_context(&ctx_FeatureList, new CUIFeatureList("feature_list", ctx_Menu));
	instantiate_single_rendering_context(&ctx_Console, new CUIConsole("console", ctx_Menu));
#ifdef OX_ENABLE_CODE_PROFILE
	instantiate_single_rendering_context(&ctx_PerfProfilerVisualization, new CUIPerfProfilerVisualization("performace_profiler_visualization", ctx_Menu));
#endif
	instantiate_single_rendering_context(&ctx_MenuBackground, new CUIMenuBackground("menu_background", ctx_Menu, 250ms));

	instantiate_single_rendering_context(&ctx_BackgroundRendering, new CUIBackgroundRendering("background_rendering"));

	// explicitly set non-interactible contexts
	ctx_FeatureList->set_not_interactible();
	ctx_MenuBackground->set_not_interactible();
	ctx_BackgroundRendering->set_not_interactible();
}

void COxWareUI::destroy_rendering_contexts()
{
	for (IRenderingContext* ctx : m_rendering_contexts)
	{
		CConsole::the().info("Destroying {} rendering context...", ctx->get_id());
		ctx->on_destroy();
		delete ctx;
	}

	m_rendering_contexts.clear();
}

void COxWareUI::handle_ingame_mouseevents()
{
	// fucking GoldSrc's input handling

	static bool last = false;
	if (last != m_is_any_interactible_rendering_context_active)
	{
		// clear all movement/shooting that we did before we have opened the menu
		if (CGameUtil::the().is_fully_connected())
		{
			CGameUtil::the().reset_all_in_states();
			CMemoryFnHookMgr::the().ClearIOStates().call();
		}

		// center the mouse position, for convenience - so that there's no mouse delta after we toggle UI.
		CEngineInput::the().surface_centerize_cursor_position();

		if (m_is_any_interactible_rendering_context_active)
		{
			CEngineInput::the().toggle_ingame_input(false);
		}
		else
		{
			CEngineInput::the().toggle_ingame_input(true);
		}

		last = m_is_any_interactible_rendering_context_active;
	}

	// to prevent some stupid bugs from leaving the mouse not active even tho it's supposed to be..
	if (CGameUtil::the().is_fully_connected())
	{
		CEngineInput::the().toggle_ingame_clientdll_mouse(!m_is_any_interactible_rendering_context_active);
	}

#if 0
	// call this function for nothing before the engine. It seems that this resets the 
	// delta internally somehow inside of SDL. I know that this is pretty diddly and wacky,
	// but it works...
	int deltaX, deltaY;
	CEngineInput::the().SDL_GetRelativeMouseState(&deltaX, &deltaY);

	CConsole::the().info("delta: relative: {} | [{}, {}]", (bool)CEngineInput::the().SDL_GetRelativeMouseMode(), deltaX, deltaY);
#endif
}

void COxWareUI::lock_interaction_on_all_contexts()
{
	for (auto context : m_rendering_contexts)
	{
		context->lock_interaction();
	}
}

void COxWareUI::unlock_interaction_on_all_contexts()
{
	for (auto context : m_rendering_contexts)
	{
		context->unlock_interaction();
	}
}
void COxWareUI::render_popup()
{
	if (m_popup_close_requested)
	{
		m_popup_close_requested = false;
		on_popup_close();
		return;
	}

	static constexpr auto window_flags_bg =
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs;

	g_gui_widgets_i->set_next_window_pos({}, ImGuiCond_Always);
	g_gui_widgets_i->set_next_window_size(g_imgui_platform_layer_i->get_screen_size(), ImGuiCond_Always);

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, Vector2D());

	g_gui_widgets_i->create_new_window(
		"popup_background", window_flags_bg,
		[this]()
		{
			g_gui_window_rendering_i->render_box(
				g_gui_window_rendering_i->get_current_drawlist(),
				{ 0, 0 },
				g_imgui_platform_layer_i->get_screen_size(),
				{ 0.0f, 0.0f, 0.0f, 0.5f });
		});

	g_gui_widgets_i->pop_stylevar();

	g_gui_widgets_i->center_next_window_pos(ImGuiCond_Always);
	g_gui_widgets_i->set_next_window_rounding(CMenuStyle::k_rounding_factor, ImDrawFlags_RoundCornersTopRight | ImDrawFlags_RoundCornersBottomLeft);
	g_gui_widgets_i->set_next_window_size(m_popup_window_size, ImGuiCond_Appearing);

	g_gui_thememgr_i->push_color(GUICLR_ChildBackground, CColor());

	int window_flags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

	window_flags |= m_popup_window_flags;

	g_gui_widgets_i->create_new_window(
		"popup_window", window_flags, 
		[this]()
		{
			if (!m_popup_window_title.empty())
			{
				g_gui_widgets_i->add_text(m_popup_window_title, TEXTPROP_None, g_gui_fontmgr_i->get_font("segoeui", FONT_BIGGEST, FONTDEC_Regular));
			}

			g_gui_widgets_i->add_child(
				"popup_window_child", 
				Vector2D(-1, -1 - 20), true, ImGuiWindowFlags_None, 
				[&]()
				{
					m_popup_callback();
				});
			
			g_gui_widgets_i->add_spacing();

			if (g_gui_widgets_i->begin_columns("popup_window_column", 2))
			{
				g_gui_widgets_i->setup_column_fixed_width(g_gui_widgets_i->get_current_window_size().x / 1.7f);
				g_gui_widgets_i->goto_next_column();

				// *empty*

				g_gui_widgets_i->goto_next_column();

				if (g_gui_widgets_i->add_button("OK", { -1.0f, 0.0f }, false, BUTTONFLAG_CenterLabel))
				{
					on_popup_close();
				}

				g_gui_widgets_i->end_columns();
			}
		});

	g_gui_thememgr_i->pop_color();
}

void COxWareUI::on_popup_close()
{
	m_popup_callback = nullptr;

	if (m_on_close_callback)
	{
		m_on_close_callback();
	}

	unlock_interaction_on_all_contexts();
}
