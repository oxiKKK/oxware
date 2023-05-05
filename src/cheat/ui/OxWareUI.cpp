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

void COxWareUI::swapbuffers_detour(HDC hdc)
{
	if (CoXWARE::the().is_cheat_exiting())
	{
		return; // this is an external func, so we have to handle it like this.
	}

	m_hdc = hdc;

	static bool gui_initialized = false;
	if (!gui_initialized)
	{
		initialize(WindowFromDC(hdc));

		gui_initialized = true;
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

	// don't left mouse deactivated when the UI is up and we're unloading the cheat.
	auto surfacefuncs = CHLInterfaceHook::the().ISurface();
	if (CGameUtil::the().is_fully_connected())
	{
		CMemoryHookMgr::the().cl_funcs().get()->pfnHUD_IN_ActivateMouse();

		// there was a bug where if we were ingame and game ui wasn't up and we unloaded the cheat through menu, the mouse activation would not get executed.
		// cursor drawing is handled differently on these builds...
		if (surfacefuncs && CoXWARE::the().get_build_number() < FIRST_SDL_BUILD)
		{
			if (CGameUtil::the().is_fully_connected())
			{
				surfacefuncs->SetCursor(hl::vgui2::dc_none);
			}
		}
	}
	else if (!CGameUtil::the().is_fully_connected())
	{
		if (surfacefuncs)
		{
			surfacefuncs->UnlockCursor();
			surfacefuncs->SetCursor(hl::vgui2::dc_arrow);
		}
	}

	destroy_rendering_contexts();

	g_window_msg_handler_i->destroy();
	g_user_input_i->destroy();

	g_imgui_platform_layer_i->destroy_layer();
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
	// window proc message handler
	g_window_msg_handler_i->initialize(wnd);

	// keyboard/mouse I/O
	g_user_input_i->initialize();

	if (!g_imgui_platform_layer_i->create_new_layer(IMPLATFORM_win32, wnd, [this]() { render_imgui(); }))
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
				if (g_imgui_platform_layer_i->imgui_wndproc_handler(hWnd, msg, wParam, lParam))
				{
					*lpReturnVal = TRUE;
					return WNDPROCRET_Custom; // return *lpReturnVal
				}
			}

			return WNDPROCRET_Generic; // return CallWindowProcA
		});

	instantiate_rendering_contexts();

	m_original_context = wglGetCurrentContext();
	m_cheat_context = wglCreateContext(m_hdc);

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
	if (!once)
	{
		bool already_have_launched = g_registry_i->read_int(REG_OXWARE, "already_launched") == 1;
		if (!already_have_launched)
		{
			create_welcome_popup();
		}

		once = true;
	}

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
	g_gui_widgets_i->block_input_on_all_except_popup(is_in_popup_dialog());

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
}

void COxWareUI::post_render()
{
	g_imgui_platform_layer_i->should_render_imgui_cursor(m_is_any_interactible_rendering_context_active);

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

			g_gui_widgets_i->add_text("Press F1 to begin the cheating experience! :)", TEXTPROP_Wrapped);

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
			g_registry_i->write_int(REG_OXWARE, "already_launched", 1);
		});
}

void COxWareUI::instantiate_rendering_contexts()
{
	// just little bit of speedup, since we're using a vector of pointers, allocate them in one memory region.
	m_rendering_contexts.reserve(64);

	instantiate_single_rendering_context(&ctx_Menu, new CUIMenu("menu", VK_F1));
	instantiate_single_rendering_context(&ctx_FeatureList, new CUIFeatureList("feature_list", ctx_Menu));
	instantiate_single_rendering_context(&ctx_Console, new CUIConsole("console", VK_OEM_3, ctx_Menu));
#ifdef OX_ENABLE_CODE_PROFILE
	instantiate_single_rendering_context(&ctx_PerfProfilerVisualization, new CUIPerfProfilerVisualization("performace_profiler_visualization", VK_F2, ctx_Menu));
#endif
	instantiate_single_rendering_context(&ctx_MenuBackground, new CUIMenuBackground("menu_background", ctx_Menu));

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
}

// define this in order to debug the mouse event code below.
//#define MOUSEINPUT_DEBUG

void COxWareUI::handle_ingame_mouseevents()
{
	// fucking GoldSrc's input handling

	static int(__cdecl*SDL_SetRelativeMouseMode)(int enabled);
	static unsigned(__cdecl* SDL_GetRelativeMouseState)(int* x, int* y);
	if (CoXWARE::the().get_build_number() >= FIRST_SDL_BUILD)
	{
		if (!SDL_SetRelativeMouseMode || !SDL_GetRelativeMouseState)
		{
			SDL_SetRelativeMouseMode = (decltype(SDL_SetRelativeMouseMode))GetProcAddress(GetModuleHandleA("SDL2.dll"), "SDL_SetRelativeMouseMode");
			SDL_GetRelativeMouseState = (decltype(SDL_GetRelativeMouseState))GetProcAddress(GetModuleHandleA("SDL2.dll"), "SDL_GetRelativeMouseState");
		}
	}

	static bool last = false;
	if (last != m_is_any_interactible_rendering_context_active)
	{
		// can happen that this gets called before the interface hook is initialized, hence it can crash.
		// therefore, we need to check for every hook if it's installed, before we can use it.
		auto gameuifuncs = CHLInterfaceHook::the().IGameUI();
		auto surfacefuncs = CHLInterfaceHook::the().ISurface();
		auto& enginefuncs_hook = CMemoryHookMgr::the().cl_enginefuncs();

		// vgui2::ISurface::SurfaceSetCursorPos function just calls SDL_WarpMouseInWindow.
		int wide = 0, tall = 0;
		if (surfacefuncs)
		{
			surfacefuncs->GetScreenSize(wide, tall);
			surfacefuncs->SurfaceSetCursorPos(wide / 2, tall / 2);
#ifdef MOUSEINPUT_DEBUG
			CConsole::the().info("center: [{}, {}]", wide / 2, tall / 2);
#endif
		}

		// unlock the cursor, just in case it's locked (this would prevent SetCursor from executing)
		surfacefuncs->UnlockCursor();

		// disable in-game crosshair (handled by vgui2)
		if (m_is_any_interactible_rendering_context_active)
		{
			if (surfacefuncs && 
				gameuifuncs && gameuifuncs->IsGameUIActive())
			{
				surfacefuncs->SetCursor(hl::vgui2::dc_none);
			}
		}
		else
		{
			if (surfacefuncs && 
				gameuifuncs)
			{
				if (gameuifuncs->IsGameUIActive())
				{
					surfacefuncs->SetCursor(hl::vgui2::dc_arrow);
				}
				else
				{
					if (CoXWARE::the().get_build_number() < FIRST_SDL_BUILD)
					{
						// There's an issue where if we're in-game, with relative mode on, on non-sdl engine build, 
						// and we close the UI (menu), the cursor doesn't disseapear. (in windowed mode, on 4554 for example)
						if (CGameUtil::the().is_fully_connected())
						{
							surfacefuncs->SetCursor(hl::vgui2::dc_none);
						}
					}
				}
			}
		}

		// clear all movement/shooting that we did before we have opened the menu
		if (CGameUtil::the().is_fully_connected())
		{
			CGameUtil::the().reset_all_in_states();
			CMemoryFnHookMgr::the().ClearIOStates().call();
		}

		if (CoXWARE::the().get_build_number() >= FIRST_SDL_BUILD)
		{
			// This fixes many mouse cursor stuck issues. For more information on this, see comments below.
			// 
			// Issue #0001:
			//	When raw input was on (m_rawinput 1), in game, the client dll input code called SDL_GetRelativeMouseState
			//	to get the mouse delta. However, with our menu and the way we handle this whole input situation, it failed
			//	to do so, because somehwere inside the engine code (presumably in BaseUISurface::SetCursor) the engine called
			//	SDL_SetRelativeMouseMode with bad state. Therefore, the function wasn't executed, because the relative mode
			//	wasn't "on", having the mouse cursor stuck, until the function (BaseUISurface::SetCursor) weren't ran again
			//	(by calling CBaseUI::ActivateGameUI)..
			if (gameuifuncs)
			{
				// all of this is caused when m_rawinput is on.
				if (enginefuncs_hook.is_installed())
				{
					auto m_rawinput = enginefuncs_hook.get()->pfnGetCvarPointer((char*)"m_rawinput");
					if (m_rawinput && m_rawinput->value != 0.0f)
					{
						if (gameuifuncs->IsGameUIActive())
						{
							// without this the cursor stays hidden if we're displaying GameUI and we close the UI.
							// we have to disable the relative mouse mode, because it's suppoed to be on only when
							// we're ingame looking around with our camera.
							if (m_is_any_interactible_rendering_context_active)
							{
								SDL_SetRelativeMouseMode(FALSE);

								// centerize the cursor position, for convenience. This has to be called when the relative
								// mode is off, because we'll use our cursor now (in UI).
								surfacefuncs->SurfaceSetCursorPos(wide / 2, tall / 2);
							}
						}
						else
						{
							// enable the relative mode when we close UI, disable otherwise.
							SDL_SetRelativeMouseMode(!m_is_any_interactible_rendering_context_active);

							if (m_is_any_interactible_rendering_context_active)
							{
								// centerize the cursor pos once again, for convenience. This has to be again, 
								// called when the relative mode is off.
								surfacefuncs->SurfaceSetCursorPos(wide / 2, tall / 2);
							}

							// call this function for nothing before the engine. It seems that this resets the 
							// delta internally somehow inside of SDL. I know that this is pretty diddly and wacky,
							// but it works...
							int deltaX, deltaY;
							SDL_GetRelativeMouseState(&deltaX, &deltaY);

#ifdef MOUSEINPUT_DEBUG
							CConsole::the().info("delta: [{}, {}]", deltaX, deltaY);
#endif
						}
					}
				}
			}
		}
		else
		{
			// WinAPI used to be used in these builds to handle key&mouse inputs + m_rawinput is nonexistent.
		}

		last = m_is_any_interactible_rendering_context_active;
	}

	// to prevent some stupid bugs from leaving the mouse not active even tho it's supposed to be..
	// TODO: maybe it can work consistently without this, even?
	if (CGameUtil::the().is_fully_connected())
	{
		if (m_is_any_interactible_rendering_context_active)
		{
			CMemoryHookMgr::the().cl_funcs().get()->pfnHUD_IN_DeactivateMouse();
		}
		else
		{
			CMemoryHookMgr::the().cl_funcs().get()->pfnHUD_IN_ActivateMouse();
		}
	}
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
	g_gui_widgets_i->set_next_window_size(m_popup_window_size, (m_popup_window_flags & ImGuiWindowFlags_NoResize) ? ImGuiCond_Always : ImGuiCond_Once);

	g_gui_thememgr_i->push_color(GUICLR_ChildBackground, {});

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
